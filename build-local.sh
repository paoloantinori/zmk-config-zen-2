#!/bin/bash
set -eo pipefail

BOARD="$1"
HALF="$2"
WORKSPACE="${3:-/tmp/zmk-build-workspace}"

if [ -f "$WORKSPACE/.west/config" ]; then
    echo "=== Reusing existing workspace: $WORKSPACE ==="
    cd "$WORKSPACE"
    # Re-copy config from host to pick up any keymap/conf changes
    rm -rf "$WORKSPACE/config"
    cp -r /work/config "$WORKSPACE/config"
    # Quick update in case west.yml changed
    west update --fetch-opt=--filter=tree:0 2>&1 | grep -c "^=== updating" || true
    west zephyr-export 2>&1
else
    echo "=== Initializing new workspace: $WORKSPACE ==="
    rm -rf "${WORKSPACE:?}"/* "${WORKSPACE:?}"/.[!.]* 2>/dev/null || true
    cp -r /work/config "$WORKSPACE/config"
    cd "$WORKSPACE"

    west init -l config/ 2>&1 | tail -1
    west update --fetch-opt=--filter=tree:0 2>&1 | grep -c "^=== updating" || true
    west zephyr-export 2>&1
fi

# Apply board file overlays (e.g. modified widgets)
if [ -d "$WORKSPACE/config/patches" ]; then
    echo "Applying board file patches..."
    shopt -s nullglob
    patches=("$WORKSPACE/config/patches"/*)
    if [ ${#patches[@]} -gt 0 ]; then
        cp -rv "${patches[@]}" "$WORKSPACE/zmk/app/boards/"
    fi
    shopt -u nullglob
fi

BUILD_DIR="$WORKSPACE/build-${HALF}"
west build -s zmk/app -d "$BUILD_DIR" -b "${BOARD}" -- -DZMK_CONFIG="$WORKSPACE/config"

mkdir -p /artifacts
cp "${BUILD_DIR}/zephyr/zmk.uf2" "/artifacts/corneish_zen_${HALF}_zmk.uf2"
echo "SUCCESS: corneish_zen_${HALF}_zmk.uf2"
echo "Size: $(ls -lh /artifacts/corneish_zen_${HALF}_zmk.uf2 | awk '{print $5}')"
