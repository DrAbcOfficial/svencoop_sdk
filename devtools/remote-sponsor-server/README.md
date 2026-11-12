# Remote sponsor / ban server (Go)

This directory contains a minimal, self-hosted implementation of the Sven Co-op
player-extras service used by `CPlayerDatabase`. It serves the same
RC4-protected wire format as the game DLL, keeps the list in a CSV file, and
provides an optional HTTP admin API for hot reloads and edits.

The old built-in sponsor/ban list was removed from the game DLL. Servers now
get that data only from a remote endpoint (or not at all).

## Build and run

```sh
cd devtools/remote-sponsor-server
/usr/local/go/bin/go test ./...
/usr/local/go/bin/go build -o remote-sponsor-server .
./remote-sponsor-server -list players.csv -listen :8080 -admin-token change-me
```

The server starts with an empty list if `players.csv` does not exist yet.
Copy `players.example.csv` to `players.csv` for a quick smoke test:

```sh
cp players.example.csv players.csv
```

## Flags

| Flag | Default | Meaning |
| --- | --- | --- |
| `-listen` | `:8080` | HTTP listen address. |
| `-list` | `players.csv` | CSV file with `auth_id,name,description,level` rows. |
| `-key` | built-in 64-byte key | RC4 key. Must match the game DLL. |
| `-timezone` | `UTC` | Timezone field returned to the game (max 7 bytes). |
| `-path` | `/playerextras.php` | Lookup endpoint path. |
| `-admin-token` | empty | Bearer token for the admin API. Empty disables admin endpoints. |

The built-in key is the one hard-coded in `dlls/server/donid.cpp`. If you
change it with `-key`, change the DLL too and rebuild.

## CSV format

The file has no header and exactly four columns:

```csv
"STEAM_0:0:100000001","Example Donor","Example donor","1"
"STEAM_0:0:100000003","Example Banned","Example ban","-1"
```

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
sv_online_api_url "http://127.0.0.1:8080/playerextras.php"
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
Run this behind TLS (for example nginx/Caddy) if it is exposed to the internet,
and keep the admin API on a trusted network.

## Admin API

All admin endpoints require `Authorization: Bearer <token>` or
`X-Admin-Token: <token>`. They are disabled when `-admin-token` is empty.

```sh
# List players
curl -H "Authorization: Bearer change-me" http://127.0.0.1:8080/admin/players

# Add or update a player
curl -X POST http://127.0.0.1:8080/admin/players \
  -H "Authorization: Bearer change-me" \
  -H "Content-Type: application/json" \
  -d '{"auth_id":"STEAM_0:0:100000002","name":"New Player","description":"Added by admin","level":2}'

# Delete a player
curl -X DELETE http://127.0.0.1:8080/admin/players/STEAM_0:0:100000002 \
  -H "Authorization: Bearer change-me"

# Reload the CSV from disk
curl -X POST http://127.0.0.1:8080/admin/reload \
  -H "Authorization: Bearer change-me"
```

Every admin mutation rewrites `-list` atomically (write to a temporary file,
then rename). `/healthz` returns `ok` and needs no authentication.
