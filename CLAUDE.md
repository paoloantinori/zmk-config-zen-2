# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ZMK firmware configuration for a **Corne-ish Zen v2** split keyboard with a 36-key layout. The keyboard uses caksoylar's fork of ZMK (not upstream) with additional modules for RGB LED widget and smart toggle.

## Build System

Firmware is built exclusively via **GitHub Actions** — there is no local build. Pushing to `main` triggers the build workflow if relevant files change.

- **Firmware build**: `.github/workflows/build.yml` — triggers on changes to `config/west.yml`, `config/*.keymap`, `config/*.dtsi`, `config/corneish_zen.conf`, `config/boards/**`, `build.yaml`. Uses ZMK's reusable `build-user-config.yml` workflow.
- **Keymap visualization**: `.github/workflows/draw-keymaps.yml` — triggers on changes to keymap/config files. Uses `caksoylar/keymap-drawer` to generate SVG diagrams in `keymap-drawer/`.

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
| `build.yaml` | Declares board halves for ZMK build (`corneish_zen_v2_left` / `corneish_zen_v2_right`) |

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

This repo uses `caksoylar/zmk` branch `zen-v1+v2`, which is **10 commits ahead of upstream ZMK `main`** but ~150 commits behind. The fork adds Corne-ish Zen-specific display and hardware patches that haven't been upstreamed:

| Area | Changes | Config options used here |
|------|---------|--------------------------|
| **Display refresh** | Periodic full refresh to clear e-ink ghosting | `CONFIG_ZMK_DISPLAY_FULL_REFRESH_PERIOD=300` |
| **Display invert** | Option to invert IL0323 epaper (white-on-black) | `CONFIG_IL0323_INVERT` (disabled) |
| **IL0323 partial refresh** | Alternative partial refresh mode for the display driver | (not enabled) |
| **Momentary layer tracking** | Tracks which layers were activated via `&mo`/`&lt` (upstream only tracks `&tog`) | `CONFIG_ZMK_TRACK_MOMENTARY_LAYERS=y` |
| **Hide momentary layers in widget** | Zen's layer status widget can hide momentary layers to reduce display flicker | `CONFIG_ZMK_DISPLAY_HIDE_MOMENTARY_LAYERS=y` |
| **Battery widget tweaks** | Skip updates when unchanged; adjusted level thresholds | (always active in Zen board code) |
| **Custom status screen layout** | Rearranged widget layout for Zen's OLED; selectable logo images (ZMK, Miryoku, LPKB) | `CONFIG_CUSTOM_WIDGET_LOGO_IMAGE_*` (not enabled) |
| **8-bit status screen** | A pixel-art status screen (in the older `zen-8bit` branch only) | `CONFIG_ZEN_STATUS_SCREEN_8BIT` (disabled) |

The previously used `zen-8bit` branch includes all of the above plus an additional 8-bit style status screen with card images. It's older and more diverged from upstream.

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
- **ZMK source**: https://github.com/zmkfirmware/zmk — the ZMK firmware codebase. Needed when the docs are insufficient — behavior implementations (`app/src/behaviors/`), devicetree bindings (`app/dts/bindings/`), Kconfig options (`app/Kconfig*`), and keycode definitions (`app/include/dt-bindings/zmk/`). Note: this repo uses caksoylar's fork (`caksoylar/zmk` branch `zen-v1+v2`), which may diverge from upstream.
- **caksoylar's keymap**: https://github.com/caksoylar/zmk-config — upstream inspiration for this config's architecture
- **Keymap editor**: https://nickcoutsos.github.io/keymap-editor/ — visual editor that can load this repo's config directly
- **keymap-drawer**: https://github.com/caksoylar/keymap-drawer — generates the SVG keymap visualizations in `keymap-drawer/`
