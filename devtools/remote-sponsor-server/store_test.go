package main

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sync"
	"testing"
)

func TestStoreCRUD(t *testing.T) {
	store, err := NewStore(filepath.Join(t.TempDir(), "players.json"))
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}

	player, existed, err := store.Upsert(Player{AuthID: "STEAM_0:0:1", Name: "Donor", Description: "Donor", Level: 1})
	if err != nil {
		t.Fatalf("Upsert: %v", err)
	}
	if existed {
		t.Fatal("first upsert should create a new player")
	}
	if player.AuthID != "STEAM_0:0:1" {
		t.Fatalf("auth id = %q", player.AuthID)
	}

	if store.Count() != 1 {
		t.Fatalf("count = %d, want 1", store.Count())
	}

	if _, found := store.Lookup("STEAM_0:0:1"); !found {
		t.Fatal("expected player to exist")
	}
	if _, found := store.Lookup("STEAM_0:0:2"); found {
		t.Fatal("unexpected player")
	}

	if _, existed, err := store.Upsert(Player{AuthID: "STEAM_0:0:1", Name: "Donor", Description: "Donor", Level: 4}); err != nil || !existed {
		t.Fatalf("second upsert = %v, existed = %t", err, existed)
	}

	players := store.List()
	if len(players) != 1 || players[0].Level != 4 {
		t.Fatalf("players = %+v", players)
	}

	if deleted, err := store.Delete("STEAM_0:0:1"); err != nil || !deleted {
		t.Fatalf("Delete = %t, %v", deleted, err)
	}
	if deleted, err := store.Delete("STEAM_0:0:1"); err != nil || deleted {
		t.Fatalf("second Delete = %t, %v", deleted, err)
	}

	if store.Count() != 0 {
		t.Fatalf("count = %d, want 0", store.Count())
	}
}

func TestStorePersistsToDisk(t *testing.T) {
	path := filepath.Join(t.TempDir(), "players.json")
	store, err := NewStore(path)
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}

	if _, _, err := store.Upsert(Player{AuthID: "STEAM_0:0:7", Name: "Saved", Description: "Saved", Level: 2}); err != nil {
		t.Fatalf("Upsert: %v", err)
	}

	reloaded, err := NewStore(path)
	if err != nil {
		t.Fatalf("NewStore reload: %v", err)
	}

	player, found := reloaded.Lookup("STEAM_0:0:7")
	if !found || player.Name != "Saved" || player.Level != 2 {
		t.Fatalf("reloaded player = %+v, found = %t", player, found)
	}
}

func TestStoreWritesVersionedJSON(t *testing.T) {
	path := filepath.Join(t.TempDir(), "players.json")
	store, err := NewStore(path)
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}

	if _, _, err := store.Upsert(Player{AuthID: "STEAM_0:0:3", Name: "Saved", Description: "Saved", Level: 2}); err != nil {
		t.Fatalf("Upsert: %v", err)
	}

	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatalf("read file: %v", err)
	}

	var file dataFile
	if err := json.Unmarshal(data, &file); err != nil {
		t.Fatalf("unmarshal file: %v", err)
	}
	if file.Version != dataFormatVersion {
		t.Fatalf("version = %d, want %d", file.Version, dataFormatVersion)
	}
	if len(file.Players) != 1 || file.Players[0].AuthID != "STEAM_0:0:3" {
		t.Fatalf("players = %+v", file.Players)
	}
}

func TestStoreReloadReplacesContents(t *testing.T) {
	path := filepath.Join(t.TempDir(), "players.json")
	store, err := NewStore(path)
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}

	if _, _, err := store.Upsert(Player{AuthID: "STEAM_0:0:1", Name: "Old", Description: "Old", Level: 1}); err != nil {
		t.Fatalf("Upsert: %v", err)
	}

	writeJSONStore(t, path, Player{AuthID: "STEAM_0:0:2", Name: "New", Description: "New", Level: 3})

	if err := store.Reload(); err != nil {
		t.Fatalf("Reload: %v", err)
	}

	if _, found := store.Lookup("STEAM_0:0:1"); found {
		t.Fatal("old player should be gone after reload")
	}
	if player, found := store.Lookup("STEAM_0:0:2"); !found || player.Level != 3 {
		t.Fatalf("new player = %+v, found = %t", player, found)
	}
}

func TestStoreRejectsInvalidPlayers(t *testing.T) {
	store, err := NewStore(filepath.Join(t.TempDir(), "players.json"))
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}

	invalid := []Player{
		{AuthID: "", Name: "NoId", Description: "NoId", Level: 0},
		{AuthID: "STEAM_0:0:1", Name: "BadLevel", Description: "BadLevel", Level: 7},
		{AuthID: "STEAM_0:0:1", Name: "BadLevel", Description: "BadLevel", Level: -2},
	}

	for _, player := range invalid {
		if _, _, err := store.Upsert(player); err == nil {
			t.Fatalf("Upsert(%+v) should fail", player)
		}
	}
}

func TestLoadJSONRejectsDuplicateAuthIDs(t *testing.T) {
	path := filepath.Join(t.TempDir(), "players.json")
	writeJSONStore(t, path,
		Player{AuthID: "STEAM_0:0:1", Name: "A", Description: "A", Level: 1},
		Player{AuthID: "STEAM_0:0:1", Name: "B", Description: "B", Level: 2},
	)

	if _, err := loadJSON(path); err == nil {
		t.Fatal("duplicate auth IDs should be rejected")
	}
}

func TestStoreConcurrentAccess(t *testing.T) {
	path := filepath.Join(t.TempDir(), "players.json")
	store, err := NewStore(path)
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}

	if _, _, err := store.Upsert(Player{AuthID: "STEAM_0:0:0", Name: "Seed", Description: "Seed", Level: 0}); err != nil {
		t.Fatalf("seed Upsert: %v", err)
	}

	const workers = 8
	const iterations = 20

	var waitGroup sync.WaitGroup

	for worker := 0; worker < workers; worker++ {
		waitGroup.Add(1)

		go func(worker int) {
			defer waitGroup.Done()

			for iteration := 0; iteration < iterations; iteration++ {
				authID := fmt.Sprintf("STEAM_0:0:%d", worker*1000+iteration)

				if _, _, err := store.Upsert(Player{AuthID: authID, Name: "Concurrent", Description: "Concurrent", Level: 1}); err != nil {
					t.Errorf("Upsert(%s): %v", authID, err)
					return
				}

				store.Lookup(authID)
				store.List()
				store.Count()

				if iteration%5 == 0 {
					if _, err := store.Delete(authID); err != nil {
						t.Errorf("Delete(%s): %v", authID, err)
						return
					}
				}

				if iteration%10 == 0 {
					if err := store.Reload(); err != nil {
						t.Errorf("Reload: %v", err)
						return
					}
				}
			}
		}(worker)
	}

	waitGroup.Wait()

	if _, err := loadJSON(path); err != nil {
		t.Fatalf("final JSON is invalid: %v", err)
	}
}
