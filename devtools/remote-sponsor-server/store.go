package main

import (
	"bytes"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"sync"
	"unicode/utf8"
)

const dataFormatVersion = 1

// Player is one row of the sponsor/ban list.
type Player struct {
	AuthID      string `json:"auth_id"`
	Name        string `json:"name"`
	Description string `json:"description"`
	Level       int    `json:"level"`
}

// dataFile is the on-disk JSON document.
type dataFile struct {
	Version int      `json:"version"`
	Players []Player `json:"players"`
}

// Store owns the in-memory list and keeps the JSON file in sync.
//
// writeMu serializes reloads and mutations so a reload cannot replace a newer
// in-memory list written by a concurrent admin request. mu protects the map
// itself and allows lookups to proceed while the file is being rewritten.
type Store struct {
	writeMu sync.Mutex
	mu      sync.RWMutex
	path    string
	players map[string]Player
}

func NewStore(path string) (*Store, error) {
	players, err := loadJSON(path)
	if err != nil {
		if !errors.Is(err, os.ErrNotExist) {
			return nil, err
		}
		log.Printf("list %s does not exist yet; starting with an empty list", path)
		players = make(map[string]Player)
	}

	return &Store{path: path, players: players}, nil
}

// Reload replaces the in-memory list with the contents of the JSON file.
func (s *Store) Reload() error {
	s.writeMu.Lock()
	defer s.writeMu.Unlock()

	players, err := loadJSON(s.path)
	if err != nil {
		return err
	}

	s.mu.Lock()
	s.players = players
	s.mu.Unlock()

	return nil
}

func loadJSON(path string) (map[string]Player, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}

	// Tolerate a UTF-8 BOM, which some editors add.
	data = bytes.TrimPrefix(data, []byte{0xEF, 0xBB, 0xBF})

	var file dataFile

	decoder := json.NewDecoder(bytes.NewReader(data))
	decoder.DisallowUnknownFields()

	if err := decoder.Decode(&file); err != nil {
		return nil, fmt.Errorf("read %s: %w", path, err)
	}

	var extra any
	if err := decoder.Decode(&extra); !errors.Is(err, io.EOF) {
		return nil, fmt.Errorf("read %s: unexpected trailing data", path)
	}

	if file.Version != dataFormatVersion {
		return nil, fmt.Errorf("read %s: unsupported version %d (expected %d)", path, file.Version, dataFormatVersion)
	}

	players := make(map[string]Player, len(file.Players))

	for index, player := range file.Players {
		player, err := validatePlayer(player)
		if err != nil {
			return nil, fmt.Errorf("read %s player %d: %w", path, index, err)
		}
		if _, exists := players[player.AuthID]; exists {
			return nil, fmt.Errorf("read %s player %d: duplicate auth_id %q", path, index, player.AuthID)
		}

		players[player.AuthID] = player
	}

	return players, nil
}

func validatePlayer(player Player) (Player, error) {
	player.AuthID = strings.TrimSpace(player.AuthID)
	if player.AuthID == "" {
		return Player{}, errors.New("auth_id must not be empty")
	}
	if len(player.AuthID) > maxAuthIDLength {
		return Player{}, fmt.Errorf("auth_id must not be longer than %d bytes", maxAuthIDLength)
	}
	if player.Level < minLevel || player.Level > maxLevel {
		return Player{}, fmt.Errorf("level must be between %d and %d", minLevel, maxLevel)
	}

	player.Name = sanitizeField(player.Name, maxNameLength, defaultPlayerName)
	player.Description = sanitizeField(player.Description, maxDescriptionLength, defaultPlayerDesc)

	return player, nil
}

// sanitizeField keeps the value safe for the pipe/quoted response format used
// by the game DLL. Truncation happens on UTF-8 rune boundaries.
func sanitizeField(value string, limit int, fallback string) string {
	var builder strings.Builder

	for _, character := range value {
		switch {
		case character == '|' || character == '"' || character == 0x0D || character == 0x0A || character == 0:
			builder.WriteByte(' ')
		case character < 0x20:
			builder.WriteByte(' ')
		default:
			builder.WriteRune(character)
		}
	}

	value = strings.TrimSpace(builder.String())
	if value == "" {
		return fallback
	}

	return truncateUTF8(value, limit)
}

func truncateUTF8(value string, limit int) string {
	if len(value) <= limit {
		return value
	}

	for limit > 0 && !utf8.RuneStart(value[limit]) {
		limit--
	}

	return value[:limit]
}

// saveLocked writes the current map to disk. The caller must hold s.mu.
func (s *Store) saveLocked() error {
	directory := filepath.Dir(s.path)
	if err := os.MkdirAll(directory, 0o755); err != nil {
		return err
	}

	tempFile, err := os.CreateTemp(directory, ".players-*.json")
	if err != nil {
		return err
	}
	tempName := tempFile.Name()

	defer os.Remove(tempName)

	players := make([]Player, 0, len(s.players))
	for _, player := range s.players {
		players = append(players, player)
	}
	sort.Slice(players, func(i, j int) bool {
		return players[i].AuthID < players[j].AuthID
	})

	encoder := json.NewEncoder(tempFile)
	encoder.SetEscapeHTML(false)
	encoder.SetIndent("", "  ")

	if err := encoder.Encode(dataFile{Version: dataFormatVersion, Players: players}); err != nil {
		tempFile.Close()
		return err
	}
	if err := tempFile.Sync(); err != nil {
		tempFile.Close()
		return err
	}
	if err := tempFile.Close(); err != nil {
		return err
	}
	if err := os.Chmod(tempName, 0o644); err != nil {
		return err
	}

	return os.Rename(tempName, s.path)
}

func (s *Store) Lookup(authID string) (Player, bool) {
	s.mu.RLock()
	defer s.mu.RUnlock()

	player, found := s.players[authID]
	return player, found
}

func (s *Store) List() []Player {
	s.mu.RLock()
	defer s.mu.RUnlock()

	players := make([]Player, 0, len(s.players))
	for _, player := range s.players {
		players = append(players, player)
	}
	sort.Slice(players, func(i, j int) bool {
		return players[i].AuthID < players[j].AuthID
	})

	return players
}

func (s *Store) Count() int {
	s.mu.RLock()
	defer s.mu.RUnlock()

	return len(s.players)
}

func (s *Store) Upsert(player Player) (Player, bool, error) {
	player, err := validatePlayer(player)
	if err != nil {
		return Player{}, false, err
	}

	s.writeMu.Lock()
	defer s.writeMu.Unlock()

	s.mu.Lock()
	defer s.mu.Unlock()

	oldPlayer, existed := s.players[player.AuthID]
	s.players[player.AuthID] = player

	if err := s.saveLocked(); err != nil {
		if existed {
			s.players[player.AuthID] = oldPlayer
		} else {
			delete(s.players, player.AuthID)
		}
		return Player{}, false, err
	}

	return player, existed, nil
}

func (s *Store) Delete(authID string) (bool, error) {
	authID = strings.TrimSpace(authID)

	s.writeMu.Lock()
	defer s.writeMu.Unlock()

	s.mu.Lock()
	defer s.mu.Unlock()

	oldPlayer, existed := s.players[authID]
	if !existed {
		return false, nil
	}

	delete(s.players, authID)

	if err := s.saveLocked(); err != nil {
		s.players[authID] = oldPlayer
		return false, err
	}

	return true, nil
}
