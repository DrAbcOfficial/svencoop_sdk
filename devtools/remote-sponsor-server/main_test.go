package main

import (
	"bytes"
	"crypto/rc4"
	"encoding/hex"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"path/filepath"
	"strings"
	"testing"
)

func newTestApp(t *testing.T) *App {
	t.Helper()

	store := &Store{
		path: filepath.Join(t.TempDir(), "players.csv"),
		players: map[string]Player{
			"STEAM_0:0:100000001": {
				AuthID:      "STEAM_0:0:100000001",
				Name:        "Example Donor",
				Description: "Example donor",
				Level:       1,
			},
			"STEAM_0:0:100000003": {
				AuthID:      "STEAM_0:0:100000003",
				Name:        "Example Banned",
				Description: "Example ban",
				Level:       -1,
			},
		},
	}

	return &App{
		store:      store,
		key:        defaultKey,
		timeZone:   defaultTimeZone,
		lookupPath: defaultLookupPath,
		adminToken: "secret",
	}
}

func encryptForTest(t *testing.T, key string, input []byte) []byte {
	t.Helper()

	cipher, err := rc4.NewCipher([]byte(key))
	if err != nil {
		t.Fatalf("rc4.NewCipher: %v", err)
	}

	output := make([]byte, len(input))
	cipher.XORKeyStream(output, input)
	return output
}

func decryptForTest(t *testing.T, key string, input []byte) []byte {
	t.Helper()
	return encryptForTest(t, key, input)
}

func TestCryptMatchesReference(t *testing.T) {
	plain := []byte("ipaddr=127.0.0.1\ntimestamp=1234567890\nauthid=STEAM_0:0:100000001\n")
	got, err := crypt(defaultKey, plain)
	if err != nil {
		t.Fatalf("crypt: %v", err)
	}

	want := "8e2275e400aca6ff8be78aa25b053216909a17bc1bfcbc04a7782a1be7db71bde3fabdbed5c93c6914ece36bedac974bcbf9979c38789135f4e5cd09fdf5733a00"
	if hex.EncodeToString(got) != want {
		t.Fatalf("ciphertext = %x, want %s", got, want)
	}
}

func TestLoadExampleCSV(t *testing.T) {
	store, err := NewStore("players.example.csv")
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}

	if store.Count() != 3 {
		t.Fatalf("count = %d, want 3", store.Count())
	}

	player, found := store.Lookup("STEAM_0:0:100000003")
	if !found || player.Level != -1 {
		t.Fatalf("banned player = %+v, found = %t", player, found)
	}
}

func TestLookupKnownPlayer(t *testing.T) {
	app := newTestApp(t)
	body := encryptForTest(t, defaultKey, []byte("ipaddr=127.0.0.1\ntimestamp=1234567890\nauthid=STEAM_0:0:100000001\n"))

	request := httptest.NewRequest(http.MethodPut, defaultLookupPath, bytes.NewReader(body))
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d (%s)", recorder.Code, http.StatusOK, recorder.Body.String())
	}

	response := string(decryptForTest(t, defaultKey, recorder.Body.Bytes()))
	want := "STEAM_0:0:100000001|1234567890|127.0.0.1|UTC|\"Example Donor\"|\"Example donor\"|1"
	if response != want {
		t.Fatalf("response = %q, want %q", response, want)
	}
}

func TestLookupUnknownPlayer(t *testing.T) {
	app := newTestApp(t)
	body := encryptForTest(t, defaultKey, []byte("ipaddr=10.0.0.1\ntimestamp=42\nauthid=STEAM_0:0:999999999\n"))

	request := httptest.NewRequest(http.MethodPut, defaultLookupPath, bytes.NewReader(body))
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d (%s)", recorder.Code, http.StatusOK, recorder.Body.String())
	}

	response := string(decryptForTest(t, defaultKey, recorder.Body.Bytes()))
	want := "STEAM_0:0:999999999|42|10.0.0.1|UTC|\"Player\"|\"-\"|0"
	if response != want {
		t.Fatalf("response = %q, want %q", response, want)
	}
}

func TestLookupRejectsBadRequest(t *testing.T) {
	app := newTestApp(t)
	body := encryptForTest(t, defaultKey, []byte("ipaddr=127.0.0.1\nauthid=STEAM_0:0:100000001\n"))

	request := httptest.NewRequest(http.MethodPut, defaultLookupPath, bytes.NewReader(body))
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusBadRequest {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusBadRequest)
	}
	if !strings.Contains(recorder.Body.String(), "[Critical Error]") {
		t.Fatalf("body = %q, want a [Critical Error] marker", recorder.Body.String())
	}
}

func TestAdminUpsertAndDelete(t *testing.T) {
	app := newTestApp(t)
	router := app.routes()

	upsertBody := "{\"auth_id\":\"STEAM_0:0:100000002\",\"name\":\"New Player\",\"description\":\"Added by admin\",\"level\":2}"
	upsertRequest := httptest.NewRequest(http.MethodPost, adminPlayersPath, strings.NewReader(upsertBody))
	upsertRequest.Header.Set("Authorization", "Bearer secret")
	upsertRecorder := httptest.NewRecorder()
	router.ServeHTTP(upsertRecorder, upsertRequest)

	if upsertRecorder.Code != http.StatusCreated {
		t.Fatalf("upsert status = %d, want %d (%s)", upsertRecorder.Code, http.StatusCreated, upsertRecorder.Body.String())
	}

	if player, found := app.store.Lookup("STEAM_0:0:100000002"); !found || player.Level != 2 {
		t.Fatalf("upserted player = %+v, found = %t", player, found)
	}

	deleteRequest := httptest.NewRequest(http.MethodDelete, adminPlayersPath+"/STEAM_0:0:100000002", nil)
	deleteRequest.Header.Set("X-Admin-Token", "secret")
	deleteRecorder := httptest.NewRecorder()
	router.ServeHTTP(deleteRecorder, deleteRequest)

	if deleteRecorder.Code != http.StatusNoContent {
		t.Fatalf("delete status = %d, want %d (%s)", deleteRecorder.Code, http.StatusNoContent, deleteRecorder.Body.String())
	}

	reloaded, err := NewStore(app.store.path)
	if err != nil {
		t.Fatalf("NewStore: %v", err)
	}
	if reloaded.Count() != 2 {
		t.Fatalf("reloaded count = %d, want 2", reloaded.Count())
	}
}

func TestAdminRequiresToken(t *testing.T) {
	app := newTestApp(t)
	request := httptest.NewRequest(http.MethodGet, adminPlayersPath, nil)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusUnauthorized {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusUnauthorized)
	}

	app.adminToken = ""
	recorder = httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusForbidden {
		t.Fatalf("disabled admin status = %d, want %d", recorder.Code, http.StatusForbidden)
	}
}

func TestAdminList(t *testing.T) {
	app := newTestApp(t)
	request := httptest.NewRequest(http.MethodGet, adminPlayersPath, nil)
	request.Header.Set("Authorization", "Bearer secret")
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusOK)
	}

	var result struct {
		Count   int      `json:"count"`
		Players []Player `json:"players"`
	}
	if err := json.Unmarshal(recorder.Body.Bytes(), &result); err != nil {
		t.Fatalf("decode response: %v", err)
	}
	if result.Count != 2 || len(result.Players) != 2 {
		t.Fatalf("result = %+v, want two players", result)
	}
}

func TestSanitizeField(t *testing.T) {
	got := sanitizeField("  Name|with\"quotes\n  ", maxNameLength, defaultPlayerName)
	want := "Name with quotes"
	if got != want {
		t.Fatalf("sanitizeField = %q, want %q", got, want)
	}

	if got := sanitizeField("   ", maxNameLength, defaultPlayerName); got != defaultPlayerName {
		t.Fatalf("empty sanitizeField = %q, want %q", got, defaultPlayerName)
	}
}

func TestTruncateUTF8(t *testing.T) {
	if got := truncateUTF8("abcdef", 3); got != "abc" {
		t.Fatalf("truncateUTF8 = %q, want %q", got, "abc")
	}
	if got := truncateUTF8("你好世界", 4); got != "你" {
		t.Fatalf("truncateUTF8 = %q, want %q", got, "你")
	}
}
