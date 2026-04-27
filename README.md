# Corne-ish Zen V2 Custom Configuration

This is my config repo for this super cool keyboard. Unfortunately the person I've bought it from chose the 36 keys layout, so my config has strange configuration due to the lack of additional keys.

I'm using a custom fork of `zmk` that rebases all Corne-ish Zen display patches from @caksoylar's `zen-v1+v2` branch on top of upstream ZMK main (Zephyr 4.1).

https://github.com/paoloantinori/zmk/tree/zen-v1+v2-rebased

I'm also using his Keymap diagram generator to produce the nice images you see below.

And, I'm also using https://nickcoutsos.github.io/keymap-editor/ as an editor to modify things on the fly.

Inspirations:

- https://github.com/maxpetretta/keymap/tree/master for my first encounter with Combos and Capsword
- https://github.com/nickfaraco/zmk-config for a reverse engineered Urob's timeless HRMs and the idea of a LOCK layer

## Flashing Firmware

Firmware is built via GitHub Actions and the UF2 artifacts are attached to each build run. To flash:

1. Download the UF2 files from the [latest successful build](https://github.com/paoloantinori/zmk-config-zen-2/actions/workflows/build.yml)
2. Connect the half you want to flash to your computer via USB
3. **Double-click** (quickly press twice) the reset button on that half
4. The keyboard will appear as a USB mass storage drive
5. Drag and drop the correct `.uf2` file onto the drive:
   - `corneish_zen_left@2_zmk.uf2` for the left half
   - `corneish_zen_right@2_zmk.uf2` for the right half
6. The drive will automatically unmount and the keyboard will reboot with the new firmware

Repeat for the other half. A single press of reset only reboots -- you need the double-click to enter bootloader mode.

## Keymap Visualization

Below representation was generated with [`keymap-drawer`](https://github.com/caksoylar/keymap-drawer) -- check out the automatically generated layouts using the [automated Github workflow](https://github.com/caksoylar/keymap-drawer/tree/main#setting-up-an-automated-drawing-workflow) for each keyboard in the [`keymap-drawer` folder](keymap-drawer/), which is always up to date with the config.

![Keymap Representation](./keymap-drawer/corneish_zen.svg?raw=true "Keymap Representation")
