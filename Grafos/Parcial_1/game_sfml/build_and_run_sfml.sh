#!/bin/zsh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

arch -arm64 cmake -S "$SCRIPT_DIR" -B "$SCRIPT_DIR/build" -DCMAKE_OSX_ARCHITECTURES=arm64
arch -arm64 cmake --build "$SCRIPT_DIR/build" -j 4
exec arch -arm64 "$SCRIPT_DIR/build/game_v1_sfml"
