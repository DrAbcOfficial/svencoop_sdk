package main

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/rc4"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/pem"
	"math/big"
	"net"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"
)

const testAdminToken = "0123456789abcdef"

var testNewline = string([]byte{0x0A})

func csvLine(fields ...string) string {
	quote := string([]byte{0x22})

	for i := range fields {
		fields[i] = quote + fields[i] + quote
	}

	return strings.Join(fields, ",") + testNewline
}

func newTestStore(t *testing.T) *Store {
	t.Helper()

	return &Store{
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
}

func newTestApp(t *testing.T) *App {
	t.Helper()

	return &App{
		store:      newTestStore(t),
		key:        defaultKey,
		timeZone:   defaultTimeZone,
		lookupPath: defaultLookupPath,
		adminToken: testAdminToken,
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

// writeSelfSignedCertificate writes a certificate and key for localhost into
// a temporary directory and returns both paths.
func writeSelfSignedCertificate(t *testing.T, directory, name string) (string, string) {
	t.Helper()

	privateKey, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		t.Fatalf("generate key: %v", err)
	}

	serial, err := rand.Int(rand.Reader, new(big.Int).Lsh(big.NewInt(1), 120))
	if err != nil {
		t.Fatalf("serial: %v", err)
	}

	template := x509.Certificate{
		SerialNumber:          serial,
		Subject:               pkix.Name{CommonName: name},
		NotBefore:             time.Now().Add(-time.Hour),
		NotAfter:              time.Now().Add(24 * time.Hour),
		KeyUsage:              x509.KeyUsageDigitalSignature | x509.KeyUsageCertSign,
		ExtKeyUsage:           []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth},
		BasicConstraintsValid: true,
		IsCA:                  true,
		DNSNames:              []string{"localhost"},
		IPAddresses:           []net.IP{net.ParseIP("127.0.0.1"), net.ParseIP("::1")},
	}

	der, err := x509.CreateCertificate(rand.Reader, &template, &template, &privateKey.PublicKey, privateKey)
	if err != nil {
		t.Fatalf("create certificate: %v", err)
	}

	keyDER, err := x509.MarshalECPrivateKey(privateKey)
	if err != nil {
		t.Fatalf("marshal key: %v", err)
	}

	certPath := filepath.Join(directory, name+".crt")
	keyPath := filepath.Join(directory, name+".key")

	if err := os.WriteFile(certPath, pem.EncodeToMemory(&pem.Block{Type: "CERTIFICATE", Bytes: der}), 0o600); err != nil {
		t.Fatalf("write certificate: %v", err)
	}
	if err := os.WriteFile(keyPath, pem.EncodeToMemory(&pem.Block{Type: "EC PRIVATE KEY", Bytes: keyDER}), 0o600); err != nil {
		t.Fatalf("write key: %v", err)
	}

	return certPath, keyPath
}
