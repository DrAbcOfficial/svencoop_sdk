package main

import (
	"fmt"
	"os"
	"path/filepath"
	"sync"
	"testing"
)

func TestStoreCRUD(t *testing.T) {
	store, err := NewStore(filepath.Join(t.TempDir(), "players.csv"))
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
	path := filepath.Join(t.TempDir(), "players.csv")
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

func TestStoreReloadReplacesContents(t *testing.T) {
	path := filepath.Join(t.TempDir(), "players.csv")
	store, err := NewStore(path)
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}

	if _, _, err := store.Upsert(Player{AuthID: "STEAM_0:0:1", Name: "Old", Description: "Old", Level: 1}); err != nil {
		t.Fatalf("Upsert: %v", err)
	}

	content := csvLine("STEAM_0:0:2", "New", "New", "3")

	if err := os.WriteFile(path, []byte(content), 0o644); err != nil {
		t.Fatalf("write CSV: %v", err)
	}

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
	store, err := NewStore(filepath.Join(t.TempDir(), "players.csv"))
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

func TestLoadCSVRejectsDuplicateAuthIDs(t *testing.T) {
	path := filepath.Join(t.TempDir(), "players.csv")
	content := csvLine("STEAM_0:0:1", "A", "A", "1") + csvLine("STEAM_0:0:1", "B", "B", "2")

	if err := os.WriteFile(path, []byte(content), 0o644); err != nil {
		t.Fatalf("write CSV: %v", err)
	}

	if _, err := loadCSV(path); err == nil {
		t.Fatal("duplicate auth IDs should be rejected")
	}
}

func TestStoreConcurrentAccess(t *testing.T) {
	path := filepath.Join(t.TempDir(), "players.csv")
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

	if _, err := loadCSV(path); err != nil {
		t.Fatalf("final CSV is invalid: %v", err)
	}
}
