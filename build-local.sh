#!/bin/bash
set -eo pipefail

BOARD="$1"
HALF="$2"
WORKSPACE="${3:-/tmp/zmk-build-workspace}"

if [ -f "$WORKSPACE/.west/config" ]; then
    echo "=== Reusing existing workspace: $WORKSPACE ==="
    cd "$WORKSPACE"
    rm -rf "$WORKSPACE/config"
    cp -r /work/config "$WORKSPACE/config"
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

# Ensure BT_CLR raises the profile changed event directly (not just via deferred work)
if grep -q 'clear_profile_bond(active_profile);' "$WORKSPACE/zmk/app/src/ble.c"; then
    sed -i '/^void zmk_ble_clear_bonds/,/^};$/ {
        /update_advertising();/ {
            N
            s/update_advertising();\n};/update_advertising();\n    raise_profile_changed_event();\n};/
        }
    }' "$WORKSPACE/zmk/app/src/ble.c"
    echo "Patched ble.c: BT_CLR now raises profile changed event directly"
fi

BUILD_DIR="$WORKSPACE/build-${HALF}"
west build -s zmk/app -d "$BUILD_DIR" -b "${BOARD}" -- -DZMK_CONFIG="$WORKSPACE/config"

mkdir -p /artifacts
cp "${BUILD_DIR}/zephyr/zmk.uf2" "/artifacts/corneish_zen_${HALF}_zmk.uf2"
echo "SUCCESS: corneish_zen_${HALF}_zmk.uf2"
echo "Size: $(ls -lh /artifacts/corneish_zen_${HALF}_zmk.uf2 | awk '{print $5}')"
