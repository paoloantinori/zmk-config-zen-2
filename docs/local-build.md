# Local Build Setup

Build ZMK firmware locally using the same container that GitHub Actions CI uses. No Zephyr SDK, toolchain, or Python environment installation required.

## How It Works

The `build-local.sh` script runs inside the official ZMK build container (`zmkfirmware/zmk-build-arm:stable`) and mirrors the CI build steps:

1. Copies `config/` into a fresh temp workspace
2. `west init -l config/` — initializes the West workspace from the manifest
3. `west update --fetch-opt=--filter=tree:0` — fetches all dependencies (treeless clone for speed)
4. `west zephyr-export` — registers the Zephyr CMake package (critical step, without this `find_package(Zephyr)` fails)
5. `west build -s zmk/app -b <board> -- -DZMK_CONFIG=...` — builds the firmware
6. Copies the `.uf2` artifact to `/artifacts/`

## Prerequisites

- **Podman** (default on Fedora) or **Docker**
- ~2 GB free for the container image (pulled automatically on first run)

## Usage

```bash
./fwbuild          # build both halves
./fwbuild left     # left half only
./fwbuild right    # right half only
```

Under the hood, `fwbuild` runs `build-local.sh` inside the ZMK build container via Podman.

Artifacts are written to `/tmp/zmk-artifacts/`:
- `corneish_zen_left_zmk.uf2`
- `corneish_zen_right_zmk.uf2`

## Build Times

| Phase | First run | Subsequent runs |
|-------|-----------|-----------------|
| Container pull | ~1 min (one-time) | 0 (cached) |
| `west update` | ~2-3 min | ~30s (cached in container) |
| `west build` | ~1-2 min | ~1-2 min |
| **Total** | ~4-5 min per half | ~2 min per half |

## Known Issues

### `CONFIG_ZMK_USB_LOGGING` causes Kconfig errors

The Zen board (nRF52840) does not enable Zephyr's `SERIAL` subsystem. `CONFIG_ZMK_USB_LOGGING=y` selects `USB_CDC_ACM` and `UART_CONSOLE`, both of which depend on `SERIAL`. Zephyr's Kconfig system treats these unmet dependencies as fatal errors:

```
warning: USB_CDC_ACM ... has direct dependencies SERIAL ... but is currently being y-selected by ZMK_USB_LOGGING
error: Aborting due to Kconfig warnings
```

**Fix**: Keep `CONFIG_ZMK_USB_LOGGING` commented out in `config/corneish_zen.conf`.

### `west init` cannot combine `-l` with `-m`/`--mr`

Using `west init -l config/ -m some-manifest` produces:
```
FATAL ERROR: -l cannot be combined with -m, -o or --mr
```

**Fix**: Use `west init -l config/` alone. The manifest revision is read from `west.yml` in the config directory.

### `west zephyr-export` is required

Without this step, CMake fails with `Could not find a package configuration file provided by "Zephyr"`. The step registers the Zephyr CMake package in `~/.cmake/packages/Zephyr/` inside the container so `find_package(Zephyr)` works.

## Why Not Build Natively?

Building natively requires installing:
- Zephyr SDK 0.16.x (ARM toolchain)
- West and Python dependencies
- CMake, device tree compiler, and other build tools
- Matching the exact versions CI uses

The container approach eliminates all of this by using the exact same environment CI uses. If it builds in CI, it builds locally.

## Troubleshooting

**Container not found**: Run `podman pull zmkfirmware/zmk-build-arm:stable` to pre-pull the image.

**Build fails after config change**: The script creates a fresh workspace each time, so there's no stale state. Check `config/corneish_zen.conf` for Kconfig issues.

**Permission denied on `/tmp/zmk-artifacts`**: Create the directory first: `mkdir -p /tmp/zmk-artifacts`.
