# Corne-ish Zen V2 Custom Configuration

ZMK firmware config for a 36-key Corne-ish Zen v2 split keyboard, using a custom ZMK fork that rebases all Zen display patches on top of upstream ZMK main (Zephyr 4.1).

**Key features**: custom e-ink display widgets, keymap verify mode, battery percentage, BT profile legend, soft-off, positional home row mods, Italian locale, local Podman builds.

## E-Ink Display

The Zen v2 has an 80x128 pixel IL0323 e-paper display on each half. This config replaces the stock display widgets with custom ones optimized for text clarity and information density.

### Left Half — Battery + BT Status + Layer

![Left half display](img/left.jpg)

The left (central) half shows three sections:

- **Battery icon + percentage** — Canvas-based widget draws a battery icon and numeric percentage (e.g. "85") side by side in a single LVGL object. A post-rendering threshold step converts any anti-aliased gray pixels to pure black/white for crisp e-ink rendering.
- **BT connection status** — Canvas widget showing the active BT profile's connection icon (connected/disconnected/advertising) with the profile number.
- **Active profile name + layer** — Native LVGL labels showing the BT profile name (e.g. "LINUX", "MAC") and the current keymap layer.

### Right Half — Battery + BT Legend

![Left half display](img/right_legend.jpg)

The right (peripheral) half shows:

- **Battery icon + percentage** — Same canvas widget as the left half.
- **BT connection status** — Short text label ("BT ON" / "BT OFF") indicating peripheral-to-central connection.
- **BT profile legend** — Multi-line label listing all 5 BT profile slots with their names (e.g. "0 LINUX", "1 MAC", "2 PHONE", "3 EBOOK", "4 —").

### Verify Mode — Keymap Learning Tool

![Left half display](img/verify1.jpg)
![Left half display](img/verify2.jpg)
![Left half display](img/verify3.jpg)
![Left half display](img/verify4.jpg)

A built-in keymap verification mode that lets you check what any key does without sending keystrokes to the host. When activated, the left display shows the layer name and key binding for each key you press.

**How to use:**
1. Pause for 1 second (idle guard), then press both outer thumb keys simultaneously to enter verify mode
2. Press any key — the display shows the active layer and the key's binding label (e.g. "SYMB" / "!")
3. Hold a layer modifier (NAV, SYMB, FUNC) to check bindings on other layers
4. Hold SHIFT/CTRL/ALT to see modifier-aware labels (display shows "BASE+S", "SYMB+C", etc.)
5. Press both outer thumb keys again to exit — the display restores to normal

**How it works:** A dedicated VERIFY layer (layer 7) sits on top of all others. Regular keys have `&none` (suppressed), thumb keys and home-row mod positions have `&trans` (pass through) so layer modifiers and SHIFT/CTRL/ALT still work. The verify widget subscribes to key position events and looks up a static label table compiled into the firmware (~1.5KB flash). No ZMK core changes needed — it's purely a config-level feature.

### Partial E-Ink Refreshes

The display uses the IL0323 e-paper controller's partial refresh mode exclusively for widget updates. Only the screen region that changed is redrawn — pressing a key in verify mode refreshes just the two label areas, not the full display. This avoids the full-screen flash typical of e-ink updates.

Key display settings in `corneish_zen.conf`:
- `CONFIG_ZMK_DISPLAY_FULL_REFRESH_PERIOD=300` — periodic full refresh every 5 minutes to clear accumulated ghosting
- `CONFIG_IL0323_ALTERNATIVE_REFRESH=y` — limits gate scanning to the partial window, reducing fading outside the update area
- `CONFIG_ZMK_DISPLAY_HIDE_MOMENTARY_LAYERS=y` — hides momentary layer changes from the layer widget, reducing unnecessary partial refreshes

The text-only widgets (profile label, layer status, BT legend, peripheral status) use native LVGL labels with fixed widths and `LV_LABEL_LONG_CLIP` mode. This ensures text changes only invalidate the label's own area, not the parent screen — keeping partial refreshes truly partial.

## Other Features

- **Soft-off** — Hold a key combo on the FUNC layer to fully power down the keyboard with near-zero battery drain. Wake by pressing the reset button
- **Smooth scrolling** — Mouse scroll axis uses ZMK's smooth scrolling mode for natural trackpad-like feel
- **BLE split battery proxy** — The central (left) half fetches the peripheral's battery level over BLE and reports both halves to the host
- **Local Podman builds** — Build firmware locally with `./fwbuild` using the same container CI uses, no toolchain setup needed
- **Board patches** — Widget customizations live in `config/patches/` and overlay the ZMK board directory at build time
- **Positional home row mods** — Left-hand keys only trigger hold when right-hand keys are pressed (and vice versa), with `require-prior-idle-ms` to prevent false triggers
- **8 keymap layers** — Base, NAV, SYMB, FUNC, QUICK, MOUSE, LOCK, VERIFY

## Changelog

### 2026-05-30 — Custom display widgets, verify mode, ZMK rebase

- **Verify mode** — keymap learning tool: toggle via thumb combo, press keys to see their binding on the e-ink display with layer and modifier awareness
- **Native LVGL label widgets** — replaced canvas-based text rendering on the right half with native labels for crisper e-ink text
- **BT profile legend** — right half shows all 5 BT profile slot names
- **Canvas threshold post-processing** — eliminates anti-aliased gray pixels from canvas widgets for sharper e-ink rendering
- **Partial e-ink refresh** — fixed-width labels prevent full-screen invalidation, only changed regions refresh
- **BT improvements** — BT_DISC key, BT_CLR_ALL combo, +8dBm TX power, output toggle
- **ZMK fork rebased** on latest upstream main (15 commits, mostly docs)

### 2026-05 — Battery display, soft-off, local builds

- **Battery icon + percentage on e-ink display** — canvas-based widget draws battery icon and percentage text into a single LVGL object
- **Soft-off behavior** — hold-to-power-down combo on FUNC layer, peripheral power cutoff during deep sleep
- **Smooth scrolling** enabled for mouse pointing (`CONFIG_ZMK_POINTING_SMOOTH_SCROLLING`)
- **BLE split battery** — central fetches and proxies peripheral battery level to host
- **Battery reporting fix** — switched from `LITHIUM_VOLTAGE` to `STATE_OF_CHARGE` mode
- **Local Podman build** — `./fwbuild` helper script using `zmkfirmware/zmk-build-arm:stable` container
- **Board patches system** — `config/patches/` overlay mechanism for customizing widgets without forking board code

### 2026-04 — ZMK fork, display patches

- Switched to custom ZMK fork (`paoloantinori/zmk` branch `zen-v1+v2-rebased`) rebased on upstream main
- All caksoylar Zen display patches working on Zephyr 4.1
- E-ink ghosting mitigation via periodic full refresh (300s)
- Momentary layer hiding to reduce display flicker

### Earlier

- Mouse keys support, RGB LED widget, smart-toggle swapper
- Italian locale keycodes, `cap_after_period` tap-dance
- Positional home row mods with `hold-trigger-key-positions`

## ZMK Fork Patches

The custom fork ([`paoloantinori/zmk` branch `zen-v1+v2-rebased`](https://github.com/paoloantinori/zmk/tree/zen-v1+v2-rebased)) carries these patches from [@caksoylar's `zen-v1+v2` branch](https://github.com/caksoylar/zmk/tree/zen-v1+v2), rebased on top of upstream ZMK main. None of these have been merged upstream:

1. **IL0323 display invert** — `CONFIG_IL0323_INVERT` — Option to invert the e-ink display (white-on-black) by accepting `MONO01` pixel format
2. **IL0323 alternative partial refresh** — `CONFIG_IL0323_ALTERNATIVE_REFRESH` — Different partial refresh approach for the display driver
3. **Periodic full display refresh** — `CONFIG_ZMK_DISPLAY_FULL_REFRESH_PERIOD` — Timer that periodically invalidates the full screen to clear e-ink ghosting artifacts
4. **Momentary layer tracking** — `CONFIG_ZMK_TRACK_MOMENTARY_LAYERS` — Tracks whether layers were activated via `&mo`/`&lt` (upstream only tracks `&tog`), using a separate `zmk_keymap_layer_mark_momentary()` function
5. **Hide momentary layers in widget** — `CONFIG_ZMK_DISPLAY_HIDE_MOMENTARY_LAYERS` — Layer status widget skips updates for momentary layers, reducing e-ink flicker
6. **Battery widget optimization** — Only updates LVGL when battery level or USB state actually changes; adjusted level thresholds (87/62/37/12/5)
7. **Custom status screen layout** — Rearranged widget layout for Zen's display with optional heading strip hiding (`CONFIG_CUSTOM_WIDGET_LAYER_STATUS_HIDE_HEADING`)
8. **Selectable logo images** — `CONFIG_CUSTOM_WIDGET_LOGO_IMAGE_ZEN/LPKB/ZMK/MIRYOKU` — Choice of logo on the right-half status screen (Zen, LPKB, ZMK, Miryoku)
9. **Conditional layer momentary propagation** — Propagates momentary state through conditional layer chains so the layer widget correctly reflects auto-activated layers

The patches were adapted for upstream API changes: `zmk_keymap_layer_activate()` gained a `bool locking` parameter (momentary tracking uses a separate function), and LVGL renamed `lv_img_*` to `lv_image_*`.

## Flashing Firmware

Firmware is built via GitHub Actions and the UF2 artifacts are attached to each build run. To flash:

1. Download the UF2 files from the [latest successful build](https://github.com/paoloantinori/zmk-config-zen-2/actions/workflows/build.yml)
2. Connect the half you want to flash to your computer via USB
3. **Double-click** (quickly press twice) the reset button on that half
4. The keyboard will appear as a USB mass storage drive
5. Drag and drop the correct `.uf2` file onto the drive:
   - `corneish_zen_left_zmk.uf2` for the left half
   - `corneish_zen_right_zmk.uf2` for the right half
6. The drive will automatically unmount and the keyboard will reboot with the new firmware

Repeat for the other half. A single press of reset only reboots -- you need the double-click to enter bootloader mode.

## Local Build

You can also build firmware locally using the same container that CI uses. No toolchain installation needed — just [Podman](https://podman.io/) (default on Fedora) or Docker.

```bash
./fwbuild          # both halves
./fwbuild left     # left only
./fwbuild right    # right only
```

The `fwbuild` helper wraps Podman and the internal `build-local.sh` script, which handles the full pipeline: initializes a West workspace from `config/west.yml`, fetches all ZMK/Zephyr dependencies, applies board patches from `config/patches/`, builds the firmware, and copies the `.uf2` artifacts to `/tmp/zmk-artifacts/`. First build takes ~3-4 minutes per half (fetching repos); subsequent builds reuse the workspace and are faster.

> **Note**: `CONFIG_ZMK_USB_LOGGING=y` does not work on the Zen board — it causes Kconfig errors because USB serial requires the `SERIAL` subsystem, which is not enabled on this nRF52840 board config. Keep it commented out.

## Keymap Visualization

Below representation was generated with [`keymap-drawer`](https://github.com/caksoylar/keymap-drawer) -- check out the automatically generated layouts using the [automated Github workflow](https://github.com/caksoylar/keymap-drawer/tree/main#setting-up-an-automated-drawing-workflow) for each keyboard in the [`keymap-drawer` folder](keymap-drawer/), which is always up to date with the config.

![Keymap Representation](./keymap-drawer/corneish_zen.svg?raw=true "Keymap Representation")
