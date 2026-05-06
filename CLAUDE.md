# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ZMK firmware configuration for a **Corne-ish Zen v2** split keyboard with a 36-key layout. Uses a custom ZMK fork (`paoloantinori/zmk` branch `zen-v1+v2-rebased`) that carries Zen display patches on top of upstream ZMK main (Zephyr 4.1), plus modules for RGB LED widget and smart toggle.

## Build System

Firmware can be built via **GitHub Actions** (push to `main`) or **locally with Podman** using the same container CI uses.

### GitHub Actions (CI)

- **Firmware build**: `.github/workflows/build.yml` — triggers on changes to `config/west.yml`, `config/*.keymap`, `config/*.dtsi`, `config/corneish_zen.conf`, `config/boards/**`, `build.yaml`. Uses ZMK's reusable `build-user-config.yml` workflow.
- **Keymap visualization**: `.github/workflows/draw-keymaps.yml` — triggers on changes to keymap/config files. Uses `caksoylar/keymap-drawer` to generate SVG diagrams in `keymap-drawer/`.

### Local Build (Podman)

Uses the official ZMK build container (`zmkfirmware/zmk-build-arm:stable`) with `podman`. This mirrors the CI build exactly — same toolchain, same Zephyr SDK, no local dependency installation needed.

**Prerequisites**: Podman (available by default on Fedora).

**Build both halves:**
```bash
# Left half
podman run --rm -v "$PWD:/work:Z" -v /tmp/zmk-artifacts:/artifacts:Z \
  zmkfirmware/zmk-build-arm:stable bash /work/build-local.sh "corneish_zen_left@2//zmk" "left"

# Right half
podman run --rm -v "$PWD:/work:Z" -v /tmp/zmk-artifacts:/artifacts:Z \
  zmkfirmware/zmk-build-arm:stable bash /work/build-local.sh "corneish_zen_right@2//zmk" "right"
```

Firmware artifacts are written to `/tmp/zmk-artifacts/` as `corneish_zen_left_zmk.uf2` and `corneish_zen_right_zmk.uf2`.

**`build-local.sh`** handles the full build pipeline inside the container: copies config into a temp workspace, runs `west init -l` → `west update` → `west zephyr-export` → `west build`. The `west zephyr-export` step is critical — it registers the Zephyr CMake package so `find_package(Zephyr)` works.

**Known pitfalls:**
- `CONFIG_ZMK_USB_LOGGING=y` causes Kconfig errors on the Zen board (nRF52840) because it selects USB serial symbols that depend on `SERIAL`, which is not enabled. Use commented-out form for debug sessions only.
- `west init -l` cannot be combined with `-m` or `--mr` flags — use it plain with a clean workspace.

## Key Files

| File | Purpose |
|------|---------|
| `config/corneish_zen.keymap` | Main keymap definition (devicetree syntax) — behaviors, combos, layers |
| `config/corneish_zen.conf` | ZMK Kconfig settings (BLE, display, sleep, mouse pointing) |
| `config/west.yml` | Zephyr West manifest — pins the ZMK fork and modules |
| `config/keymap_drawer.config.yaml` | Keymap-drawer rendering config (SVG styling, glyph mappings, combo layout) |
| `config/includes/mouse.dtsi` | Mouse/pointing device configuration (move/scroll acceleration) |
| `config/italian.keycodes` | Custom keycode defines for Italian keyboard layout input |
| `config/keys_en_gb.h` | GB keycodes header (used at compile time, not for keymap-drawer) |
| `build.yaml` | Declares board halves for ZMK build (`corneish_zen_left@2//zmk` / `corneish_zen_right@2//zmk`) |
| `build-local.sh` | Local build script for Podman — runs full west pipeline inside ZMK build container |

## Inspiration and Lineage

The keymap is heavily influenced by [caksoylar/zmk-config](https://github.com/caksoylar/zmk-config), sharing these design patterns:
- **Positional home row mods** (`aml`/`amr` in caksoylar's, `hml`/`hmr` here) with `hold-trigger-key-positions` and `require-prior-idle-ms`
- **Swapper** behavior for Alt+Tab window switching (using `zmk-smart-toggle`)
- **Mouse layer** layout with same movement/scroll placement pattern (left-hand modifiers, right-hand movement)
- **Combos** for common keys (backspace, escape, tab) with horizontal key pairs
- **Layer activation** through thumb keys with `&mo`/`&lt`

Key differences from caksoylar's config:
- QWERTY alpha layout (vs Colemak-DH)
- Italian locale keycodes (`GB_` prefix) instead of standard US keycodes
- 7 layers vs 5 (adds QUICK layer, LOCK layer)
- No conditional/tri-layer for FUNC (FUNC is independently accessed via thumb)
- Uses `zmk,behavior-tri-state` for swapper (vs `zmk,behavior-smart-toggle`)
- Includes `cap_after_period` tap-dance and `lm` layer-modifier macro

## Architecture

### ZMK Fork vs Upstream

This repo uses `paoloantinori/zmk` branch `zen-v1+v2-rebased`, which carries all Corne-ish Zen patches from caksoylar's fork rebased on top of upstream ZMK main (Zephyr 4.1). The patches that haven't been upstreamed:

| Area | Changes | Config options used here |
|------|---------|--------------------------|
| **IL0323 invert** | Option to invert IL0323 epaper display (white-on-black) | `CONFIG_IL0323_INVERT` (disabled) |
| **IL0323 alt refresh** | Alternative partial refresh mode for display driver | `CONFIG_IL0323_ALTERNATIVE_REFRESH` (not enabled) |
| **Display refresh** | Periodic full refresh to clear e-ink ghosting | `CONFIG_ZMK_DISPLAY_FULL_REFRESH_PERIOD=300` |
| **Momentary layer tracking** | Tracks which layers were activated via `&mo`/`&lt` (upstream only tracks `&tog`) | `CONFIG_ZMK_TRACK_MOMENTARY_LAYERS=y` |
| **Hide momentary layers in widget** | Zen's layer status widget can hide momentary layers to reduce display flicker | `CONFIG_ZMK_DISPLAY_HIDE_MOMENTARY_LAYERS=y` |
| **Battery widget tweaks** | Skip updates when unchanged; adjusted level thresholds | (always active in Zen board code) |
| **Custom status screen layout** | Rearranged widget layout for Zen's OLED; selectable logo images (ZMK, Miryoku, LPKB, Zen) | `CONFIG_CUSTOM_WIDGET_LOGO_IMAGE_ZEN=y` |
| **Conditional layer momentary** | Propagates momentary state through conditional layer chains | (always active) |

The rebased fork adapts these patches for upstream API changes (e.g., `zmk_keymap_layer_activate(layer, bool locking)` uses a separate `mark_momentary()` function; LVGL uses `lv_image_*` not `lv_img_*`).

**Flashing**: Double-click the reset button on the half to enter UF2 bootloader mode. Artifacts: `corneish_zen_left@2_zmk.uf2` / `corneish_zen_right@2_zmk.uf2`. Single press only reboots — must double-click.

#### Additional Modules (not in ZMK fork)
- **`zmk-rgbled-widget`** (`caksoylar/zmk-rgbled-widget`) — RGB LED indicator for battery level (color-coded blinks), BLE connection status, and active layer. Used via `#include <behaviors/rgbled_widget.dtsi>` in the keymap.
- **`zmk-smart-toggle`** (`caksoylar/zmk-smart-toggle`) — Custom behavior `zmk,behavior-smart-toggle` that holds a key until a non-ignored key is pressed or the layer deactivates. Powers the "swapper" pattern for single-key Alt+Tab window switching. Note: this keymap uses `zmk,behavior-tri-state` instead, which is a different approach to the same problem.

Italian locale keycodes are included via `italian.keycodes` (compiled via `GB_` prefix keys).

### Keymap Layers (7 total)
0. **BASE** — Alpha layer with home row mods (positional hold-taps `hml`/`hmr`)
1. **NAV** — Navigation + mouse movement (left hand mouse, right hand arrows)
2. **SYMB** — Symbols and numbers
3. **FUNC** — Function keys, media controls, Bluetooth profiles
4. **QUICK** — Quick-access layer (grave, tab) via `&lm` (layer+modifier macro)
5. **MOUSE** — Dedicated mouse layer with scroll on top row, movement on home row
6. **LOCK** — Empty layer to lock keyboard (toggled via 4-key combo)

### Custom Behaviors
- **`hml`/`hmr`**: Positional hold-taps for home row mods — uses `hold-trigger-key-positions` so left-hand keys only trigger hold when right-hand keys are pressed (and vice versa). "Balanced" flavor with `require-prior-idle-ms` to avoid false triggers during fast typing.
- **`lm`**: Layer-modifier macro — activates a layer while holding a modifier (analogous to QMK's `LM()`).
- **`swapper`**: Tri-state behavior for Alt+Tab window switching.
- **`cap_after_period`**: Tap-dance — single tap: `.`, double tap: `. SPACE sticky-shift`, triple tap: `...`

### Keymap Drawing
The `#ifndef KEYMAP_DRAWER` guard in the keymap file excludes `keys_en_gb.h` during keymap-drawer parsing (the drawer uses `GB_` prefixed keycodes directly). Keymap-drawer config maps `GB_` prefixed keycodes and raw bindings to SVG glyphs/labels.

## Reference Documentation

- **ZMK docs**: https://zmk.dev/docs — primary reference for behaviors (hold-tap, combos, macros, tap-dance), keymap devicetree syntax, Kconfig options, and keycodes. Most relevant sections:
  - [Behaviors](https://zmk.dev/docs/behaviors/) — hold-tap flavors, `&mo`/`&lt`/`&tog`, combos, macros, caps word, tap-dance
  - [Keymaps](https://zmk.dev/docs/keymaps/) — layer definitions, devicetree structure, keycode references
  - [Pointing](https://zmk.dev/docs/pointing) — mouse keys, move/scroll behaviors, input processors
  - [Config/Kconfig](https://zmk.dev/docs/config/) — `.conf` file options (BLE, display, sleep, logging)
- **ZMK source**: https://github.com/zmkfirmware/zmk — the ZMK firmware codebase. Needed when the docs are insufficient — behavior implementations (`app/src/behaviors/`), devicetree bindings (`app/dts/bindings/`), Kconfig options (`app/Kconfig*`), and keycode definitions (`app/include/dt-bindings/zmk/`). Note: this repo uses the custom fork (`paoloantinori/zmk` branch `zen-v1+v2-rebased`), which carries Zen-specific patches on top of upstream.
- **caksoylar's keymap**: https://github.com/caksoylar/zmk-config — upstream inspiration for this config's architecture
- **Keymap editor**: https://nickcoutsos.github.io/keymap-editor/ — visual editor that can load this repo's config directly
- **keymap-drawer**: https://github.com/caksoylar/keymap-drawer — generates the SVG keymap visualizations in `keymap-drawer/`
