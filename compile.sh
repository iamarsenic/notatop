#!/usr/bin/env bash
set -e

BUILD_DIR="build"
BINARY="nat"

echo "==> Make build directory..."
mkdir -pv "$BUILD_DIR"

echo "==> Configuring cmake..."
cmake -B "$BUILD_DIR" -S . -DCMAKE_BUILD_TYPE=Release

echo "==> Running cmake..."
cmake --build "$BUILD_DIR"

echo "==> Build runned successfully!"
echo "Binary pwd: ./$BUILD_DIR"