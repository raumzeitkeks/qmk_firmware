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
    _SYM,
    _NUM,
    _MATH,
    _FN,
    _NAV,
};

enum custom_keycodes {
    _DUMMY = SAFE_RANGE,

    // Symbols (must be in the same order as in the symbols array!)
    _SYMBOLS_START,
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
    MATH_TILD,
    MATH_COMM,
    MATH_DOT,
    MATH_EQL,
    MATH_CIRC,
    MATH_LPRN,
    MATH_RPRN,
    MATH_LABK,
    MATH_RABK,
    MATH_EURO,
    MATH_SLSH,
    MATH_ASTR,
    MATH_PLUS,
    MATH_PERC,
    MATH_MINS,
    _SYMBOLS_STOP,
    _SYMBOLS_NUM = _SYMBOLS_STOP - _SYMBOLS_START - 1,
    // Symbols End
};

typedef struct {
  uint16_t unshifted_keycode;
  uint16_t shifted_keycode;
  bool is_unshifted_dead;
  bool is_shifted_dead;
} symbol_t;

// Must be in the same order as in the custom_keycodes enum!
const symbol_t symbols[_SYMBOLS_NUM + 1] = {
    { KC_NO,   KC_NO,   false, false },  // not a symbol
    { DE_TILD, KC_NO,   false, false },  // SYM_TILD_NO
    { DE_COMM, DE_SCLN, false, false },  // SYM_COMM_SCLN
    { DE_EXLM, DE_QUES, false, false },  // SYM_EXLM_QUES
    { DE_DOT,  DE_COLN, false, false },  // SYM_DOT_COLN
    { DE_DLR,  DE_HASH, false, false },  // SYM_DLR_HASH
    { DE_QUOT, DE_DQUO, false, false },  // SYM_QUOT_DQUO
    { DE_EQL,  DE_GRV,  false, true  },  // SYM_EQL_GRV
    { DE_CIRC, DE_DEG,  true,  false },  // SYM_CIRC_DEG
    { DE_LPRN, DE_LBRC, false, false },  // SYM_LPRN_LBRC
    { DE_RPRN, DE_RBRC, false, false },  // SYM_RPRN_RBRC
    { DE_LCBR, DE_LABK, false, false },  // SYM_LCBR_LABK
    { DE_RCBR, DE_RABK, false, false },  // SYM_RCBR_RABK
    { DE_EURO, DE_AT,   false, false },  // SYM_EURO_AT
    { DE_SLSH, DE_BSLS, false, false },  // SYM_SLSH_BSLS
    { DE_ASTR, DE_PIPE, false, false },  // SYM_ASTR_PIPE
    { DE_PLUS, DE_AMPR, false, false },  // SYM_PLUS_AMPR
    { DE_PERC, KC_NO,   false, false },  // SYM_PERC_NO
    { DE_MINS, DE_UNDS, false, false },  // SYM_MINS_UNDS
    { DE_TILD, DE_TILD, false, false },  // MATH_TILD
    { DE_COMM, DE_COMM, false, false },  // MATH_COMM
    { DE_DOT,  DE_DOT,  false, false },  // MATH_DOT
    { DE_EQL,  DE_EQL,  false, false },  // MATH_EQL
    { DE_CIRC, DE_CIRC, true,  true  },  // MATH_CIRC
    { DE_LPRN, DE_LPRN, false, false },  // MATH_LPRN
    { DE_RPRN, DE_RPRN, false, false },  // MATH_RPRN
    { DE_LABK, DE_LABK, false, false },  // MATH_LABK
    { DE_RABK, DE_RABK, false, false },  // MATH_RABK
    { DE_EURO, DE_EURO, false, false },  // MATH_EURO
    { DE_SLSH, DE_SLSH, false, false },  // MATH_SLSH
    { DE_ASTR, DE_ASTR, false, false },  // MATH_ASTR
    { DE_PLUS, DE_PLUS, false, false },  // MATH_PLUS
    { DE_PERC, DE_PERC, false, false },  // MATH_PERC
    { DE_MINS, DE_MINS, false, false },  // MATH_MINS
};

size_t get_symbol_index(uint16_t keycode) {
    return (_SYMBOLS_START < keycode && keycode < _SYMBOLS_STOP) ? keycode - _SYMBOLS_START : 0;
}

bool process_symbols(uint16_t keycode, keyrecord_t *record) {
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
    const symbol_t symbol = symbols[get_symbol_index(keycode)];
    const uint16_t symbol_keycode = is_shift_pressed ? symbol.shifted_keycode : symbol.unshifted_keycode;
    const bool is_dead_symbol = is_shift_pressed ? symbol.is_shifted_dead : symbol.is_unshifted_dead;

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
        process_symbols(keycode, record)
    );
}

#define UMLAUT_T(kc) LT(_UMLAUT, kc)
#define SYM_T(kc) LT(_SYM, kc)
#define NUM_T(kc) LT(_NUM, kc)
#define MATH_T(kc) LT(_MATH, kc)
#define FN_T(kc) LT(_FN, kc)
#define NAV_T(kc) LT(_NAV, kc)

#define _X_ KC_NO
#define ___ KC_TRNS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
     _X_, DE_V, DE_W, DE_M, DE_G, DE_K, DE_J, FN_T(SYM_COMM_SCLN), SYM_DOT_COLN, SYM_QUOT_DQUO, DE_B, _X_,
     DE_Q, LGUI_T(DE_S), LCTL_T(DE_C), LSFT_T(DE_N), SYM_T(DE_T), TO(_MATH), CW_TOGG, NAV_T(DE_A), RSFT_T(DE_E), RCTL_T(DE_I), RGUI_T(DE_H), DE_Z,
     _X_, DE_X, DE_P, DE_L, UMLAUT_T(DE_D), TO(_NAV), _X_, _X_, _X_, _X_, TO(_SYM), NUM_T(DE_U), DE_O, DE_Y, DE_F, _X_,
     _X_, KC_DEL, KC_BSPC, LALT_T(DE_R), KC_TAB, KC_ESC, LALT_T(KC_SPC), KC_ENT, KC_INS, _X_
    ),
    [_UMLAUT] = LAYOUT(
     _X_, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, _X_,
     ___, DE_SS, KC_LCTL, KC_LSFT, ___, _X_, CW_TOGG, DE_ADIA, ___, ___, ___, ___,
     _X_, ___, ___, ___, MO(_UMLAUT), _X_, _X_, _X_, _X_, _X_, _X_, DE_UDIA, DE_ODIA, ___, ___, _X_,
     _X_, ___, ___, KC_LALT, ___, ___, KC_SPC, ___, ___, _X_
    ),
    [_SYM] = LAYOUT(
     _X_, _X_, _X_, _X_, _X_, _X_, SYM_TILD_NO, SYM_EXLM_QUES, SYM_DLR_HASH, SYM_EQL_GRV, SYM_CIRC_DEG, _X_,
     _X_, KC_LGUI, KC_LCTL, KC_LSFT, MO(_SYM), _X_, _X_, SYM_LPRN_LBRC, SYM_RPRN_RBRC, SYM_LCBR_LABK, SYM_RCBR_RABK, SYM_EURO_AT,
     _X_, _X_, _X_, _X_, _X_, TO(_NAV), _X_, _X_, _X_, _X_, TO(_BASE), SYM_SLSH_BSLS, SYM_ASTR_PIPE, SYM_PLUS_AMPR, SYM_PERC_NO, _X_,
     _X_, ___, ___, KC_LALT, ___, ___, KC_LALT, ___, ___, _X_
    ),
    [_NUM] = LAYOUT(
     _X_, _X_, DE_7, DE_8, DE_9, _X_, _X_, _X_, _X_, _X_, _X_, _X_,
     _X_, _X_, DE_1, DE_2, DE_3, TO(_MATH), _X_, _X_, KC_RSFT, KC_RCTL, KC_RGUI, _X_,
     _X_, _X_, DE_4, DE_5, DE_6, TO(_NAV), _X_, _X_, _X_, _X_, TO(_SYM), MO(_NUM), _X_, _X_, _X_, _X_,
     _X_, ___, ___, DE_0, ___, ___, KC_LALT, ___, ___, _X_
    ),
    [_MATH] = LAYOUT(
     _X_, _X_, DE_7, DE_8, DE_9, _X_, MATH_TILD, MATH_COMM, MATH_DOT, MATH_EQL, MATH_CIRC, _X_,
     _X_, _X_, DE_1, DE_2, DE_3, TO(_BASE), _X_, MATH_LPRN, MATH_RPRN, MATH_LABK, MATH_RABK, MATH_EURO,
     _X_, _X_, DE_4, DE_5, DE_6, TO(_NAV), _X_, _X_, _X_, _X_, TO(_SYM), MATH_SLSH, MATH_ASTR, MATH_PLUS, MATH_PERC, _X_,
     _X_, ___, ___, DE_0, ___, ___, MATH_MINS, ___, ___, _X_
    ),
    [_FN] = LAYOUT(
     _X_, KC_F10, KC_F7, KC_F8, KC_F9, _X_, _X_, MO(_FN), _X_, _X_, _X_, _X_,
     _X_, KC_F11, KC_F1, KC_F2, KC_F3, TO(_MATH), _X_, _X_, KC_RSFT, KC_RCTL, KC_RGUI, _X_,
     _X_, KC_F12, KC_F4, KC_F5, KC_F6, TO(_NAV), _X_, _X_, _X_, _X_, TO(_SYM), _X_, _X_, _X_, _X_, _X_,
     _X_, ___, ___, KC_LALT, ___, ___, KC_LALT, ___, ___, _X_
    ),
    [_NAV] = LAYOUT(
     _X_, KC_PGUP, C(DE_X), C(DE_C), KC_UP, KC_END, _X_, _X_, _X_, _X_, _X_, _X_,
     KC_HOME, KC_LEFT, KC_LCTL, KC_LSFT, KC_RIGHT, _X_, _X_, MO(_NAV), KC_RSFT, KC_RCTL, KC_RGUI, _X_,
     _X_, KC_PGDN, _X_, C(DE_V), KC_DOWN, _X_, _X_, _X_, _X_, _X_, _X_, _X_, _X_, _X_, _X_, _X_,
     _X_, ___, ___, KC_LALT, ___, ___, KC_LALT, ___, ___, _X_
    ),
};
