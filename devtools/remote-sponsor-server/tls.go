package main

import (
	"crypto/tls"
	"errors"
	"fmt"
	"log"
	"net/http"
	"net/url"
	"os"
	"strings"
	"sync"
	"time"
)

func parseTLSMinVersion(value string) (uint16, error) {
	switch strings.ToLower(strings.TrimSpace(value)) {
	case "", "1.2", "tls1.2", "tls12":
		return tls.VersionTLS12, nil
	case "1.3", "tls1.3", "tls13":
		return tls.VersionTLS13, nil
	default:
		return 0, fmt.Errorf("unsupported TLS minimum version %q (use 1.2 or 1.3)", value)
	}
}

func buildTLSConfig(certPath, keyPath, minVersion string) (*tls.Config, *certReloader, error) {
	if certPath == "" && keyPath == "" {
		return nil, nil, nil
	}
	if certPath == "" || keyPath == "" {
		return nil, nil, errors.New("both -tls-cert and -tls-key must be set")
	}

	version, err := parseTLSMinVersion(minVersion)
	if err != nil {
		return nil, nil, err
	}

	reloader, err := newCertReloader(certPath, keyPath)
	if err != nil {
		return nil, nil, err
	}

	config := &tls.Config{
		MinVersion:     version,
		GetCertificate: reloader.GetCertificate,
	}

	return config, reloader, nil
}

// certReloader serves the certificate currently on disk. The certificate is
// re-read when either file's size or modification time changes, allowing
// rotation without restarting the process.
type certReloader struct {
	certPath string
	keyPath  string

	mu           sync.Mutex
	cert         *tls.Certificate
	certModTime  time.Time
	keyModTime   time.Time
	certSize     int64
	keySize      int64
	lastErrorMsg string
}

func newCertReloader(certPath, keyPath string) (*certReloader, error) {
	reloader := &certReloader{certPath: certPath, keyPath: keyPath}

	if err := reloader.reloadLocked(true); err != nil {
		return nil, err
	}

	return reloader, nil
}

func (r *certReloader) reloadLocked(force bool) error {
	certInfo, err := os.Stat(r.certPath)
	if err != nil {
		return err
	}

	keyInfo, err := os.Stat(r.keyPath)
	if err != nil {
		return err
	}

	if !force &&
		certInfo.ModTime().Equal(r.certModTime) && certInfo.Size() == r.certSize &&
		keyInfo.ModTime().Equal(r.keyModTime) && keyInfo.Size() == r.keySize {
		return nil
	}

	certificate, err := tls.LoadX509KeyPair(r.certPath, r.keyPath)
	if err != nil {
		return err
	}

	r.cert = &certificate
	r.certModTime = certInfo.ModTime()
	r.certSize = certInfo.Size()
	r.keyModTime = keyInfo.ModTime()
	r.keySize = keyInfo.Size()

	log.Printf("TLS certificate loaded from %s", r.certPath)
	return nil
}

func (r *certReloader) GetCertificate(*tls.ClientHelloInfo) (*tls.Certificate, error) {
	r.mu.Lock()
	defer r.mu.Unlock()

	if r.cert == nil {
		if err := r.reloadLocked(true); err != nil {
			return nil, err
		}
	}

	if err := r.reloadLocked(false); err != nil {
		message := err.Error()
		if message != r.lastErrorMsg {
			log.Printf("TLS certificate reload failed, keeping the current certificate: %v", err)
			r.lastErrorMsg = message
		}
	} else {
		r.lastErrorMsg = ""
	}

	return r.cert, nil
}

func parsePublicURL(value string) (*url.URL, error) {
	value = strings.TrimSpace(value)
	if value == "" {
		return nil, nil
	}

	parsed, err := url.Parse(value)
	if err != nil {
		return nil, err
	}
	if parsed.Scheme != "https" {
		return nil, errors.New("public URL must use https")
	}
	if parsed.Host == "" {
		return nil, errors.New("public URL must include a host")
	}
	if parsed.User != nil {
		return nil, errors.New("public URL must not contain credentials")
	}
	if parsed.RawQuery != "" || parsed.Fragment != "" {
		return nil, errors.New("public URL must not contain a query or fragment")
	}

	return parsed, nil
}

func redirectHandler(publicURL *url.URL) http.Handler {
	base := strings.TrimRight(publicURL.String(), "/")

	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		http.Redirect(w, r, base+r.URL.RequestURI(), http.StatusPermanentRedirect)
	})
}
