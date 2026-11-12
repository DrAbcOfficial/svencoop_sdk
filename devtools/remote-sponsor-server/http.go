package main

import (
	"crypto/rand"
	"crypto/rc4"
	"crypto/subtle"
	"encoding/hex"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"runtime/debug"
	"strconv"
	"strings"
	"time"
)

// App holds the HTTP configuration.
type App struct {
	store         *Store
	key           string
	timeZone      string
	lookupPath    string
	adminToken    string
	hsts          bool
	accessLog     bool
	maxConcurrent int
}

func (a *App) routes() http.Handler {
	mux := http.NewServeMux()

	mux.HandleFunc("GET "+healthPath, a.handleHealth)

	// Method checks happen inside the authenticated handlers so an
	// unauthenticated request cannot learn which methods an admin route accepts.
	mux.HandleFunc(adminPlayersPath, a.requireAdmin(a.handlePlayers))
	mux.HandleFunc(adminPlayersPath+"/{authid}", a.requireAdmin(a.handlePlayer))
	mux.HandleFunc(adminReloadPath, a.requireAdmin(a.handleReload))
	mux.HandleFunc("/admin", a.requireAdmin(a.handleAdminNotFound))
	mux.HandleFunc("/admin/", a.requireAdmin(a.handleAdminNotFound))

	// The lookup endpoint intentionally stays public, matching the original
	// game client protocol.
	mux.HandleFunc(a.lookupPath, a.handleLookup)
	if a.lookupPath != "/" {
		mux.HandleFunc("/", a.handleLookup)
	}

	var handler http.Handler = mux

	if a.maxConcurrent > 0 {
		handler = withConcurrencyLimit(a.maxConcurrent, handler)
	}
	if a.accessLog {
		handler = withAccessLog(handler)
	}
	handler = a.withSecurityHeaders(handler)

	return withRecovery(handler)
}

func (a *App) withSecurityHeaders(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("X-Content-Type-Options", "nosniff")
		w.Header().Set("X-Frame-Options", "DENY")
		w.Header().Set("Referrer-Policy", "no-referrer")

		if r.URL.Path == "/admin" || strings.HasPrefix(r.URL.Path, "/admin/") {
			w.Header().Set("Cache-Control", "no-store")
		}
		if a.hsts {
			w.Header().Set("Strict-Transport-Security", hstsHeaderValue)
		}

		next.ServeHTTP(w, r)
	})
}

func withAccessLog(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		requestID := newRequestID()
		w.Header().Set("X-Request-ID", requestID)

		start := time.Now()
		recorder := &statusRecorder{ResponseWriter: w}

		next.ServeHTTP(recorder, r)

		status := recorder.status
		if status == 0 {
			status = http.StatusOK
		}

		log.Printf("http request_id=%s method=%s path=%q status=%d bytes=%d duration=%s remote=%s",
			requestID, r.Method, r.URL.Path, status, recorder.bytes,
			time.Since(start).Round(time.Microsecond), remoteHost(r.RemoteAddr))
	})
}

func withRecovery(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		defer func() {
			if value := recover(); value != nil {
				log.Printf("panic serving %s %s: %v", r.Method, r.URL.Path, value)
				log.Printf("%s", debug.Stack())
				http.Error(w, "internal server error", http.StatusInternalServerError)
			}
		}()

		next.ServeHTTP(w, r)
	})
}

func withConcurrencyLimit(limit int, next http.Handler) http.Handler {
	semaphore := make(chan struct{}, limit)

	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		select {
		case semaphore <- struct{}{}:
			defer func() { <-semaphore }()
			next.ServeHTTP(w, r)
		default:
			w.Header().Set("Retry-After", "1")
			http.Error(w, "server busy", http.StatusServiceUnavailable)
		}
	})
}

type statusRecorder struct {
	http.ResponseWriter
	status int
	bytes  int
}

func (r *statusRecorder) WriteHeader(status int) {
	if r.status != 0 {
		return
	}

	r.status = status
	r.ResponseWriter.WriteHeader(status)
}

func (r *statusRecorder) Write(data []byte) (int, error) {
	if r.status == 0 {
		r.WriteHeader(http.StatusOK)
	}

	written, err := r.ResponseWriter.Write(data)
	r.bytes += written

	return written, err
}

func (r *statusRecorder) Unwrap() http.ResponseWriter {
	return r.ResponseWriter
}

func (r *statusRecorder) Flush() {
	if flusher, ok := r.ResponseWriter.(http.Flusher); ok {
		flusher.Flush()
	}
}

func newRequestID() string {
	var value [8]byte

	if _, err := rand.Read(value[:]); err != nil {
		return strconv.FormatInt(time.Now().UnixNano(), 16)
	}

	return hex.EncodeToString(value[:])
}

func remoteHost(address string) string {
	host, _, err := net.SplitHostPort(address)
	if err != nil {
		return address
	}

	return host
}

func (a *App) handleHealth(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	_, _ = fmt.Fprintln(w, "ok")
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
			http.Error(w, "admin API is disabled (set -admin-token or -admin-token-file)", http.StatusForbidden)
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

func (a *App) handlePlayers(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case http.MethodGet:
		a.handleListPlayers(w, r)
	case http.MethodPost:
		a.handleUpsertPlayer(w, r)
	default:
		methodNotAllowed(w, "GET, POST")
	}
}

func (a *App) handlePlayer(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case http.MethodDelete:
		a.handleDeletePlayer(w, r)
	default:
		methodNotAllowed(w, "DELETE")
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

	log.Printf("admin upsert auth_id=%q level=%d remote=%s", player.AuthID, player.Level, remoteHost(r.RemoteAddr))
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

	log.Printf("admin delete auth_id=%q remote=%s", authID, remoteHost(r.RemoteAddr))
	w.WriteHeader(http.StatusNoContent)
}

func (a *App) handleReload(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		methodNotAllowed(w, "POST")
		return
	}

	if err := a.store.Reload(); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	count := a.store.Count()
	log.Printf("admin reload count=%d remote=%s", count, remoteHost(r.RemoteAddr))
	writeJSON(w, http.StatusOK, map[string]any{"count": count})
}

func (a *App) handleAdminNotFound(w http.ResponseWriter, r *http.Request) {
	http.NotFound(w, r)
}

func methodNotAllowed(w http.ResponseWriter, allow string) {
	w.Header().Set("Allow", allow)
	http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
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
