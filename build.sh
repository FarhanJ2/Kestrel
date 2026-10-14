#!/usr/bin/env bash
# Builds Kestrel inside the dev image. Usage: ./build.sh [Debug|Release] (default: Debug)
#
# The repo is mounted at the same path the dev container uses, so the build
# directory and compile_commands.json are shared between the two.
set -euo pipefail

cd "$(dirname "$0")"

build_type="${1:-Debug}"
workdir="/workspaces/$(basename "$PWD")"

docker build --target dev -t kestrel-dev .
docker run --rm -v "$PWD":"$workdir" -w "$workdir" kestrel-dev \
    bash -c "cmake -S . -B build -DCMAKE_BUILD_TYPE=$build_type && cmake --build build -j\"\$(nproc)\""
