# ZMK/QMK Glossary

A reference for the key concepts, behaviors, and jargon used in keyboard firmware customization. Organized by category, with examples from this repo's keymap where applicable.

---

## Key Actions & Behaviors

### Tap
Press and release a key quickly. Sends the keycode once. The most basic action — typing a letter is a tap.

### Hold
Press a key and keep it pressed. What happens on hold depends on the behavior bound to that key. Could activate a layer, send a modifier, or do nothing.

### Hold-Tap (`&ht`, `&lt`, `&mt`)
A key that does **one thing on tap** and a **different thing on hold**. The firmware waits to see if you're tapping or holding before deciding which action to take.

- **Layer-Tap (`&lt`)**: Tap = keycode, Hold = activate layer. Example from this keymap: `&lt NAV GB_SPACE` on key 31 — tap sends Space, hold activates NAV layer.
- **Mod-Tap (`&mt`)**: Tap = keycode, Hold = modifier. Example: `&mt LCTRL GB_Z` on key 20 — tap sends Z, hold sends Left Ctrl.
- **Home Row Mods (`&hml`/`&hmr`)**: Mod-taps on the home row (ASDF/HJKL positions) that use positional logic to reduce misfires. See "Home Row Mods" below.

### Tapping Term
The time threshold (in milliseconds) that decides whether a keypress is a "tap" or a "hold." If you release before the tapping term, it's a tap. If you hold past it, it's a hold. This keymap uses `TAP_TERM_MS = 200` (200ms).

### Quick-Tap
If you double-tap a hold-tap key within `quick-tap-ms`, the second press always registers as a tap (never a hold). Prevents accidental holds during fast repeated typing. Set to `QUICK_TAP_MS = 125` in this keymap.

### Flavor
How a hold-tap decides between tap and hold. The main options:

| Flavor | When does hold trigger? |
|--------|------------------------|
| **tap-preferred** | Only if no other key is pressed during the hold. Favors tapping. |
| **balanced** | If another key is pressed AND released while holding. Good for home row mods. |
| **hold-preferred** | As soon as the tapping term expires, regardless of other keys. Favors holding. |

This keymap uses `balanced` for home row mods (`&hml`/`&hmr`) and `tap-preferred` for `&lt` and `&mt`.

### Tap-Dance (`&td`)
A key that does **different things** based on how many times you tap it. Each tap must land within `tapping-term-ms` of the previous one.

Example from this keymap — `cap_after_period` on key 28:
- Single tap: `.` (period)
- Double tap: `. ` + sticky shift (auto-capitalize next letter)
- Triple tap: `...` + space (ellipsis)

**Trade-off**: Any tap-dance adds latency to the single-tap action, because the firmware must wait to see if another tap follows.

### Sticky Key (`&sk`)
A "one-shot" modifier. Tap it once, and the modifier applies to **only the next keypress**, then deactivates. No need to hold. Example: `&sk LSHFT` — tap it, then type 'a', and you get 'A'. The shift is consumed after one key.

### Mod-Morph
A key that sends **different keycodes** depending on which modifiers are currently held. The firmware checks the modifier state and picks binding 1 (no modifier) or binding 2 (modifier active).

Example concept: a key that sends `;` normally but `:` when Shift is held — without needing a separate Shift+; mapping.

### Macro (`&macro`)
A sequence of keypresses played back automatically. Can include key presses, releases, pauses, and modifier holds. Example from this keymap — `dot_spc`: types `.`, then `Space`, then activates sticky shift.

---

## Layers

### Layer
A complete set of key bindings for all physical keys. Only the "active" layers process keypresses. Layers stack — higher layers override lower ones. This keymap has 8 layers: BASE (0), NAV (1), NUM (2), FUNC (3), QUICK (4), MOUSE (5), LOCK (6), VERIFY (7).

### Momentary Layer (`&mo`)
Activates a layer **while the key is held**. Release = back to previous layer. Example: `&mo FUNC` on key 35 — hold for function keys, release to return.

### Toggle Layer (`&tog`)
**Flips** a layer on or off. Tap once = layer on (stays on after release). Tap again = layer off. Example: `&tog LOCK` locks the keyboard until the same combo is pressed again.

### Transparent Key (`&trans`, `___`)
"Pass through" — the key does whatever the next lower active layer says. If MOUSE layer has `___` at a position, and BASE layer has `Q` there, pressing that key while in MOUSE sends Q.

### None Key (`&none`, `XXXXX`)
Dead key — does absolutely nothing when pressed. Blocks pass-through to lower layers. Used to prevent accidental keypresses on layers where a key has no purpose.

### Auto-Layer / Smart Layer
A layer that **activates via a trigger** (combo, key) and **auto-deactivates** when you press a key that doesn't belong to that layer's purpose. No manual exit needed.

Examples from this keymap:
- **Numword** (`&num_word`): Activates NUM layer, stays active while typing digits, auto-exits on any non-number key. Uses the `zmk-auto-layer` module.
- **Smart Mouse** (`&smart_mouse`): Activates MOUSE layer via combo, auto-exits when you press a non-mouse key.

### Conditional Layer
A layer that activates **only when two or more other layers are simultaneously active**. Example: if NAV and NUM are both held, a COMBO layer could activate. This keymap doesn't use conditional layers, but ZMK supports them.

---

## Home Row Mods

### What They Are
Placing modifiers (Shift, Ctrl, Alt, GUI/Super) on the home row keys (A S D F / H J K L) as hold behaviors, while keeping their normal letters as tap behaviors.

This keymap's home row mods:

```
Left hand (hold):    A=plain  S=LALT   D=LSHIFT  F=LCTRL  G=plain
Right hand (hold):   H=RCTRL  J=RSHFT  K=RALT    L=plain  Enter=plain
```

### Positional Hold-Tap
The key innovation that makes home row mods usable. A left-hand home row mod **only triggers as a hold** when a right-hand key is pressed (and vice versa). This prevents false modifier triggers during fast same-hand typing.

Controlled by `hold-trigger-key-positions` — this keymap defines `LEFT_KEYS`, `RIGHT_KEYS`, and `THUMBS` position groups for this purpose.

### `require-prior-idle-ms`
Don't trigger a hold if the user was typing within the last N milliseconds. Prevents home row mods from firing during fast typing flow. Set to `75` in this keymap.

### `hold-trigger-on-release`
The hold decision waits until the **other key is released** (not just pressed). Makes the hold-tap more forgiving — you can overlap keypresses slightly without triggering a false hold.

---

## Combos

### Combo
Press **two or more keys simultaneously** (within `timeout-ms`) to trigger a different action than pressing either key alone. The keys don't have to be adjacent.

Examples from this keymap:
- Keys 0+1 (Q+W) → Escape
- Keys 13+16 (F+J) → Smart Mouse layer
- Keys 34+35 (right thumbs) → Numword

### `timeout-ms`
How close together (in milliseconds) the combo keys must be pressed. Lower = harder to trigger accidentally but requires more precise timing. This keymap uses `COMBO_TIMEOUT_MS = 80` as default, with some combos tightened to `40`.

### `require-prior-idle-ms` (on combos)
Same concept as on hold-taps: the combo won't fire if you were typing within the last N ms. Prevents combos from triggering during fast typing. Higher values = more protection, but the combo feels less responsive if you try to activate it right after typing.

---

## Mouse / Pointing

### Mouse Keys (`&mmv`, `&msc`, `&mkp`)
Keyboard keys that emulate mouse actions:
- **`&mmv`** (mouse move): Moves the cursor. Example: `&mmv MOVE_UP` on NAV layer.
- **`&msc`** (mouse scroll): Scrolls. Example: `&msc SCRL_DOWN`.
- **`&mkp`** (mouse key press): Clicks a mouse button. Example: `&mkp LCLK` for left click, `&mkp RCLK` for right click, `&mkp MCLK` for middle click, `&mkp MB4`/`MB5` for browser back/forward.

### Acceleration
Mouse keys start slow and speed up the longer you hold them. Controlled by:
- **`acceleration-exponent`**: 0 = constant speed, 1 = linear acceleration, 2 = quadratic (faster ramp-up).
- **`time-to-max-speed-ms`**: How long until the cursor reaches full speed.
- **`trigger-period-ms`**: How often movement events are generated (lower = smoother). Default 16ms (~60Hz), this keymap uses 8ms (~125Hz).

### Input Processor
A filter that modifies pointing device events before they reach the host. Can scale speed, swap axes, invert direction, or convert movement to scroll. Applied per-layer via input listener child nodes.

Key processors in this keymap:
- **`&zip_xy_scaler 1 4`**: Reduces mouse movement to 1/4 speed (precision/sniper mode). Active on NAV layer.
- **`&zip_scroll_scaler 4 1`**: Quadruples scroll speed. Active on NAV layer.

### Auto-Mouse Layer (`&zip_temp_layer`)
A ZMK input processor that **automatically activates a layer** when a physical pointing device (trackball, trackpad) generates movement, and deactivates after a timeout. Only works with hardware pointing devices, not keyboard mouse keys.

### Sniper / Precision Mode
A layer or key that dramatically reduces pointing speed for fine cursor control. Implemented via `&zip_xy_scaler` with a fractional multiplier (e.g., `1 4` = quarter speed).

### Drag-Scroll
Converting pointing device movement (X/Y) into scroll events (wheel/horizontal wheel). Hold a key to scroll by moving the trackball instead of the cursor. Implemented via `&zip_xy_to_scroll_mapper`.

---

## Caps Word (`&caps_word`)

A "smart caps lock" that auto-capitalizes letters and **auto-deactivates** when you press a key that isn't part of a typical ALL_CAPS identifier (like space or most punctuation). Unlike Caps Lock, it turns itself off.

The `continue-list` defines which non-alpha keys keep caps_word active. This keymap: `UNDERSCORE MINUS BSPC N1-N0` — so you can type `MY_VARIABLE_123` and it stays active, but pressing space exits it.

In this keymap, caps_word is on the FUNC layer (hold FUNC + tap K position).

---

## Split Keyboard Concepts

### Central / Peripheral
In a wireless split keyboard, one half is the **central** (connects to the host computer via BLE) and the other is the **peripheral** (connects to the central via BLE). The central runs the keymap logic. On the Corne-ish Zen, the left half is central by default.

### Bond / Pairing
The BLE security handshake between the keyboard and the host (or between the two halves). Stored persistently. "Clearing bonds" (`BT_CLR_ALL`) forces a full re-pair.

### Profile (`BT_SEL`)
A saved BLE connection slot. The Zen supports 5 profiles (0-4), letting you pair with 5 different computers and switch between them. `&bt BT_SEL 0` selects profile 0.

---

## Build System

### West
Zephyr's meta-tool for managing multi-repo projects. Commands used in this build:
- `west init -l` — Initialize workspace from a local manifest
- `west update` — Fetch all dependencies listed in `west.yml`
- `west build` — Compile the firmware
- `west zephyr-export` — Register Zephyr's CMake package (required for `find_package(Zephyr)` to work)

### Devicetree / DTS
The configuration language used by Zephyr (and ZMK) to describe hardware and behaviors. Your `.keymap` file is actually devicetree syntax — the `compatible`, `#binding-cells`, `bindings` properties are all devicetree concepts. Files ending in `.dtsi` are devicetree include files.

### Kconfig / `.conf`
Zephyr's configuration system for enabling/disabling features at compile time. Your `corneish_zen.conf` uses Kconfig — lines like `CONFIG_ZMK_POINTING=y` enable the pointing subsystem.

### UF2
The firmware file format used for flashing. The Zen enters UF2 bootloader mode via double-clicking the reset button (or the FUNC layer bootloader key). The keyboard appears as a USB drive — drag the `.uf2` file onto it to flash.

### Shield vs Board
In ZMK's build system:
- **Board**: The physical PCB with its MCU (e.g., `corneish_zen_left@2`). Defines hardware: GPIO pins, display, BLE radio.
- **Shield**: A keymap/config that sits on top of a generic board (e.g., a Corne shield on a nice!nano board). The Zen is a board (not a shield) because it has a custom onboard MCU.

---

## ZMK Modules

External code packages added via `config/west.yml` that extend ZMK's functionality. This keymap uses:

| Module | Purpose |
|--------|---------|
| `zmk-rgbled-widget` | RGB LED indicator for battery/BLE/layer status |
| `zmk-smart-toggle` | Smart toggle behavior (used for alt-tab swapper, could be used for mouse) |
| `zmk-auto-layer` | Auto-deactivating layer (used for Numword and Smart Mouse) |
| `zmk-leader-key` | Vim-style leader key sequences (Italian accented characters) |
| `zmk-helpers` | Convenience macros for keymap configuration |

---

## Common Abbreviations

| Abbreviation | Meaning |
|---|---|
| HRM | Home Row Mods |
| HT | Hold-Tap |
| LT | Layer-Tap |
| MT | Mod-Tap |
| TD | Tap-Dance |
| SK | Sticky Key (one-shot) |
| CW | Caps Word |
| NW | Num Word |
| AML | Auto Mouse Layer |
| DPI | Dots Per Inch (pointing device sensitivity) |
| CPI | Counts Per Inch (same as DPI, used by some sensor datasheets) |
| BLE | Bluetooth Low Energy |
| GATT | Generic Attribute Profile (BLE communication protocol) |
| MCU | Microcontroller Unit (the chip: nRF52840 on the Zen) |
| PCB | Printed Circuit Board |
| SPI | Serial Peripheral Interface (high-speed wired protocol, used by PMW3610) |
| I2C | Inter-Integrated Circuit (simpler wired protocol, used by some trackpads) |
| LVGL | Light and Versatile Graphics Library (used for the Zen's e-ink display) |
