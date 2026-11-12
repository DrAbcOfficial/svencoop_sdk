package main

import (
	"crypto/tls"
	"net/http"
	"net/http/httptest"
	"os"
	"testing"
	"time"
)

func TestParseTLSMinVersion(t *testing.T) {
	versions := map[string]uint16{
		"":       tls.VersionTLS12,
		"1.2":    tls.VersionTLS12,
		"tls12":  tls.VersionTLS12,
		"1.3":    tls.VersionTLS13,
		"TLS1.3": tls.VersionTLS13,
	}

	for value, want := range versions {
		got, err := parseTLSMinVersion(value)
		if err != nil {
			t.Fatalf("parseTLSMinVersion(%q): %v", value, err)
		}
		if got != want {
			t.Fatalf("parseTLSMinVersion(%q) = %d, want %d", value, got, want)
		}
	}

	if _, err := parseTLSMinVersion("1.0"); err == nil {
		t.Fatal("TLS 1.0 should be rejected")
	}
}

func TestBuildTLSConfigRequiresBothFiles(t *testing.T) {
	if config, reloader, err := buildTLSConfig("", "", "1.2"); err != nil || config != nil || reloader != nil {
		t.Fatalf("no TLS = %v, %v, %v; want nil, nil, nil", config, reloader, err)
	}

	if _, _, err := buildTLSConfig("cert.pem", "", "1.2"); err == nil {
		t.Fatal("a certificate without a key should be rejected")
	}
	if _, _, err := buildTLSConfig("", "key.pem", "1.2"); err == nil {
		t.Fatal("a key without a certificate should be rejected")
	}
}

func TestBuildTLSConfig(t *testing.T) {
	directory := t.TempDir()
	certPath, keyPath := writeSelfSignedCertificate(t, directory, "server")

	config, reloader, err := buildTLSConfig(certPath, keyPath, "1.3")
	if err != nil {
		t.Fatalf("buildTLSConfig: %v", err)
	}
	if config == nil || reloader == nil {
		t.Fatal("buildTLSConfig returned nil config or reloader")
	}
	if config.MinVersion != tls.VersionTLS13 {
		t.Fatalf("MinVersion = %d, want %d", config.MinVersion, tls.VersionTLS13)
	}
	if config.GetCertificate == nil {
		t.Fatal("GetCertificate is nil")
	}

	if _, _, err := buildTLSConfig(certPath, keyPath, "1.0"); err == nil {
		t.Fatal("an unsupported minimum version should be rejected")
	}
}

func TestCertReloaderPicksUpRotatedCertificate(t *testing.T) {
	directory := t.TempDir()
	certPath, keyPath := writeSelfSignedCertificate(t, directory, "first")

	reloader, err := newCertReloader(certPath, keyPath)
	if err != nil {
		t.Fatalf("newCertReloader: %v", err)
	}

	first, err := reloader.GetCertificate(nil)
	if err != nil {
		t.Fatalf("GetCertificate: %v", err)
	}
	firstBytes := append([]byte(nil), first.Certificate[0]...)

	replacementCert, replacementKey := writeSelfSignedCertificate(t, directory, "second")

	if err := os.WriteFile(certPath, mustReadFile(t, replacementCert), 0o600); err != nil {
		t.Fatalf("replace certificate: %v", err)
	}
	if err := os.WriteFile(keyPath, mustReadFile(t, replacementKey), 0o600); err != nil {
		t.Fatalf("replace key: %v", err)
	}

	// Ensure the modification times differ even on coarse filesystems.
	future := time.Now().Add(2 * time.Second)
	if err := os.Chtimes(certPath, future, future); err != nil {
		t.Fatalf("chtimes certificate: %v", err)
	}
	if err := os.Chtimes(keyPath, future, future); err != nil {
		t.Fatalf("chtimes key: %v", err)
	}

	second, err := reloader.GetCertificate(nil)
	if err != nil {
		t.Fatalf("GetCertificate after rotation: %v", err)
	}
	if string(firstBytes) == string(second.Certificate[0]) {
		t.Fatal("reloader kept the old certificate after rotation")
	}
}

func TestHTTPSHandshakeWorks(t *testing.T) {
	directory := t.TempDir()
	certPath, keyPath := writeSelfSignedCertificate(t, directory, "server")

	config, _, err := buildTLSConfig(certPath, keyPath, "1.2")
	if err != nil {
		t.Fatalf("buildTLSConfig: %v", err)
	}

	server := httptest.NewUnstartedServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		_, _ = w.Write([]byte("secure"))
	}))
	server.TLS = config
	server.StartTLS()
	defer server.Close()

	client := &http.Client{
		Transport: &http.Transport{
			TLSClientConfig: &tls.Config{InsecureSkipVerify: true},
		},
	}

	response, err := client.Get(server.URL)
	if err != nil {
		t.Fatalf("GET: %v", err)
	}
	defer response.Body.Close()

	if response.StatusCode != http.StatusOK {
		t.Fatalf("status = %d, want %d", response.StatusCode, http.StatusOK)
	}
	if response.TLS == nil {
		t.Fatal("response did not use TLS")
	}
}

func TestParsePublicURL(t *testing.T) {
	if parsed, err := parsePublicURL("  "); err != nil || parsed != nil {
		t.Fatalf("empty public URL = %v, %v; want nil, nil", parsed, err)
	}

	parsed, err := parsePublicURL("https://sponsor.example.com")
	if err != nil {
		t.Fatalf("parsePublicURL: %v", err)
	}
	if parsed.Host != "sponsor.example.com" {
		t.Fatalf("host = %q", parsed.Host)
	}

	for _, invalid := range []string{
		"http://sponsor.example.com",
		"https://",
		"https://sponsor.example.com/?token=x",
		"https://sponsor.example.com/#fragment",
	} {
		if _, err := parsePublicURL(invalid); err == nil {
			t.Fatalf("parsePublicURL(%q) should fail", invalid)
		}
	}
}

func TestRedirectHandler(t *testing.T) {
	publicURL, err := parsePublicURL("https://sponsor.example.com")
	if err != nil {
		t.Fatalf("parsePublicURL: %v", err)
	}

	handler := redirectHandler(publicURL)
	request := httptest.NewRequest(http.MethodGet, "http://evil.example/playerextras.php?x=1", nil)
	recorder := httptest.NewRecorder()
	handler.ServeHTTP(recorder, request)

	if recorder.Code != http.StatusPermanentRedirect {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusPermanentRedirect)
	}

	want := "https://sponsor.example.com/playerextras.php?x=1"
	if location := recorder.Header().Get("Location"); location != want {
		t.Fatalf("Location = %q, want %q", location, want)
	}
}

func mustReadFile(t *testing.T, path string) []byte {
	t.Helper()

	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatalf("read %s: %v", path, err)
	}

	return data
}
