/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#include "quantum.h"

/*******************************************************************************************
 *
 * ENUM DECLARATIONS
 *
*******************************************************************************************/

enum layer_names {
  _QWERTY, // Layer 0
  _ARROW_FUNC, // Layer 1
  _MOUSE_AND_FKEYS, // Layer 2
  _RGB_KEYBOARDCFG, // Layer 3
  _NUMPAD, // Layer 4
  _SYMBOLS, // Layer 5
};

enum custom_keycodes {
  MACRO_DBL_CLICK = SAFE_RANGE
};

/*******************************************************************************************
 *
 * CUSTOM TAPPING TERM
 *
*******************************************************************************************/

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LGUI_T(KC_A):
            return TAPPING_TERM + 50;
        // case SFT_T(KC_SPC):
        //     return TAPPING_TERM + 1250;
        // case LT(1, KC_GRV):
        //     return 130;
        default:
            return TAPPING_TERM;
    }
}

/*******************************************************************************************
 *
 * CUSTOM MACROS
 *
*******************************************************************************************/

// Function to handle custom keycodes
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MACRO_DBL_CLICK:
            if (record->event.pressed) {
                // When the key is pressed, send two MS_BTN1 clicks
                // Send first click
                tap_code(MS_BTN1);
                // Add a small delay between clicks (e.g., 50ms)
                // This delay is important for the operating system to register two distinct clicks.
                // Adjust as needed for your system's responsiveness.
                wait_ms(50); // Use _delay_ms for blocking delay
                // Send second click
                tap_code(MS_BTN1);
            }
            // Return false to indicate that we have handled this keycode
            // and QMK should not process it further as a standard key.
            return false;
        default:
            // For all other keycodes, let QMK handle them normally
            return true;
    }
};


/*******************************************************************************************
 *
 * TAP DANCE
 *
*******************************************************************************************/


// Define an enum for your tap dance keys
enum tap_dance_keycodes {
    TD_MOUSE_LAYER_SET, // Your custom tap dance key
};


// Function to handle the tap dance logic
void td_my_layer_toggle_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        // Single tap: Turn the layer OFF
        layer_off(_MOUSE_AND_FKEYS);
    } else if (state->count == 2) {
        // Double tap: Turn the layer ON
        layer_on(_MOUSE_AND_FKEYS);
    }
    // For any other number of taps (e.g., triple tap), you could add more logic
    // or simply do nothing, as is the case here.
}

// This function is called when the tap dance sequence is completed or reset.
// For this specific logic, we don't need to do anything here.
// It's useful for cleaning up states if you were doing something more complex
// like momentarily enabling a modifier during a tap dance.
void td_my_layer_toggle_reset(tap_dance_state_t *state, void *user_data) { }

// Register your tap dance actions
tap_dance_action_t tap_dance_actions[] = {
    // The first argument (NULL) means no function is called on each individual tap.
    // We only care about the final state (number of taps) in `td_my_layer_toggle_finished`.
    [TD_MOUSE_LAYER_SET] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_my_layer_toggle_finished, td_my_layer_toggle_reset),
};


/*******************************************************************************************
 *
 * KEYMAP
 *
*******************************************************************************************/


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT_right_ball(
    KC_GRV   , KC_1     , KC_2     , KC_3     , KC_4     , KC_5     ,                                  KC_6     , KC_7     , KC_8     , KC_9     , KC_0     , KC_MINS  ,
    QK_BOOT  , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                  KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_BSLS  ,
    QK_REBOOT, LGUI_T(KC_A) , LALT_T(KC_S), LCTL_T(KC_D), LSFT_T(KC_F), KC_G ,                         KC_H     , RSFT_T(KC_J), RCTL_T(KC_K), RALT_T(KC_L), RGUI_T(KC_SCLN), KC_QUOT ,
    MO(1)    , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     , KC_LBRC  ,              KC_RBRC, KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , TG(2)  ,

    // Bottom Row Left
    SCRL_TO  , KC_LCTL  , KC_LALT  , KC_LGUI, LT(_NUMPAD,KC_TAB), LT(_MOUSE_AND_FKEYS,KC_ESC), LT(_ARROW_FUNC,KC_ENT),
                                                                                              // Bottom Row Right
                                                                                              LT(_SYMBOLS,KC_BSPC) , LT(_RGB_KEYBOARDCFG,KC_SPACE) , LT(6,KC_DEL) , TD(TD_MOUSE_LAYER_SET)
  ),

  [_ARROW_FUNC] = LAYOUT_right_ball(
    S(KC_ESC), S(KC_1)  , S(KC_2)  , S(KC_3)  , S(KC_4)  , S(KC_5)  ,                                  S(KC_6)  ,S(KC_7), S(KC_8)  , S(KC_9)  , S(KC_0)  , _______ ,
    S(KC_DEL),  _______ , _______  , _______  , _______  , _______  ,                                  G(KC_R)  , G(KC_V)  , G(KC_C)  , G(KC_X)  , G(KC_Z)  , S(KC_INT3) ,
    S(KC_TAB), _______  , _______  , _______  , _______  , _______  ,                                  KC_LEFT  , KC_DOWN  , KC_UP  , KC_RIGHT  , KC_QUOT  , S(KC_2)  ,
    QK_REBOOT  , _______  , _______  , _______  , _______  , _______  , _______ ,         S(KC_BSLS)  , KC_INS  , KC_HOME  , KC_PGDN , KC_PGUP, KC_END , QK_REBOOT ,
    QK_BOOT, S(KC_LCTL)  , S(KC_LALT)  , S(KC_LGUI) , _______ , _______ , _______ ,    _______ , _______ , LT(6,KC_DEL) , QK_BOOT
  ),

  [_MOUSE_AND_FKEYS] = LAYOUT_right_ball(
    SSNP_FRE , KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                                  KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   , KC_F11   ,
    SSNP_VRT , _______  , _______ , _______ , _______ , _______  ,                                  MACRO_DBL_CLICK  ,  QK_MOUSE_WHEEL_LEFT , QK_MOUSE_WHEEL_UP , QK_MOUSE_WHEEL_RIGHT  , _______  , KC_F12   ,
    SSNP_HOR , _______  , _______ , _______ , _______ , _______ ,                                  MACRO_DBL_CLICK  , QK_MOUSE_BUTTON_1 , QK_MOUSE_BUTTON_3  , QK_MOUSE_BUTTON_2  , _______  , _______  ,
    _______  , _______  , _______ , _______ , _______ , _______ , _______  ,            _______  , KC_PGDN  , _______  ,  QK_MOUSE_WHEEL_DOWN  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______ , _______ , _______ , _______ ,             _______ , _______ , _______ ,_______
  ),

  [_RGB_KEYBOARDCFG] = LAYOUT_right_ball(
    RGB_TOG  , AML_TO   , AML_I50  , AML_D50  , _______  , DT_PRNT  ,                                  RGB_M_P  , RGB_M_B  , RGB_M_R  , RGB_M_SW , RGB_M_SN , RGB_M_K  ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , _______  , DT_UP    ,                                  RGB_M_X  , RGB_M_G  , RGB_M_T  , RGB_M_TW , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , _______  , DT_DOWN  ,                                  CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , KBC_SAVE , KBC_RST  ,
    _______  , _______  , SCRL_DVD , SCRL_DVI , SCRL_MO  , SCRL_TO  , EE_CLR   ,            EE_CLR   , KC_HOME  , KC_PGDN  , KC_PGUP  , KC_END   , _______  , _______  ,
    _______  , _______  , _______  ,  _______ , _______ , _______ , _______ ,               _______ , _______ , _______ , QK_BOOT
  ),

  [_NUMPAD] = LAYOUT_right_ball(
    _______  , _______   , _______  , _______  , _______  , _______  ,                                  _______  , _______  , S(KC_8)  , S(KC_9) , S(KC_0) , KC_KP_MINUS  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,                                   _______  , KC_7     , KC_8     , KC_9    , KC_KP_PLUS  , KC_KP_PLUS  ,
    _______ , _______  , _______  , _______  , _______  , _______  ,                                    _______  , KC_4    , KC_5    , KC_6     , KC_KP_EQUAL , KC_KP_EQUAL  ,
    _______  , _______  , _______ , _______ , _______  , _______  , _______   ,            _______   , _______  , KC_1     , KC_2     , KC_3      , KC_SLASH  , _______  ,
    _______  , _______  , _______  , _______ , _______ , _______ , _______ ,    KC_DOT , KC_0 , _______ , _______
  ),

  [_SYMBOLS] = LAYOUT_right_ball(
    _______  , _______   , _______  , _______  , _______  , _______  ,                                  _______  , _______  , _______  , _______ , _______ , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,                                  _______  , _______  , _______  , _______ , _______  , _______  ,
    _______ , _______  , _______  , _______  , _______  , _______  ,                                  _______  , _______ , _______ , _______  , _______ , _______  ,
    _______  , _______  , _______ , _______ , _______  , _______  , _______ ,            _______   , _______  , _______  , _______  , _______   , _______  , _______  ,
    _______  , _______  , _______  , _______ , _______ , _______ , _______ ,               _______   , _______  , _______ , _______
  ),

  [6] = LAYOUT_right_ball(
    _______  , _______   , _______  , _______  , _______  , _______  ,                                  _______  , _______  , _______  , _______ , _______ , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,                                  _______  , _______  , _______  , _______ , _______  , _______  ,
    _______ , _______  , _______  , _______  , _______  , _______  ,                                  _______  , _______ , _______ , _______  , _______ , _______  ,
    _______  , _______  , _______ , _______ , _______  , _______  , _______   ,            _______   , _______  , _______  , _______  , _______   , _______  , _______  ,
    _______  , KC_LCTL  , KC_LALT  , KC_LGUI , LT(1,KC_LNG2) , LT(2,KC_SPC) , LT(3,KC_LNG1) ,    LT(2,KC_ENT) , LT(1,KC_LNG2) , LT(6,KC_DEL) , QK_BOOT
  ),

  [7] = LAYOUT_right_ball(
    _______  , _______   , _______  , _______  , _______  , _______  ,                                  _______  , _______  , _______  , _______ , _______ , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,                                  _______  , _______  , _______  , _______ , _______  , _______  ,
    _______ , _______  , _______  , _______  , _______  , _______  ,                                  _______  , _______ , _______ , _______  , _______ , _______  ,
    _______  , _______  , _______ , _______ , _______  , _______  , _______   ,            _______   , _______  , _______  , _______  , _______   , _______  , _______  ,
    _______  , KC_LCTL  , KC_LALT  , KC_LGUI , LT(1,KC_LNG2) , LT(2,KC_SPC) , LT(3,KC_LNG1) ,    LT(2,KC_ENT) , LT(1,KC_LNG2) , LT(6,KC_DEL) , QK_BOOT
  ),
};
// clang-format on


/*******************************************************************************************
 *
 * SCROLL MODE
 *
*******************************************************************************************/
layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(get_highest_layer(state) == 3);
    return state;
}

/*******************************************************************************************
 *
 * OLED
 *
*******************************************************************************************/
#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif


