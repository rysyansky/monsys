#!/bin/bash
set -euo pipefail

BACKEND_DIR="$PWD/backend"
FRONTEND_DIR="$PWD/frontend"
PROJECT_DIR="$PWD"

if ! [ -d "monsys" ]; then
    mkdir -p monsys
fi

if ! [ -d "monsys/static" ]; then
    mkdir -p monsys/static
fi

cd $BACKEND_DIR

cmake --preset release
cmake --build --preset release
cp build/release/app ../monsys/

cd $FRONTEND_DIR

npm run build
rm -rf ../monsys/static/assets
mkdir -p ../monsys/static
cp dist/index.html ../monsys/static/
cp -r dist/assets ../monsys/static/