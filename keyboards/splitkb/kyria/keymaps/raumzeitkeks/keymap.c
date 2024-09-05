/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include "action.h"
#include "modifiers.h"
#include "quantum.h"

#include "keycodes.h"
#include "keymap_german.h"

enum layers {
    _BASE = 0,
    _UMLAUT,
    _NAV,
    _SYM,
    _NUM,
    _MATH,
};

enum custom_keycodes {
    _DUMMY = SAFE_RANGE,

    _SYM_START, // symbols
    SYM_TILD_NO,
    SYM_COMM_SCLN,
    SYM_EXLM_QUES,
    SYM_DOT_COLN,
    SYM_DLR_HASH,
    SYM_QUOT_DQUO,
    SYM_EQL_GRV,
    SYM_CIRC_DEG,
    SYM_LPRN_LBRC,
    SYM_RPRN_RBRC,
    SYM_LCBR_LABK,
    SYM_RCBR_RABK,
    SYM_EURO_AT,
    SYM_SLSH_BSLS,
    SYM_ASTR_PIPE,
    SYM_PLUS_AMPR,
    SYM_PERC_NO,
    SYM_MINS_UNDS,
    _SYM_STOP, // end of symbols
    _SYM_NUM = _SYM_STOP - _SYM_START - 1,
};

#define UMLAUT_T(kc) LT(_UMLAUT, kc)
#define NAV_T(kc) LT(_NAV, kc)
#define SYM_T(kc) LT(_SYM, kc)
#define NUM_T(kc) LT(_NUM, kc)
#define MATH_T(kc) LT(_MATH, kc)

#define _X_ KC_NO
#define ___ KC_TRNS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
     _X_, DE_V, DE_W, DE_M, DE_G, DE_K, DE_J, SYM_COMM_SCLN, SYM_DOT_COLN, SYM_QUOT_DQUO, DE_B, _X_,
     DE_Q, LGUI_T(DE_S), LCTL_T(DE_C), LSFT_T(DE_N), SYM_T(DE_T), _X_, _X_, NAV_T(DE_A), RSFT_T(DE_E), RCTL_T(DE_I), RGUI_T(DE_H), DE_Z,
     _X_, DE_X, DE_P, DE_L, UMLAUT_T(DE_D), _X_, _X_, _X_, _X_, _X_, _X_, NUM_T(DE_U), DE_O, DE_Y, DE_F, _X_,
     _X_, KC_DEL, KC_BSPC, LALT_T(DE_R), KC_TAB, KC_ESC, LALT_T(KC_SPC), KC_ENT, KC_INS, _X_
    ),
    [_UMLAUT] = LAYOUT(
     _X_, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, _X_,
     ___, DE_SS, KC_LCTL, KC_LSFT, ___, _X_, _X_, DE_ADIA, ___, ___, ___, ___,
     _X_, ___, ___, ___, MO(_UMLAUT), ___, _X_, _X_, _X_, _X_, _X_, DE_UDIA, DE_ODIA, ___, ___, _X_,
     _X_, ___, ___, KC_LALT, ___, ___, DE_MINS, ___, ___, _X_
    ),
    [_SYM] = LAYOUT(
     _X_, _X_, _X_, _X_, _X_, _X_, SYM_TILD_NO, SYM_EXLM_QUES, SYM_DLR_HASH, SYM_EQL_GRV, SYM_CIRC_DEG, _X_,
     _X_, KC_LGUI, KC_LCTL, KC_LSFT, MO(_SYM), _X_, _X_, SYM_LPRN_LBRC, SYM_RPRN_RBRC, SYM_LCBR_LABK, SYM_RCBR_RABK, SYM_EURO_AT,
     _X_, _X_, _X_, _X_, _X_, _X_, _X_, _X_, _X_, _X_, _X_, SYM_SLSH_BSLS, SYM_ASTR_PIPE, SYM_PLUS_AMPR, SYM_PERC_NO, _X_,
     _X_, ___, ___, KC_LALT, ___, ___, KC_LALT, ___, ___, _X_
    ),
};

typedef struct {
  uint16_t custom_keycode;
  uint16_t unshifted_keycode;
  uint16_t shifted_keycode;
  bool is_unshifted_dead;
  bool is_shifted_dead;
} symbol_key_t;

const symbol_key_t symbol_keys[] = {
    { SYM_TILD_NO, DE_TILD, KC_NO, false, false },
    { SYM_COMM_SCLN, DE_COMM, DE_SCLN, false, false },
    { SYM_EXLM_QUES, DE_EXLM, DE_QUES, false, false },
    { SYM_DOT_COLN, DE_DOT, DE_COLN, false, false },
    { SYM_DLR_HASH, DE_DLR, DE_HASH, false, false },
    { SYM_QUOT_DQUO, DE_QUOT, DE_DQUO, false, false },
    { SYM_EQL_GRV, DE_EQL, DE_GRV, false, true },
    { SYM_CIRC_DEG, DE_CIRC, DE_DEG, true, false },
    { SYM_LPRN_LBRC, DE_LPRN, DE_LBRC, false, false },
    { SYM_RPRN_RBRC, DE_RPRN, DE_RBRC, false, false },
    { SYM_LCBR_LABK, DE_LCBR, DE_LABK, false, false },
    { SYM_RCBR_RABK, DE_RCBR, DE_RABK, false, false },
    { SYM_EURO_AT, DE_EURO, DE_AT, false, false },
    { SYM_SLSH_BSLS, DE_SLSH, DE_BSLS, false, false },
    { SYM_ASTR_PIPE, DE_ASTR, DE_PIPE, false, false },
    { SYM_PLUS_AMPR, DE_PLUS, DE_AMPR, false, false },
    { SYM_PERC_NO, DE_PERC, KC_NO, false, false },
    { SYM_MINS_UNDS, DE_MINS, DE_UNDS, false, false },
};

symbol_key_t find_symbol_key(uint16_t keycode) {
    if (_SYM_START < keycode && keycode < _SYM_STOP) {
        for (int i = 0; i < _SYM_NUM; ++i) {
            if (keycode == symbol_keys[i].custom_keycode) {
                return symbol_keys[i];
            }
        }
    }
    const symbol_key_t not_found = { KC_NO, KC_NO, false, KC_NO, false };
    return not_found;
}

bool process_symbol_keys(uint16_t keycode, keyrecord_t *record) {
    static uint16_t registered_keycode = KC_NO;
    static bool registered_dead = false;

    // If a custom shift key is registered, then this event is either
    // releasing it or manipulating another key at the same time.
    // Either way we release the currently registered key.
    if (registered_keycode != KC_NO) {
        unregister_code16(registered_keycode);
        registered_keycode = KC_NO;
        if (registered_dead) {
            tap_code16(KC_SPACE);
            registered_dead = false;
        }
    }

    // Continue default handling if this is a release event.
    if (!record->event.pressed) {
        return true;
    }

    // Continue default handling if this is a tap-hold key being held.
    if ((IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode)) && record->tap.count == 0) {
        return true;
    }

    // Get current modifiers
    const uint8_t mods = get_mods();
    const uint8_t weak_mods = get_mods();
    const uint8_t oneshot_mods = get_mods();
    const uint8_t effective_mods = mods | weak_mods | oneshot_mods;
    const bool is_shift_pressed = (effective_mods & MOD_MASK_SHIFT) != 0;

    // Get symbol key information
    const symbol_key_t symbol_key = find_symbol_key(keycode);
    if (symbol_key.custom_keycode == KC_NO) {
        return true;
    }
    const uint16_t symbol_keycode = is_shift_pressed ? symbol_key.shifted_keycode : symbol_key.unshifted_keycode;
    const bool is_dead_symbol = is_shift_pressed ? symbol_key.is_shifted_dead : symbol_key.is_unshifted_dead;

    // Remove shift modifier if necessary
    const bool needs_shift_pressed = (QK_MODS_GET_MODS(symbol_keycode) & MOD_LSFT) != 0;
    if (is_shift_pressed && !needs_shift_pressed) {
        del_weak_mods(MOD_MASK_SHIFT);
        del_oneshot_mods(MOD_MASK_SHIFT);
        unregister_mods(MOD_MASK_SHIFT);
    }

    // Register keycode
    register_code16(symbol_keycode);
    registered_keycode = symbol_keycode;
    registered_dead = is_dead_symbol;

    // Reset modifiers to original state
    set_mods(mods);
    set_weak_mods(weak_mods);
    set_oneshot_mods(oneshot_mods);

    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    return (
        process_symbol_keys(keycode, record)
    );
}
