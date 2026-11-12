# syntax=docker/dockerfile:1

FROM golang:1.22-alpine AS build

WORKDIR /src

COPY go.mod ./
COPY *.go ./

RUN CGO_ENABLED=0 go build -trimpath -ldflags="-s -w" -o /out/remote-sponsor-server . \
    && mkdir -p /out/data \
    && touch /out/data/.keep

FROM gcr.io/distroless/static-debian12:nonroot

COPY --from=build --chown=65532:65532 /out/remote-sponsor-server /usr/local/bin/remote-sponsor-server
COPY --from=build --chown=65532:65532 /out/data /data

WORKDIR /data

EXPOSE 8080 8443

ENTRYPOINT ["/usr/local/bin/remote-sponsor-server"]
