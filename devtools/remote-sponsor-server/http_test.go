package main

import (
	"bytes"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"strconv"
	"strings"
	"sync"
	"testing"
	"time"
)

func lookupRequestForTest(t *testing.T, ip string, timestamp int64, authID string) *http.Request {
	t.Helper()

	plain := []byte("ipaddr=" + ip + "\ntimestamp=" + strconv.FormatInt(timestamp, 10) + "\nauthid=" + authID + "\n")
	body := encryptForTest(t, defaultKey, plain)

	return httptest.NewRequest(http.MethodPut, defaultLookupPath, bytes.NewReader(body))
}

func TestLookupKnownPlayer(t *testing.T) {
	app := newTestApp(t)
	recorder := httptest.NewRecorder()

	app.routes().ServeHTTP(recorder, lookupRequestForTest(t, "127.0.0.1", 1234567890, "STEAM_0:0:100000001"))

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d (%s)", recorder.Code, http.StatusOK, recorder.Body.String())
	}

	response := string(decryptForTest(t, defaultKey, recorder.Body.Bytes()))
	want := "STEAM_0:0:100000001|1234567890|127.0.0.1|UTC|\"Example Donor\"|\"Example donor\"|1"
	if response != want {
		t.Fatalf("response = %q, want %q", response, want)
	}

	if recorder.Header().Get("Cache-Control") != "no-store" {
		t.Fatalf("Cache-Control = %q, want no-store", recorder.Header().Get("Cache-Control"))
	}
}

func TestLookupUnknownPlayer(t *testing.T) {
	app := newTestApp(t)
	recorder := httptest.NewRecorder()

	app.routes().ServeHTTP(recorder, lookupRequestForTest(t, "10.0.0.1", 42, "STEAM_0:0:999999999"))

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d (%s)", recorder.Code, http.StatusOK, recorder.Body.String())
	}

	response := string(decryptForTest(t, defaultKey, recorder.Body.Bytes()))
	want := "STEAM_0:0:999999999|42|10.0.0.1|UTC|\"Player\"|\"-\"|0"
	if response != want {
		t.Fatalf("response = %q, want %q", response, want)
	}
}

func TestLookupIsPublic(t *testing.T) {
	app := newTestApp(t)
	app.adminToken = ""

	request := lookupRequestForTest(t, "127.0.0.1", 1, "STEAM_0:0:100000001")
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusOK {
		t.Fatalf("public lookup status = %d, want %d", recorder.Code, http.StatusOK)
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

func TestLookupRejectsOversizedBody(t *testing.T) {
	app := newTestApp(t)

	request := httptest.NewRequest(http.MethodPut, defaultLookupPath, bytes.NewReader(make([]byte, maxBodySize+1)))
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusRequestEntityTooLarge {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusRequestEntityTooLarge)
	}
}

func TestLookupRejectsWrongMethod(t *testing.T) {
	app := newTestApp(t)

	request := httptest.NewRequest(http.MethodGet, defaultLookupPath, nil)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusMethodNotAllowed {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusMethodNotAllowed)
	}
}

func TestAdminRoutesRequireAuthentication(t *testing.T) {
	app := newTestApp(t)
	router := app.routes()

	requests := []struct {
		method string
		path   string
		body   string
	}{
		{http.MethodGet, adminPlayersPath, ""},
		{http.MethodPost, adminPlayersPath, "{}"},
		{http.MethodDelete, adminPlayersPath + "/STEAM_0:0:100000001", ""},
		{http.MethodPost, adminReloadPath, ""},
		{http.MethodGet, "/admin", ""},
		{http.MethodGet, "/admin/unknown", ""},
	}

	for _, test := range requests {
		request := httptest.NewRequest(test.method, test.path, strings.NewReader(test.body))
		recorder := httptest.NewRecorder()
		router.ServeHTTP(recorder, request)

		if recorder.Code != http.StatusUnauthorized {
			t.Fatalf("%s %s status = %d, want %d", test.method, test.path, recorder.Code, http.StatusUnauthorized)
		}
		if recorder.Header().Get("WWW-Authenticate") == "" {
			t.Fatalf("%s %s did not send WWW-Authenticate", test.method, test.path)
		}
	}
}

func TestAdminRejectsWrongToken(t *testing.T) {
	app := newTestApp(t)
	request := httptest.NewRequest(http.MethodGet, adminPlayersPath, nil)
	request.Header.Set("Authorization", "Bearer wrong-token")
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusUnauthorized {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusUnauthorized)
	}
}

func TestAdminDisabledReturnsForbidden(t *testing.T) {
	app := newTestApp(t)
	app.adminToken = ""

	request := httptest.NewRequest(http.MethodGet, adminPlayersPath, nil)
	request.Header.Set("Authorization", "Bearer "+testAdminToken)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusForbidden {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusForbidden)
	}
}

func TestAdminList(t *testing.T) {
	app := newTestApp(t)
	request := httptest.NewRequest(http.MethodGet, adminPlayersPath, nil)
	request.Header.Set("Authorization", "Bearer "+testAdminToken)
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
	if recorder.Header().Get("Cache-Control") != "no-store" {
		t.Fatalf("admin Cache-Control = %q, want no-store", recorder.Header().Get("Cache-Control"))
	}
}

func TestAdminUpsertAndDelete(t *testing.T) {
	app := newTestApp(t)
	router := app.routes()

	upsertBody := "{\"auth_id\":\"STEAM_0:0:100000002\",\"name\":\"New Player\",\"description\":\"Added by admin\",\"level\":2}"
	upsertRequest := httptest.NewRequest(http.MethodPost, adminPlayersPath, strings.NewReader(upsertBody))
	upsertRequest.Header.Set("Authorization", "Bearer "+testAdminToken)
	upsertRecorder := httptest.NewRecorder()
	router.ServeHTTP(upsertRecorder, upsertRequest)

	if upsertRecorder.Code != http.StatusCreated {
		t.Fatalf("upsert status = %d, want %d (%s)", upsertRecorder.Code, http.StatusCreated, upsertRecorder.Body.String())
	}

	if player, found := app.store.Lookup("STEAM_0:0:100000002"); !found || player.Level != 2 {
		t.Fatalf("upserted player = %+v, found = %t", player, found)
	}

	deleteRequest := httptest.NewRequest(http.MethodDelete, adminPlayersPath+"/STEAM_0:0:100000002", nil)
	deleteRequest.Header.Set("X-Admin-Token", testAdminToken)
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

func TestAdminUpdateExistingPlayerReturnsOK(t *testing.T) {
	app := newTestApp(t)
	body := "{\"auth_id\":\"STEAM_0:0:100000001\",\"name\":\"Renamed\",\"description\":\"Updated\",\"level\":4}"
	request := httptest.NewRequest(http.MethodPost, adminPlayersPath, strings.NewReader(body))
	request.Header.Set("Authorization", "Bearer "+testAdminToken)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d (%s)", recorder.Code, http.StatusOK, recorder.Body.String())
	}

	player, found := app.store.Lookup("STEAM_0:0:100000001")
	if !found || player.Name != "Renamed" || player.Level != 4 {
		t.Fatalf("player = %+v, found = %t", player, found)
	}
}

func TestAdminDeleteMissingPlayer(t *testing.T) {
	app := newTestApp(t)
	request := httptest.NewRequest(http.MethodDelete, adminPlayersPath+"/STEAM_0:0:404", nil)
	request.Header.Set("Authorization", "Bearer "+testAdminToken)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusNotFound {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusNotFound)
	}
}

func TestAdminReload(t *testing.T) {
	app := newTestApp(t)

	if _, _, err := app.store.Upsert(Player{AuthID: "STEAM_0:0:100000009", Name: "Temporary", Description: "temporary", Level: 0}); err != nil {
		t.Fatalf("Upsert: %v", err)
	}

	request := httptest.NewRequest(http.MethodPost, adminReloadPath, nil)
	request.Header.Set("Authorization", "Bearer "+testAdminToken)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d (%s)", recorder.Code, http.StatusOK, recorder.Body.String())
	}

	// The store rewrote the CSV on upsert, so reload keeps all three rows.
	if app.store.Count() != 3 {
		t.Fatalf("count after reload = %d, want 3", app.store.Count())
	}
}

func TestAdminMethodNotAllowed(t *testing.T) {
	app := newTestApp(t)
	router := app.routes()

	requests := []struct {
		method string
		path   string
		allow  string
	}{
		{http.MethodPut, adminPlayersPath, "GET, POST"},
		{http.MethodGet, adminPlayersPath + "/STEAM_0:0:100000001", "DELETE"},
		{http.MethodGet, adminReloadPath, "POST"},
	}

	for _, test := range requests {
		request := httptest.NewRequest(test.method, test.path, nil)
		request.Header.Set("Authorization", "Bearer "+testAdminToken)
		recorder := httptest.NewRecorder()
		router.ServeHTTP(recorder, request)

		if recorder.Code != http.StatusMethodNotAllowed {
			t.Fatalf("%s %s status = %d, want %d", test.method, test.path, recorder.Code, http.StatusMethodNotAllowed)
		}
		if recorder.Header().Get("Allow") != test.allow {
			t.Fatalf("%s %s Allow = %q, want %q", test.method, test.path, recorder.Header().Get("Allow"), test.allow)
		}
	}
}

func TestAdminUnknownPathRequiresAuthAndReturnsNotFound(t *testing.T) {
	app := newTestApp(t)
	request := httptest.NewRequest(http.MethodGet, "/admin/unknown", nil)
	request.Header.Set("Authorization", "Bearer "+testAdminToken)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusNotFound {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusNotFound)
	}
}

func TestHealthIsPublic(t *testing.T) {
	app := newTestApp(t)
	app.adminToken = ""

	request := httptest.NewRequest(http.MethodGet, healthPath, nil)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	if recorder.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusOK)
	}
	if strings.TrimSpace(recorder.Body.String()) != "ok" {
		t.Fatalf("body = %q, want ok", recorder.Body.String())
	}
}

func TestSecurityHeaders(t *testing.T) {
	app := newTestApp(t)
	app.hsts = true

	request := httptest.NewRequest(http.MethodGet, healthPath, nil)
	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, request)

	headers := map[string]string{
		"X-Content-Type-Options":    "nosniff",
		"X-Frame-Options":           "DENY",
		"Referrer-Policy":           "no-referrer",
		"Strict-Transport-Security": hstsHeaderValue,
	}

	for name, want := range headers {
		if got := recorder.Header().Get(name); got != want {
			t.Fatalf("%s = %q, want %q", name, got, want)
		}
	}

	app.hsts = false
	recorder = httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, httptest.NewRequest(http.MethodGet, healthPath, nil))

	if got := recorder.Header().Get("Strict-Transport-Security"); got != "" {
		t.Fatalf("HSTS without TLS = %q, want empty", got)
	}
}

func TestAccessLogAddsRequestID(t *testing.T) {
	app := newTestApp(t)
	app.accessLog = true

	recorder := httptest.NewRecorder()
	app.routes().ServeHTTP(recorder, httptest.NewRequest(http.MethodGet, healthPath, nil))

	if recorder.Header().Get("X-Request-ID") == "" {
		t.Fatal("X-Request-ID is missing")
	}
}

func TestConcurrencyLimitRejectsExtraRequests(t *testing.T) {
	entered := make(chan struct{})
	release := make(chan struct{})

	handler := withConcurrencyLimit(1, http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		close(entered)
		<-release
		w.WriteHeader(http.StatusOK)
	}))

	server := httptest.NewServer(handler)
	defer server.Close()

	var wg sync.WaitGroup
	wg.Add(1)

	go func() {
		defer wg.Done()
		response, err := server.Client().Get(server.URL)
		if err == nil {
			response.Body.Close()
		}
	}()

	select {
	case <-entered:
	case <-time.After(5 * time.Second):
		t.Fatal("first request did not reach the handler")
	}

	response, err := server.Client().Get(server.URL)
	if err != nil {
		t.Fatalf("second request: %v", err)
	}
	response.Body.Close()

	if response.StatusCode != http.StatusServiceUnavailable {
		t.Fatalf("second request status = %d, want %d", response.StatusCode, http.StatusServiceUnavailable)
	}
	if response.Header.Get("Retry-After") == "" {
		t.Fatal("Retry-After is missing on a throttled response")
	}

	close(release)
	wg.Wait()
}

func TestRecoveryReturnsInternalError(t *testing.T) {
	handler := withRecovery(http.HandlerFunc(func(http.ResponseWriter, *http.Request) {
		panic("boom")
	}))

	recorder := httptest.NewRecorder()
	handler.ServeHTTP(recorder, httptest.NewRequest(http.MethodGet, "/", nil))

	if recorder.Code != http.StatusInternalServerError {
		t.Fatalf("status = %d, want %d", recorder.Code, http.StatusInternalServerError)
	}
}

func TestStatusRecorderDefaultsToOK(t *testing.T) {
	recorder := &statusRecorder{ResponseWriter: httptest.NewRecorder()}

	if _, err := recorder.Write([]byte("data")); err != nil {
		t.Fatalf("write: %v", err)
	}

	if recorder.status != http.StatusOK || recorder.bytes != 4 {
		t.Fatalf("status = %d, bytes = %d; want 200 and 4", recorder.status, recorder.bytes)
	}
}
