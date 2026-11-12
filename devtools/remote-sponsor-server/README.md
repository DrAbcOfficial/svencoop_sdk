# Remote sponsor / ban server (Go)

This directory contains a self-hosted implementation of the Sven Co-op player
extras service used by `CPlayerDatabase`. It speaks the same RC4-protected
wire format as the game DLL, stores its list in a JSON file, serves HTTP or
HTTPS, and exposes an authenticated admin API.

The old built-in sponsor/ban list was removed from the game DLL. Servers now
get that data only from a remote endpoint (or not at all).

## Auth model

| Endpoint | Auth |
| --- | --- |
| `-path` (`/playerextras.php` by default) | Public, no authentication. This matches the original game client. |
| `/healthz` | Public. |
| `/admin` and everything under `/admin/` | Requires a bearer token. Without a configured token the whole admin API is disabled and returns `403`. |

The admin token is read from `-admin-token` or `-admin-token-file` (never
both). Tokens must be at least 16 characters. Prefer the file so the secret
does not appear in the process list. Admin requests may send either
`Authorization: Bearer <token>` or `X-Admin-Token: <token>`.

## Build and run

```sh
cd devtools/remote-sponsor-server
/usr/local/go/bin/go test ./...
/usr/local/go/bin/go build -o remote-sponsor-server .
```

Plain HTTP for local testing or when a reverse proxy terminates TLS:

```sh
echo 'change-me-0123456789' > admin-token
chmod 600 admin-token
./remote-sponsor-server \
  -listen 127.0.0.1:8080 \
  -list players.json \
  -admin-token-file admin-token
```

HTTPS with automatic certificate reload, plus an HTTP listener that redirects
to the canonical HTTPS URL:

```sh
./remote-sponsor-server \
  -listen :443 \
  -tls-cert /etc/remote-sponsor/fullchain.pem \
  -tls-key /etc/remote-sponsor/privkey.pem \
  -tls-min-version 1.2 \
  -hsts \
  -http-listen :80 \
  -public-url https://sponsor.example.com \
  -list players.json \
  -admin-token-file /etc/remote-sponsor/admin-token
```

The certificate and key are re-read automatically when their size or
modification time changes, so renewal does not require a restart.

The server starts with an empty list if `players.json` does not exist yet.
Copy `players.example.json` to `players.json` for a quick smoke test:

```sh
cp players.example.json players.json
```

## Flags

| Flag | Default | Meaning |
| --- | --- | --- |
| `-listen` | `:8080` | Main listen address. Serves HTTPS when both TLS flags are set. |
| `-http-listen` | empty | Optional plain HTTP address that permanently redirects to `-public-url`. Requires TLS and `-public-url`. |
| `-public-url` | empty | Absolute `https://` URL used by `-http-listen` for redirects. |
| `-list` | `players.json` | JSON file containing a `version` and a `players` array. |
| `-key` | built-in 64-byte key | RC4 key. Must match the game DLL. |
| `-timezone` | `UTC` | Timezone field returned to the game (max 7 bytes). |
| `-path` | `/playerextras.php` | Public lookup endpoint path. It cannot be under `/admin`. |
| `-admin-token` | empty | Bearer token for the admin API. |
| `-admin-token-file` | empty | File containing the admin bearer token. |
| `-access-log` | `true` | Log one line per HTTP request (method, path, status, duration, remote address, request ID). |
| `-max-concurrent` | `0` | Maximum concurrent HTTP requests; `0` means unlimited. Excess requests get `503` and `Retry-After`. |
| `-tls-cert` | empty | TLS certificate file. Enables HTTPS together with `-tls-key`. |
| `-tls-key` | empty | TLS private key file. Enables HTTPS together with `-tls-cert`. |
| `-tls-min-version` | `1.2` | Minimum TLS version: `1.2` or `1.3`. |
| `-hsts` | `true` | Send `Strict-Transport-Security` when HTTPS is enabled. |
| `-shutdown-timeout` | `15s` | Graceful shutdown timeout. |
| `-version` | `false` | Print the server version and exit. |

The built-in key is the one hard-coded in `dlls/server/donid.cpp`. If you
change it with `-key`, change the DLL too and rebuild.

## JSON format

The file is a versioned object with a `players` array:

```json
{
  "version": 1,
  "players": [
    {
      "auth_id": "STEAM_0:0:100000001",
      "name": "Example Donor",
      "description": "Example donor (electric crowbar)",
      "level": 1
    },
    {
      "auth_id": "STEAM_0:0:100000003",
      "name": "Example Banned",
      "description": "Example ban",
      "level": -1
    }
  ]
}
```

`version` must be `1`. Unknown fields, duplicate `auth_id` values and
unsupported versions are rejected at startup. The file is rewritten atomically
on every admin mutation. The old CSV format is no longer read; convert an
existing list to this JSON document manually.

| Level | Meaning |
| --- | --- |
| -1 | Banned |
| 0 | No extras |
| 1 | Donor (electric crowbar and Chumtoad) |
| 2 | Donor (gold Uzi) |
| 3 | Donor (increased Chumtoad health) |
| 4 | Staff (contributor/tester) |
| 5 | Staff (artist) |
| 6 | Staff (leader/developer) |

Names and descriptions are sanitized for the pipe/quoted response format and
truncated to the DLL's field sizes.

## Game DLL configuration

The relevant server cvars are:

| Cvar | Default | Meaning |
| --- | --- | --- |
| `sv_disable_online_api` | `0` | Existing master switch. Non-zero disables all online lookups. |
| `sv_online_api_url` | official URL | Lookup endpoint. Set to an empty string to disable online lookups. |
| `sv_online_api_bans` | `0` | Set to `1` to let a remote level `-1` kick/ban the player. |

Example `server.cfg`:

```cfg
sv_online_api_url "https://sponsor.example.com/playerextras.php"
sv_online_api_bans "0"
```

With `sv_online_api_bans` at `0` the remote level `-1` is still stored and
shown as "Barred", but the DLL does not kick or ban the player. Set it to `1`
only when you trust the remote list.

## Protocol

Lookup requests are HTTP `PUT` (or `POST`) to `-path`. The body is the
request plaintext encrypted with RC4 using `-key`:

```text
ipaddr=<player ip>
timestamp=<unix timestamp>
authid=<steam id>
```

The response is RC4-encrypted with the same key and has this plaintext:

```text
<authid>|<timestamp>|<ip>|<timezone>|"<name>"|"<description>"|<level>
```

The DLL rejects a response whose `authid`, `timestamp` or `ip` does not
match the request. Unknown players get level `0`, name `Player`, and
description `-`.

RC4 is only an obfuscation layer, not authentication. Anyone with the key can
query the endpoint, and the protocol has no replay protection on the server.
Use HTTPS if the endpoint is exposed to the internet, and keep the admin API on
a trusted network.

## Admin API

All admin endpoints require a bearer token. They are disabled when no token is
configured. Every admin mutation rewrites `-list` atomically (write to a
temporary file, then rename), and mutations plus reloads are serialized so a
concurrent reload cannot clobber a newer edit.

```sh
# List players
curl -H "Authorization: Bearer change-me-0123456789" \
  https://sponsor.example.com/admin/players

# Add or update a player
curl -X POST https://sponsor.example.com/admin/players \
  -H "Authorization: Bearer change-me-0123456789" \
  -H "Content-Type: application/json" \
  -d '{"auth_id":"STEAM_0:0:100000002","name":"New Player","description":"Added by admin","level":2}'

# Delete a player
curl -X DELETE https://sponsor.example.com/admin/players/STEAM_0:0:100000002 \
  -H "Authorization: Bearer change-me-0123456789"

# Reload the JSON file from disk
curl -X POST https://sponsor.example.com/admin/reload \
  -H "Authorization: Bearer change-me-0123456789"
```

`/healthz` returns `ok` and needs no authentication.

## Production notes

- Run behind a firewall or reverse proxy; only the lookup path and
  `/healthz` need to be public.
- Use `-admin-token-file` with mode `0600` rather than `-admin-token`.
- The JSON file and its directory must be writable by the server process for
  admin mutations. In Docker, mount a writable volume at `/data`.
- Use `-max-concurrent` to bound resource usage under load. The store is
  safe for concurrent lookups, edits and reloads.
- The server shuts down gracefully on `SIGINT`/`SIGTERM` and stops accepting
  new requests while finishing in-flight ones.
- Each response carries `X-Request-ID` and the access log includes it, which
  makes reverse-proxy and server logs easy to correlate.

Example systemd unit:

```ini
[Unit]
Description=Sven Co-op remote sponsor server
After=network-online.target

[Service]
ExecStart=/opt/remote-sponsor/remote-sponsor-server \
  -listen :443 \
  -tls-cert /etc/remote-sponsor/fullchain.pem \
  -tls-key /etc/remote-sponsor/privkey.pem \
  -http-listen :80 \
  -public-url https://sponsor.example.com \
  -list /var/lib/remote-sponsor/players.json \
  -admin-token-file /etc/remote-sponsor/admin-token
Restart=on-failure
User=remote-sponsor
Group=remote-sponsor

[Install]
WantedBy=multi-user.target
```

A multi-stage `Dockerfile` is included for container deployments.
