package main

import (
	"encoding/hex"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func TestCryptMatchesReference(t *testing.T) {
	plain := []byte(strings.Join([]string{
		"ipaddr=127.0.0.1",
		"timestamp=1234567890",
		"authid=STEAM_0:0:100000001",
		"",
	}, testNewline))
	got, err := crypt(defaultKey, plain)
	if err != nil {
		t.Fatalf("crypt: %v", err)
	}

	want := "8e2275e400aca6ff8be78aa25b053216909a17bc1bfcbc04a7782a1be7db71bde3fabdbed5c93c6914ece36bedac974bcbf9979c38789135f4e5cd09fdf5733a00"
	if hex.EncodeToString(got) != want {
		t.Fatalf("ciphertext = %x, want %s", got, want)
	}
}

func TestCryptRoundTrip(t *testing.T) {
	plain := []byte("hello world")

	encrypted, err := crypt(defaultKey, plain)
	if err != nil {
		t.Fatalf("crypt: %v", err)
	}

	decrypted, err := crypt(defaultKey, encrypted)
	if err != nil {
		t.Fatalf("crypt: %v", err)
	}

	if string(decrypted) != string(plain) {
		t.Fatalf("round trip = %q, want %q", decrypted, plain)
	}
}

func TestLoadExampleJSON(t *testing.T) {
	store, err := NewStore("players.example.json")
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

func TestCleanTimeZone(t *testing.T) {
	if got := cleanTimeZone("  "); got != defaultTimeZone {
		t.Fatalf("cleanTimeZone(blank) = %q, want %q", got, defaultTimeZone)
	}
	if got := cleanTimeZone("|CET\n"); got != "CET" {
		t.Fatalf("cleanTimeZone(pipe) = %q, want %q", got, "CET")
	}
	if got := cleanTimeZone("Europe/Berlin"); got != "Europe/" {
		t.Fatalf("cleanTimeZone(long) = %q, want %q", got, "Europe/")
	}
}

func TestResolveAdminToken(t *testing.T) {
	if token, err := resolveAdminToken("", ""); err != nil || token != "" {
		t.Fatalf("empty token = %q, %v; want empty and no error", token, err)
	}

	if _, err := resolveAdminToken("short", ""); err == nil {
		t.Fatal("short token should be rejected")
	}

	token, err := resolveAdminToken("  "+testAdminToken+"  ", "")
	if err != nil || token != testAdminToken {
		t.Fatalf("token = %q, %v; want %q", token, err, testAdminToken)
	}

	if _, err := resolveAdminToken(testAdminToken, "ignored"); err == nil {
		t.Fatal("specifying both a token and a token file should fail")
	}
}

func TestResolveAdminTokenFromFile(t *testing.T) {
	directory := t.TempDir()
	path := filepath.Join(directory, "admin-token")

	if err := os.WriteFile(path, []byte(testAdminToken+testNewline), 0o600); err != nil {
		t.Fatalf("write token: %v", err)
	}

	token, err := resolveAdminToken("", path)
	if err != nil {
		t.Fatalf("resolveAdminToken: %v", err)
	}
	if token != testAdminToken {
		t.Fatalf("token = %q, want %q", token, testAdminToken)
	}

	emptyPath := filepath.Join(directory, "empty-token")
	if err := os.WriteFile(emptyPath, []byte(testNewline), 0o600); err != nil {
		t.Fatalf("write empty token: %v", err)
	}

	if _, err := resolveAdminToken("", emptyPath); err == nil {
		t.Fatal("an empty token file should be rejected")
	}
}

func TestEnabledText(t *testing.T) {
	if enabledText(true) != "enabled" || enabledText(false) != "disabled" {
		t.Fatal("enabledText returned an unexpected value")
	}
}

func TestParseLookupRequest(t *testing.T) {
	request, err := parseLookupRequest([]byte("ipaddr=1.2.3.4\ntimestamp=10\nauthid=STEAM_0:1:2\n"))
	if err != nil {
		t.Fatalf("parseLookupRequest: %v", err)
	}
	if request.IP != "1.2.3.4" || request.Timestamp != 10 || request.AuthID != "STEAM_0:1:2" {
		t.Fatalf("request = %+v", request)
	}

	for _, invalid := range []string{
		"timestamp=10\nauthid=STEAM_0:1:2\n",
		"ipaddr=1.2.3.4\nauthid=STEAM_0:1:2\n",
		"ipaddr=1.2.3.4\ntimestamp=0\nauthid=STEAM_0:1:2\n",
		"ipaddr=1.2.3.4\ntimestamp=10\n",
	} {
		if _, err := parseLookupRequest([]byte(invalid)); err == nil {
			t.Fatalf("parseLookupRequest(%q) should fail", invalid)
		}
	}
}

func TestParseLookupRequestRejectsLongFields(t *testing.T) {
	longIP := strings.Repeat("1", maxIPLength+1)
	if _, err := parseLookupRequest([]byte("ipaddr=" + longIP + "\ntimestamp=10\nauthid=STEAM_0:1:2\n")); err == nil {
		t.Fatal("an oversized ipaddr should be rejected")
	}

	longAuthID := strings.Repeat("A", maxAuthIDLength+1)
	if _, err := parseLookupRequest([]byte("ipaddr=1.2.3.4\ntimestamp=10\nauthid=" + longAuthID + "\n")); err == nil {
		t.Fatal("an oversized authid should be rejected")
	}
}
