#!/bin/zsh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$SCRIPT_DIR"

echo "=== Limpiando build anterior ==="
rm -rf build

echo "=== Configurando CMake (arm64) ==="
arch -arm64 cmake -S . -B build -DCMAKE_OSX_ARCHITECTURES=arm64

echo "=== Compilando ==="
arch -arm64 cmake --build build -j 4

echo "=== Compilación exitosa ==="
echo ""
echo "Ejecutar versión gráfica:   $SCRIPT_DIR/build/game_v1_sfml"
echo "Ejecutar versión CLI:       $SCRIPT_DIR/build/game_v1_cli"
