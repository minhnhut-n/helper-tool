#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BINARY="$SCRIPT_DIR/logging-daemon"

if [ ! -x "$BINARY" ]; then
    echo "logging daemon binary not found or not executable: $BINARY" >&2
    exit 1
fi

exec "$BINARY"
