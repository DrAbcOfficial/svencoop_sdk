// Command remote-sponsor-server is a minimal Sven Co-op player extras
// (sponsor/ban list) server. It speaks the same RC4-protected protocol as the
// server DLL, stores its list in a CSV file, and exposes an optional admin API.
package main

import (
	"bufio"
	"bytes"
	"context"
	"crypto/rc4"
	"crypto/subtle"
	"encoding/csv"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"os/signal"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"
	"unicode/utf8"
)

const (
	defaultKey = "j.6v,A?cPk4)zL[89$~GTBM@fmR-(U{;u&3qY]eh/p}sbX#Z*75Jd=nE%DC_:xiK"

	maxBodySize          = 4096
	maxAuthIDLength      = 44
	maxNameLength        = 32
	maxDescriptionLength = 128
	maxIPLength          = 64
	maxTimeZoneLength    = 7
	maxAdminRequestSize  = 1 << 20
	defaultLookupPath    = "/playerextras.php"
	defaultTimeZone      = "UTC"
	defaultListenAddress = ":8080"
	defaultListFileName  = "players.csv"
	adminPlayersPath     = "/admin/players"
	adminReloadPath      = "/admin/reload"
	healthPath           = "/healthz"
	minLevel             = -1
	maxLevel             = 6
	defaultPlayerName    = "Player"
	defaultPlayerDesc    = "-"
)

// Player is one row of the sponsor/ban list.
type Player struct {
	AuthID      string `json:"auth_id"`
	Name        string `json:"name"`
	Description string `json:"description"`
	Level       int    `json:"level"`
}

// Store owns the in-memory list and keeps the CSV file in sync.
type Store struct {
	mu      sync.RWMutex
	path    string
	players map[string]Player
}

func NewStore(path string) (*Store, error) {
	players, err := loadCSV(path)
	if err != nil {
		if !errors.Is(err, os.ErrNotExist) {
			return nil, err
		}
		log.Printf("list %s does not exist yet; starting with an empty list", path)
		players = make(map[string]Player)
	}

	return &Store{path: path, players: players}, nil
}

// Reload replaces the in-memory list with the contents of the CSV file.
func (s *Store) Reload() error {
	players, err := loadCSV(s.path)
	if err != nil {
		return err
	}

	s.mu.Lock()
	s.players = players
	s.mu.Unlock()
	return nil
}

func loadCSV(path string) (map[string]Player, error) {
	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	reader := bufio.NewReader(file)

	// Tolerate a UTF-8 BOM, which some spreadsheet exports add.
	if prefix, err := reader.Peek(3); err == nil && bytes.Equal(prefix, []byte{0xEF, 0xBB, 0xBF}) {
		if _, err := reader.Discard(3); err != nil {
			return nil, err
		}
	}

	csvReader := csv.NewReader(reader)
	csvReader.FieldsPerRecord = 4
	csvReader.TrimLeadingSpace = true

	players := make(map[string]Player)
	rowNumber := 0

	for {
		rowNumber++

		record, err := csvReader.Read()
		if errors.Is(err, io.EOF) {
			break
		}
		if err != nil {
			return nil, fmt.Errorf("read %s row %d: %w", path, rowNumber, err)
		}

		player, err := parsePlayer(record[0], record[1], record[2], record[3])
		if err != nil {
			return nil, fmt.Errorf("read %s row %d: %w", path, rowNumber, err)
		}
		if _, exists := players[player.AuthID]; exists {
			return nil, fmt.Errorf("read %s row %d: duplicate auth_id %q", path, rowNumber, player.AuthID)
		}

		players[player.AuthID] = player
	}

	return players, nil
}

func parsePlayer(authID, name, description, levelText string) (Player, error) {
	level, err := strconv.Atoi(strings.TrimSpace(levelText))
	if err != nil {
		return Player{}, fmt.Errorf("level must be a whole number")
	}

	return validatePlayer(Player{
		AuthID:      authID,
		Name:        name,
		Description: description,
		Level:       level,
	})
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
		case character == '|' || character == '"' || character == '\r' || character == '\n' || character == 0:
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

func (s *Store) saveLocked() error {
	directory := filepath.Dir(s.path)
	if err := os.MkdirAll(directory, 0o755); err != nil {
		return err
	}

	tempFile, err := os.CreateTemp(directory, ".players-*.csv")
	if err != nil {
		return err
	}
	tempName := tempFile.Name()

	defer os.Remove(tempName)

	writer := csv.NewWriter(tempFile)
	authIDs := make([]string, 0, len(s.players))

	for authID := range s.players {
		authIDs = append(authIDs, authID)
	}
	sort.Strings(authIDs)

	for _, authID := range authIDs {
		player := s.players[authID]

		if err := writer.Write([]string{
			player.AuthID,
			player.Name,
			player.Description,
			strconv.Itoa(player.Level),
		}); err != nil {
			tempFile.Close()
			return err
		}
	}

	writer.Flush()
	if err := writer.Error(); err != nil {
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

// App holds the HTTP configuration.
type App struct {
	store      *Store
	key        string
	timeZone   string
	lookupPath string
	adminToken string
}

func (a *App) routes() http.Handler {
	mux := http.NewServeMux()

	mux.HandleFunc("GET "+healthPath, a.handleHealth)
	mux.HandleFunc("GET "+adminPlayersPath, a.requireAdmin(a.handleListPlayers))
	mux.HandleFunc("POST "+adminPlayersPath, a.requireAdmin(a.handleUpsertPlayer))
	mux.HandleFunc("DELETE "+adminPlayersPath+"/{authid}", a.requireAdmin(a.handleDeletePlayer))
	mux.HandleFunc("POST "+adminReloadPath, a.requireAdmin(a.handleReload))
	mux.HandleFunc(a.lookupPath, a.handleLookup)
	if a.lookupPath != "/" {
		mux.HandleFunc("/", a.handleLookup)
	}

	return mux
}

func (a *App) handleHealth(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	_, _ = io.WriteString(w, "ok\n")
}

func (a *App) handleLookup(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPut && r.Method != http.MethodPost {
		protocolError(w, http.StatusMethodNotAllowed, "method not allowed")
		return
	}

	body, err := io.ReadAll(io.LimitReader(r.Body, maxBodySize+1))
	if err != nil {
		protocolError(w, http.StatusBadRequest, "cannot read request")
		return
	}
	if len(body) > maxBodySize {
		protocolError(w, http.StatusRequestEntityTooLarge, "request is too large")
		return
	}

	plain, err := crypt(a.key, body)
	if err != nil {
		protocolError(w, http.StatusBadRequest, "cannot decrypt request")
		return
	}

	request, err := parseLookupRequest(plain)
	if err != nil {
		protocolError(w, http.StatusBadRequest, err.Error())
		return
	}

	player, found := a.store.Lookup(request.AuthID)
	if !found {
		player = Player{
			AuthID:      request.AuthID,
			Name:        defaultPlayerName,
			Description: defaultPlayerDesc,
			Level:       0,
		}
	}

	response := fmt.Sprintf("%s|%d|%s|%s|\"%s\"|\"%s\"|%d",
		request.AuthID,
		request.Timestamp,
		request.IP,
		a.timeZone,
		sanitizeField(player.Name, maxNameLength, defaultPlayerName),
		sanitizeField(player.Description, maxDescriptionLength, defaultPlayerDesc),
		player.Level,
	)

	encrypted, err := crypt(a.key, []byte(response))
	if err != nil {
		protocolError(w, http.StatusInternalServerError, "cannot encrypt response")
		return
	}

	w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	w.Header().Set("Cache-Control", "no-store")
	w.WriteHeader(http.StatusOK)
	_, _ = w.Write(encrypted)

	log.Printf("lookup auth_id=%q ip=%q level=%d known=%t", request.AuthID, request.IP, player.Level, found)
}

type lookupRequest struct {
	IP        string
	Timestamp int64
	AuthID    string
}

func parseLookupRequest(plain []byte) (lookupRequest, error) {
	var request lookupRequest
	var timestampText string

	for _, line := range strings.Split(string(plain), "\n") {
		line = strings.TrimSuffix(line, "\r")

		key, value, found := strings.Cut(line, "=")
		if !found {
			continue
		}

		switch key {
		case "ipaddr":
			request.IP = value
		case "timestamp":
			timestampText = value
		case "authid":
			request.AuthID = value
		}
	}

	if request.IP == "" || len(request.IP) > maxIPLength {
		return lookupRequest{}, errors.New("invalid ipaddr")
	}
	if request.AuthID == "" || len(request.AuthID) > maxAuthIDLength {
		return lookupRequest{}, errors.New("invalid authid")
	}

	timestamp, err := strconv.ParseInt(timestampText, 10, 64)
	if err != nil || timestamp <= 0 {
		return lookupRequest{}, errors.New("invalid timestamp")
	}
	request.Timestamp = timestamp

	return request, nil
}

func (a *App) requireAdmin(next http.HandlerFunc) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if a.adminToken == "" {
			http.Error(w, "admin API is disabled (set -admin-token)", http.StatusForbidden)
			return
		}

		token := r.Header.Get("X-Admin-Token")
		if token == "" {
			authorization := r.Header.Get("Authorization")
			if strings.HasPrefix(authorization, "Bearer ") {
				token = strings.TrimSpace(strings.TrimPrefix(authorization, "Bearer "))
			}
		}

		if subtle.ConstantTimeCompare([]byte(token), []byte(a.adminToken)) != 1 {
			w.Header().Set("WWW-Authenticate", `Bearer realm="admin"`)
			http.Error(w, "unauthorized", http.StatusUnauthorized)
			return
		}

		next(w, r)
	}
}

func (a *App) handleListPlayers(w http.ResponseWriter, r *http.Request) {
	players := a.store.List()

	writeJSON(w, http.StatusOK, map[string]any{
		"count":   len(players),
		"players": players,
	})
}

func (a *App) handleUpsertPlayer(w http.ResponseWriter, r *http.Request) {
	var player Player

	if err := decodeJSON(w, r, &player); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	player, existed, err := a.store.Upsert(player)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	status := http.StatusOK
	if !existed {
		status = http.StatusCreated
	}

	writeJSON(w, status, player)
}

func (a *App) handleDeletePlayer(w http.ResponseWriter, r *http.Request) {
	authID := r.PathValue("authid")

	deleted, err := a.store.Delete(authID)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	if !deleted {
		http.Error(w, "player not found", http.StatusNotFound)
		return
	}

	w.WriteHeader(http.StatusNoContent)
}

func (a *App) handleReload(w http.ResponseWriter, r *http.Request) {
	if err := a.store.Reload(); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	writeJSON(w, http.StatusOK, map[string]any{"count": a.store.Count()})
}

func decodeJSON(w http.ResponseWriter, r *http.Request, value any) error {
	decoder := json.NewDecoder(http.MaxBytesReader(w, r.Body, maxAdminRequestSize))
	decoder.DisallowUnknownFields()

	if err := decoder.Decode(value); err != nil {
		return fmt.Errorf("invalid JSON: %w", err)
	}

	var extra any
	if err := decoder.Decode(&extra); !errors.Is(err, io.EOF) {
		return errors.New("invalid JSON: expected a single object")
	}

	return nil
}

func writeJSON(w http.ResponseWriter, status int, value any) {
	w.Header().Set("Content-Type", "application/json; charset=utf-8")
	w.WriteHeader(status)
	_ = json.NewEncoder(w).Encode(value)
}

func protocolError(w http.ResponseWriter, status int, message string) {
	http.Error(w, "[Critical Error] "+message, status)
}

func crypt(key string, input []byte) ([]byte, error) {
	cipher, err := rc4.NewCipher([]byte(key))
	if err != nil {
		return nil, err
	}

	output := make([]byte, len(input))
	cipher.XORKeyStream(output, input)

	return output, nil
}

func cleanTimeZone(value string) string {
	value = strings.TrimSpace(value)
	if value == "" {
		return defaultTimeZone
	}

	value = strings.NewReplacer("|", "", "\r", "", "\n", "").Replace(value)
	value = strings.TrimSpace(value)
	if value == "" {
		return defaultTimeZone
	}

	return truncateUTF8(value, maxTimeZoneLength)
}

func main() {
	listen := flag.String("listen", defaultListenAddress, "HTTP listen address")
	listPath := flag.String("list", defaultListFileName, "CSV file with auth_id,name,description,level rows")
	key := flag.String("key", defaultKey, "RC4 key shared with the game DLL (64 characters by default)")
	timeZone := flag.String("timezone", defaultTimeZone, "timezone field returned to the game (at most 7 bytes)")
	lookupPath := flag.String("path", defaultLookupPath, "HTTP path used for player lookups")
	adminToken := flag.String("admin-token", "", "bearer token for the admin API; empty disables the admin API")
	flag.Parse()

	if *key == "" {
		log.Fatal("the RC4 key must not be empty")
	}
	if !strings.HasPrefix(*lookupPath, "/") {
		log.Fatal("the lookup path must start with /")
	}
	if *lookupPath == adminPlayersPath || *lookupPath == adminReloadPath || *lookupPath == healthPath {
		log.Fatal("the lookup path conflicts with a built-in endpoint")
	}

	store, err := NewStore(*listPath)
	if err != nil {
		log.Fatalf("cannot load %s: %v", *listPath, err)
	}

	app := &App{
		store:      store,
		key:        *key,
		timeZone:   cleanTimeZone(*timeZone),
		lookupPath: *lookupPath,
		adminToken: *adminToken,
	}

	server := &http.Server{
		Addr:              *listen,
		Handler:           app.routes(),
		ReadHeaderTimeout: 5 * time.Second,
		ReadTimeout:       10 * time.Second,
		WriteTimeout:      10 * time.Second,
		IdleTimeout:       60 * time.Second,
	}

	ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer stop()

	go func() {
		log.Printf("remote sponsor server listening on %s", *listen)
		log.Printf("lookup path %s, list %s (%d players), admin API %s",
			*lookupPath, *listPath, store.Count(), enabledText(*adminToken != ""))

		if err := server.ListenAndServe(); err != nil && !errors.Is(err, http.ErrServerClosed) {
			log.Printf("HTTP server error: %v", err)
			stop()
		}
	}()

	<-ctx.Done()
	log.Printf("shutting down")

	shutdownContext, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	if err := server.Shutdown(shutdownContext); err != nil {
		log.Printf("shutdown error: %v", err)
	}
}

func enabledText(enabled bool) string {
	if enabled {
		return "enabled"
	}

	return "disabled"
}
