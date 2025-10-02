# QMK Firmware Repository

This is a QMK (Quantum Mechanical Keyboard) firmware repository. QMK is an open-source keyboard firmware that allows for extensive customization of mechanical keyboards.

## My Usage

Although this repository supports many keyboards, I only use it for my **Keyball keyboard**.

### Custom Firmware Location

My custom firmware is located at and around:
- `keyboards/keyball/keyball61/keymaps/my_miryoku/keymap.c`

This contains my personal Miryoku-based keymap configuration for the Keyball61 keyboard.

## Flashing Firmware

To flash the firmware to the keyboard:
```bash
make keyball/keyball61:my_miryoku:flash -j8
```

To compile without flashing:
```bash
make keyball/keyball61:my_miryoku -j8
```
