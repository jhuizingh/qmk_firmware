# CLAUDE.md

Notes for working in this fork. The upstream QMK docs cover the rest; this file
documents the holykeebs-specific build workflow.

## Compiling a holykeebs keyboard

This fork's keyboards are driven by the `users/holykeebs` userspace, which
implements the pointing device handling (Pimoroni trackball, TrackPoint, Cirque,
Azoteq TPS, PMW3360), sniping, drag-scroll, scroll lock, OLED, EEPROM config, and
split state sync. Boards select behavior at build time via `-e VAR=value` flags.

### The userspace is an external overlay (one-time setup)

`users/holykeebs` is **not committed to this repo**. It's the single source of
truth shared with the vial-qmk fork, kept in a separate [QMK External Userspace](
https://docs.qmk.fm/newbs_external_userspace) overlay repo at
`../holykeebs-userspace` (where `users/holykeebs/` actually lives).

Point the `qmk` CLI at it once — the setting is global, so it applies to every
fork you build:

```
qmk config user.overlay_dir="/home/idank/dev/qmk/holykeebs-userspace"
```

The build prefers `<overlay>/users/holykeebs` over any in-tree copy. If the
overlay isn't configured, builds fail loudly with
`fatal error: users/holykeebs/holykeebs.h: No such file or directory` — set
`user.overlay_dir` (or prefix the build with `QMK_USERSPACE=/path make ...`).
Edit the userspace in the overlay repo, not here.

### Command form

```
make <keyboard>:<keymap> -e USER_NAME=holykeebs -e POINTING_DEVICE=<dev> -e POINTING_DEVICE_POSITION=<pos> [-e MORE=val ...]
```

Verified example — single trackball on the right half:

```
make holykeebs/sweeq:hk -e USER_NAME=holykeebs -e POINTING_DEVICE=trackball -e POINTING_DEVICE_POSITION=right
```

Append `:flash` to the target to flash, or copy the resulting `<target>.uf2`
to the RP2040 bootloader drive.

### `USER_NAME=holykeebs` is mandatory (and the failure is silent)

The keymaps include the userspace header by full path
(`#include "users/holykeebs/holykeebs.h"`). So if you omit `USER_NAME`, the
build still **compiles and links successfully** — the header resolves the
`HK_*` keycode enum, and QMK's weak default hooks satisfy the linker — but the
firmware is **silently missing the entire userspace**: none of
`users/holykeebs/*.c` is compiled, so there's no pointing-device processing, no
`HK_*` keycode handling, and no EEPROM config.

`USER_NAME=holykeebs` is what pulls in `users/holykeebs/rules.mk`, which:
- adds `holykeebs.c` / `oled.c` / etc. to the build, and
- sets `SERIAL_DRIVER = vendor` for splits.

Without it, a split board instead fails later with
`SOFT_SERIAL_PIN undeclared` in `platforms/chibios/drivers/serial.c` (the
serial driver was never switched off the soft-serial default). If you see that
error, you forgot `USER_NAME=holykeebs`.

### Keyboards and keymaps

Boards driven by this userspace: `holykeebs/sweeq`, `holykeebs/spankbd`,
`holykeebs/aztec42`, `holykeebs/keyball61plus`, `crkbd/rev1`, `lily58/rev1`.

Keymap: use `hk` when a pointing device is present (it adds the pointer/scroll
layer); `via` for a build with no pointing device. (Some boards, e.g. aztec42,
use `default`/`vial` instead; keyball61plus uses `via`/`default` — see below.)

### keyball61plus (fixed dual PMW3360, runtime detection)

Unlike the modular boards, `holykeebs/keyball61plus` takes **no `POINTING_DEVICE`
var**. It's always built dual-PMW3360 combined; the board `rules.mk` declares the
fixed pointing config, and the firmware detects at runtime which side(s) actually
have a ball — one image covers left / right / dual — and updates the VIA "Ball
availability" layout to match. Handedness comes from `SPLIT_HAND_MATRIX_GRID` and
the master from USB/VBUS, so no master side is pinned and USB can go to either half.

```
make holykeebs/keyball61plus:via -e USER_NAME=holykeebs -e OLED=yes
```

Keymaps: `via` (drives the dynamic ball layout) or `default`. `OLED=yes` shows the
info panels on the master and the Keyball logo on the peripheral. Hires scroll is
**on by default** for this board (its `rules.mk` forces `HK_HIRES_SCROLL`; the board
is wired for it). Each half has a scroll wheel: rotation is an encoder
(`encoder_map`), and the wheel's press is an ordinary matrix key — the inner
row-3 key of its half (`[3,7]`/`[8,7]`). What the keys are bound to is keymap
content: it lives in the keymap sources and is documented on the docs site's
keymaps page, not here.

### Build variables

| Variable | Values | Notes |
|---|---|---|
| `USER_NAME` | `holykeebs` | **Required** — see above. |
| `POINTING_DEVICE` | `trackball`, `trackpoint`, `cirque35`, `cirque40`, `tps43`, `tps65`, or `<left>_<right>` (e.g. `trackball_cirque40`) | Omit for no pointing device. Full valid set: `VALID_POINTING_DEVICE_CONFIGURATIONS` in `users/holykeebs/rules.mk`. |
| `POINTING_DEVICE_POSITION` | `right`, `left`, `thumb`, `thumb_inner`, `thumb_outer`, `middle` | Where the (single) device sits. |
| `SIDE` | `left`, `right` | Required for dual (`<left>_<right>`) configs — build once per side. |
| `OLED` | `yes` (holykeebs OLED: status + keylog), `stock` (plain QMK OLED) | Omit for none. |
| `OLED_FLIP` | `yes` | Stock-OLED keymaps only: render the status panel on the off-hand half instead of the master. No effect with `OLED=yes` (the holykeebs OLED renders both halves at the driver's base rotation — all these boards mount both halves' OLEDs the same way, so no flip is needed or applied). |
| `BONGO_ENABLE` | `no` | Bongocat OLED animation. Compiled into `OLED=yes` builds by default and toggled at runtime (`HK_BONGO_TOGGLE`; hold shift to target the peripheral); set `no` to drop it. Defaults to showing the info panels. |
| `TRACKBALL_RGB_RAINBOW` | `yes` | Rainbow-cycle the Pimoroni trackball LED. |
| `HIRES_SCROLL` | `yes` | High-resolution (sub-line) scrolling. |
| `CONSOLE` | `yes` | Enable console for debug (`HK_DUMP`, `printf`). |

The master/peripheral side is derived automatically (`rules.mk` sets
`MASTER_SIDE` → `HK_MASTER_LEFT`/`HK_MASTER_RIGHT`); don't set it by hand.
keyball61plus is the exception: it resolves handedness from its matrix grid and
the master from USB, so no master side is pinned (the userspace skips the forced
`MASTER_*` when the board has its own `SPLIT_HAND_MATRIX_GRID`).

### Bulk builds

`build_all.py` compiles the full board × pointing-device × OLED matrix. It
always passes `USER_NAME=holykeebs` and a unique `TARGET` per build (so parallel
builds don't clobber each other). Run `python3 build_all.py` (`--help` for jobs
/ parallelism).

The matrix also covers the Vial firmware, which builds inside the sibling
`../vial-qmk` checkout (`HK_VIAL_QMK` overrides the path): `keyball61plus:vial`
plus `keyball/keyball{39,44,61}:vial`. Those keyball boards keep the stock
Keyball firmware there and take no build vars.

`python3 build_all.py --publish` is the formal release flow: it requires this
repo **and** the overlay to be on `hk-master`, clean, and in sync with origin,
forces a full rebuild, then syncs the matrix to the
`holykeebs/qmk_compiled` `latest` release (uploads everything, prunes stale
remote assets, preserves externally-built ones like the killerwhale file, and
refreshes the release notes with source provenance). Add `--dry-run` to see the
upload/prune plan without touching the release.

## My personal keymap (this fork of the fork)

Although this fork supports many keyboards, I only build one: my **Keyball61**.

My custom firmware lives at and around:
- `keyboards/keyball/keyball61/keymaps/my_miryoku/keymap.c`

That's a personal Miryoku-based keymap. It's a stock-Keyball board, **not** a
holykeebs-userspace board — it takes no `USER_NAME`/`POINTING_DEVICE` build vars
and doesn't need the `../holykeebs-userspace` overlay described above.

Flash it:
```bash
make keyball/keyball61:my_miryoku:flash -j8
```

Compile without flashing:
```bash
make keyball/keyball61:my_miryoku -j8
```

### A flash alone does not change the keymap — VIA owns it

`keymaps/my_miryoku/rules.mk` sets `VIA_ENABLE = yes` (with
`DYNAMIC_KEYMAP_LAYER_COUNT 8` in its `config.h`). With VIA enabled the **live
keymap lives in EEPROM**, not in the firmware image. The compiled
`keymaps[][][]` array only *seeds* EEPROM when EEPROM is blank or its layout
version changes — otherwise **EEPROM wins and flashing changes nothing.**

So editing `keymap.c` and flashing is only half the job:

```bash
make keyball/keyball61:my_miryoku:flash -j8
# then, on the keyboard: EE_CLR — hold right-thumb Space (layer 3) and press
# either innermost bottom-row key, so the firmware re-seeds the keymap
```

The failure mode is silent and very convincing. The flash reports
`Wrote <N> bytes`, the board reboots normally, and the new bindings just aren't
there. Worse, a key whose stored copy is `_______` is *transparent*, so it falls
through to the base layer and emits the base-layer letter instead of doing
nothing — which reads like a broken layer rather than a stale keymap.
Confirmed 2026-08-21: four consecutive clean flashes adding an F-key grid to
layer 4 had no effect, and X/C/V kept emitting `x`/`c`/`v`, until `EE_CLR`.
Hours went into the halves, the cable and the upstream merge; none was at fault.

To see what the board is **actually** running, open <https://remap-keys.app> and
read the layer there. Remap reads EEPROM over the VIA protocol, so it shows the
live keymap rather than what you compiled — a layer showing `▽` transparent
where `keymap.c` has real keycodes means EEPROM is stale. That is the fastest
way to tell "my flash didn't take" from "my keymap is wrong".

`EE_CLR` discards any VIA/Remap customizations made on the board. Treat
`keymap.c` as the source of truth and Remap as a read-only inspector, or the two
will overwrite each other.

### Getting a half into the bootloader

`make ...:flash` builds, then waits for an RP2040 to appear as the `RPI-RP2` drive.
Getting a half there is the fiddly part:

- **The controller's only button is RESET, not BOOTSEL.** Holding it — whether the
  board is running or being plugged in — does nothing. **Double-tap it**, like a
  mouse double-click. The board defines `RP2040_BOOTLOADER_DOUBLE_TAP_RESET` with a
  1-second window (`keyboards/keyball/config.h`), and that is the reliable route.
- `QK_BOOT` also works from the keymap, but the combos are **half-local**: only keys
  on the USB-connected half register when the split link is down, which it is
  whenever the two halves are running different firmware. On layer 1, `QK_BOOT` is
  the bottom-left corner key of the left half (hold the key left of `Z` to reach the
  layer); on layer 3 it's the outermost right thumb key (hold right-thumb Space).

**Flash both halves** after any non-trivial QMK bump — move the USB cable to the
other half and repeat. Split transport changes between QMK versions, so a half left
on older firmware breaks split comms in ways that look like unrelated bugs.

Handedness comes from `SPLIT_HAND_MATRIX_GRID` (hardware wiring), not `EE_HANDS`, so
clearing EEPROM cannot scramble which half is which — `EE_CLR` (layer 3) is always
safe. It is not merely "worth doing" after a large version jump: because VIA owns the
keymap (see above), `EE_CLR` is **required after any keymap change**, version jump or
not. It does discard trackball CPI/scroll settings and any Remap customizations, which
is the only cost — re-set CPI on layer 3 and `KBC_SAVE` if the pointer feels wrong.

The trackball can take a moment to come up after a flash; the firmware re-probes a
sensor that failed its first init, so give it a beat before treating it as broken.
