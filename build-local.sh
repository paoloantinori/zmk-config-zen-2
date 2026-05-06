#!/bin/bash
set -e

BOARD="$1"
HALF="$2"

WORKSPACE="/tmp/zmk-build-workspace"
rm -rf "$WORKSPACE"
mkdir -p "$WORKSPACE"
cp -r /work/config "$WORKSPACE/config"
cd "$WORKSPACE"

west init -l config/ 2>&1 | tail -1
west update --fetch-opt=--filter=tree:0 2>&1 | grep -c "^=== updating"
west zephyr-export 2>&1

BUILD_DIR="$WORKSPACE/build-${HALF}"
west build -s zmk/app -d "$BUILD_DIR" -b "${BOARD}" -- -DZMK_CONFIG="$WORKSPACE/config"

mkdir -p /artifacts
cp "${BUILD_DIR}/zephyr/zmk.uf2" "/artifacts/corneish_zen_${HALF}_zmk.uf2"
echo "SUCCESS: corneish_zen_${HALF}_zmk.uf2"
echo "Size: $(ls -lh /artifacts/corneish_zen_${HALF}_zmk.uf2 | awk '{print $5}')"
