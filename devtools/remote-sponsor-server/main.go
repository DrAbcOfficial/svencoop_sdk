// Command remote-sponsor-server is a Sven Co-op player extras
// (sponsor/ban list) server. It speaks the same RC4-protected protocol as the
// server DLL, stores its list in a JSON file, serves HTTPS, and exposes an
// authenticated admin API.
package main

import (
	"context"
	"errors"
	"flag"
	"fmt"
	"log"
	"net"
	"net/http"
	"os"
	"os/signal"
	"strings"
	"sync"
	"syscall"
	"time"
)

const (
	serverVersion = "1.2.0"

	defaultKey = "j.6v,A?cPk4)zL[89$~GTBM@fmR-(U{;u&3qY]eh/p}sbX#Z*75Jd=nE%DC_:xiK"

	maxBodySize          = 4096
	maxAuthIDLength      = 44
	maxNameLength        = 32
	maxDescriptionLength = 128
	maxIPLength          = 64
	maxTimeZoneLength    = 7
	maxAdminRequestSize  = 1 << 20
	maxHeaderBytes       = 1 << 16

	defaultLookupPath     = "/playerextras.php"
	defaultTimeZone       = "UTC"
	defaultListenAddress  = ":8080"
	defaultListFileName   = "players.json"
	defaultTLSMinVersion  = "1.2"
	defaultShutdownPeriod = 15 * time.Second

	readHeaderTimeout = 5 * time.Second
	readTimeout       = 10 * time.Second
	writeTimeout      = 10 * time.Second
	idleTimeout       = 60 * time.Second

	adminPlayersPath = "/admin/players"
	adminReloadPath  = "/admin/reload"
	healthPath       = "/healthz"

	minLevel            = -1
	maxLevel            = 6
	defaultPlayerName   = "Player"
	defaultPlayerDesc   = "-"
	minAdminTokenLength = 16

	hstsHeaderValue = "max-age=31536000"
)

func main() {
	listen := flag.String("listen", defaultListenAddress, "main listen address (HTTP, or HTTPS when TLS flags are set)")
	httpListen := flag.String("http-listen", "", "optional plain HTTP listen address that redirects to -public-url; requires TLS")
	publicURLFlag := flag.String("public-url", "", "absolute https URL used by -http-listen for redirects")
	listPath := flag.String("list", defaultListFileName, "JSON file with the sponsor/ban list")
	key := flag.String("key", defaultKey, "RC4 key shared with the game DLL (64 characters by default)")
	timeZone := flag.String("timezone", defaultTimeZone, "timezone field returned to the game (at most 7 bytes)")
	lookupPath := flag.String("path", defaultLookupPath, "HTTP path used for public player lookups")
	adminToken := flag.String("admin-token", "", "bearer token for the admin API; empty disables the admin API")
	adminTokenFile := flag.String("admin-token-file", "", "file containing the admin bearer token; empty disables the admin API")
	accessLog := flag.Bool("access-log", true, "log one line per HTTP request")
	maxConcurrent := flag.Int("max-concurrent", 0, "maximum concurrent HTTP requests; 0 means unlimited")
	tlsCert := flag.String("tls-cert", "", "TLS certificate file; enables HTTPS together with -tls-key")
	tlsKey := flag.String("tls-key", "", "TLS private key file; enables HTTPS together with -tls-cert")
	tlsMinVersion := flag.String("tls-min-version", defaultTLSMinVersion, "minimum TLS version: 1.2 or 1.3")
	hsts := flag.Bool("hsts", true, "send Strict-Transport-Security when HTTPS is enabled")
	shutdownTimeout := flag.Duration("shutdown-timeout", defaultShutdownPeriod, "graceful shutdown timeout")
	showVersion := flag.Bool("version", false, "print version and exit")
	flag.Parse()

	if *showVersion {
		fmt.Println(serverVersion)
		return
	}

	if *key == "" {
		log.Fatal("the RC4 key must not be empty")
	}
	if !strings.HasPrefix(*lookupPath, "/") {
		log.Fatal("the lookup path must start with /")
	}
	if *lookupPath == "/admin" || strings.HasPrefix(*lookupPath, "/admin/") {
		log.Fatal("the lookup path must not be under /admin")
	}
	if *lookupPath == adminPlayersPath || *lookupPath == adminReloadPath || *lookupPath == healthPath {
		log.Fatal("the lookup path conflicts with a built-in endpoint")
	}
	if *maxConcurrent < 0 {
		log.Fatal("-max-concurrent must not be negative")
	}
	if *shutdownTimeout <= 0 {
		log.Fatal("-shutdown-timeout must be positive")
	}

	token, err := resolveAdminToken(*adminToken, *adminTokenFile)
	if err != nil {
		log.Fatalf("admin token: %v", err)
	}

	tlsConfig, _, err := buildTLSConfig(*tlsCert, *tlsKey, *tlsMinVersion)
	if err != nil {
		log.Fatalf("TLS configuration: %v", err)
	}

	if *httpListen != "" {
		if tlsConfig == nil {
			log.Fatal("-http-listen requires TLS (-tls-cert and -tls-key)")
		}
		if *httpListen == *listen {
			log.Fatal("-http-listen must differ from -listen")
		}
	}

	publicURL, err := parsePublicURL(*publicURLFlag)
	if err != nil {
		log.Fatalf("public URL: %v", err)
	}
	if *httpListen != "" && publicURL == nil {
		log.Fatal("-http-listen requires -public-url")
	}
	if publicURL != nil && *httpListen == "" {
		log.Print("warning: -public-url is only used by -http-listen")
	}
	if *hsts && tlsConfig == nil {
		log.Print("warning: -hsts has no effect without TLS")
	}

	store, err := NewStore(*listPath)
	if err != nil {
		log.Fatalf("cannot load %s: %v", *listPath, err)
	}

	app := &App{
		store:         store,
		key:           *key,
		timeZone:      cleanTimeZone(*timeZone),
		lookupPath:    *lookupPath,
		adminToken:    token,
		hsts:          tlsConfig != nil && *hsts,
		accessLog:     *accessLog,
		maxConcurrent: *maxConcurrent,
	}

	server := newHTTPServer(*listen, app.routes())
	server.TLSConfig = tlsConfig

	var redirectServer *http.Server
	if *httpListen != "" {
		redirectServer = newHTTPServer(*httpListen, redirectHandler(publicURL))
	}

	ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer stop()

	var wg sync.WaitGroup
	errCh := make(chan error, 2)

	start := func(srv *http.Server, useTLS bool) {
		defer wg.Done()

		var serveErr error

		if useTLS {
			var listener net.Listener

			listener, serveErr = net.Listen("tcp", srv.Addr)
			if serveErr == nil {
				log.Printf("HTTPS listening on %s", srv.Addr)
				serveErr = srv.ServeTLS(listener, "", "")
			}
		} else {
			log.Printf("HTTP listening on %s", srv.Addr)
			serveErr = srv.ListenAndServe()
		}

		if serveErr != nil && !errors.Is(serveErr, http.ErrServerClosed) {
			errCh <- serveErr
		}
	}

	wg.Add(1)
	go start(server, tlsConfig != nil)

	if redirectServer != nil {
		wg.Add(1)
		go start(redirectServer, false)
	}

	log.Printf("remote sponsor server %s: lookup %s, list %s (%d players), admin API %s, TLS %s",
		serverVersion, *lookupPath, *listPath, store.Count(), enabledText(token != ""), enabledText(tlsConfig != nil))

	var runErr error

	select {
	case <-ctx.Done():
	case runErr = <-errCh:
		log.Printf("server error: %v", runErr)
		stop()
	}

	shutdownContext, cancel := context.WithTimeout(context.Background(), *shutdownTimeout)
	defer cancel()

	if redirectServer != nil {
		shutdownServer(redirectServer, shutdownContext)
	}
	shutdownServer(server, shutdownContext)

	wg.Wait()
	close(errCh)

	for err := range errCh {
		if runErr == nil {
			runErr = err
		}
	}

	if runErr != nil {
		log.Fatalf("exiting after server error: %v", runErr)
	}
}

func newHTTPServer(address string, handler http.Handler) *http.Server {
	return &http.Server{
		Addr:              address,
		Handler:           handler,
		ReadHeaderTimeout: readHeaderTimeout,
		ReadTimeout:       readTimeout,
		WriteTimeout:      writeTimeout,
		IdleTimeout:       idleTimeout,
		MaxHeaderBytes:    maxHeaderBytes,
	}
}

func shutdownServer(server *http.Server, ctx context.Context) {
	if err := server.Shutdown(ctx); err != nil {
		log.Printf("graceful shutdown of %s failed: %v", server.Addr, err)
		_ = server.Close()
	}
}

func resolveAdminToken(flagValue, filePath string) (string, error) {
	if flagValue != "" && filePath != "" {
		return "", errors.New("specify either -admin-token or -admin-token-file, not both")
	}

	token := strings.TrimSpace(flagValue)

	if filePath != "" {
		data, err := os.ReadFile(filePath)
		if err != nil {
			return "", fmt.Errorf("read %s: %w", filePath, err)
		}

		token = strings.TrimSpace(string(data))
		if token == "" {
			return "", fmt.Errorf("%s is empty", filePath)
		}

		if info, err := os.Stat(filePath); err == nil && info.Mode().Perm()&0o077 != 0 {
			log.Printf("warning: admin token file %s is readable by group/others", filePath)
		}
	}

	if token == "" {
		return "", nil
	}
	if len(token) < minAdminTokenLength {
		return "", fmt.Errorf("token must be at least %d characters", minAdminTokenLength)
	}

	return token, nil
}

func enabledText(enabled bool) string {
	if enabled {
		return "enabled"
	}

	return "disabled"
}
