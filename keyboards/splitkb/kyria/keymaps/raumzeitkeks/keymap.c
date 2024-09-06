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

#include "keymap_german.h"

enum layers {
    _BASE = 0,
    _UMLT,
    _SYMB,
    _NMBR,
    _MATH,
    _FUNC,
    _NAVI,
};

enum custom_keycodes {
    _DUMMY = SAFE_RANGE,

    // Must be in the same order as in the symbols array!
    _SYMBOLS_START,
    SYM_TILD,
    SYM_COMM,
    SYM_EXLM,
    SYM_DOT,
    SYM_DLR,
    SYM_QUOT,
    SYM_EQL,
    SYM_CIRC,
    SYM_LPRN,
    SYM_RPRN,
    SYM_LCBR,
    SYM_RCBR,
    SYM_EURO,
    SYM_SLSH,
    SYM_ASTR,
    SYM_PLUS,
    SYM_PERC,
    SYM_MINS,
    NUM_TILD,
    NUM_COMM,
    NUM_DOT,
    NUM_EQL,
    NUM_CIRC,
    NUM_LPRN,
    NUM_RPRN,
    NUM_LABK,
    NUM_RABK,
    NUM_EURO,
    NUM_SLSH,
    NUM_ASTR,
    NUM_PLUS,
    NUM_PERC,
    NUM_MINS,
    _SYMBOLS_STOP,
    _SYMBOLS_NUM = _SYMBOLS_STOP - _SYMBOLS_START - 1,

    _NAVIGATION_START,
    NAV_COPY,
    NAV_PSTE,
    NAV_UNDO,
    NAV_MUTE,
    _NAVIGATION_STOP,
    _NAVIGATION_NUM = _NAVIGATION_STOP - _NAVIGATION_START - 1,

    _TAPPING_START,
    TAP_INCR,
    TAP_DECR,
    TAP_SHOW,
    _TAPPING_STOP,
    _TAPPING_NUM = _TAPPING_STOP - _TAPPING_START - 1,
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
    { DE_TILD, DE_TILD, false, false },  // NUM_TILD
    { DE_COMM, DE_COMM, false, false },  // NUM_COMM
    { DE_DOT,  DE_DOT,  false, false },  // NUM_DOT
    { DE_EQL,  DE_EQL,  false, false },  // NUM_EQL
    { DE_CIRC, DE_CIRC, true,  true  },  // NUM_CIRC
    { DE_LPRN, DE_LPRN, false, false },  // NUM_LPRN
    { DE_RPRN, DE_RPRN, false, false },  // NUM_RPRN
    { DE_LABK, DE_LABK, false, false },  // NUM_LABK
    { DE_RABK, DE_RABK, false, false },  // NUM_RABK
    { DE_EURO, DE_EURO, false, false },  // NUM_EURO
    { DE_SLSH, DE_SLSH, false, false },  // NUM_SLSH
    { DE_ASTR, DE_ASTR, false, false },  // NUM_ASTR
    { DE_PLUS, DE_PLUS, false, false },  // NUM_PLUS
    { DE_PERC, DE_PERC, false, false },  // NUM_PERC
    { DE_MINS, DE_MINS, false, false },  // NUM_MINS
};

size_t get_symbol_index(uint16_t keycode) {
    return (_SYMBOLS_START < keycode && keycode < _SYMBOLS_STOP) ? keycode - _SYMBOLS_START : 0;
}

bool process_symbols(uint16_t keycode, keyrecord_t *record) {
    if (keycode <= _SYMBOLS_START || _SYMBOLS_STOP <= keycode) {
        return true;
    }

    static uint16_t registered_keycode = KC_NO;
    static bool is_registered_dead = false;

    // If a custom shift key is registered, then this event is either
    // releasing it or manipulating another key at the same time.
    // Either way we release the currently registered key.
    if (registered_keycode != KC_NO) {
        unregister_code16(registered_keycode);
        registered_keycode = KC_NO;
        if (is_registered_dead) {
            tap_code16(KC_SPACE);
            is_registered_dead = false;
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

    // Get modifier state
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
    is_registered_dead = is_dead_symbol;

    // Reset modifiers to original state
    set_mods(mods);
    set_weak_mods(weak_mods);
    set_oneshot_mods(oneshot_mods);

    return false;
}

bool process_navigation(uint16_t keycode, keyrecord_t *record) {
    if (keycode <= _NAVIGATION_START || _NAVIGATION_STOP <= keycode) {
        return true;
    }

    // Get modifier state
    const uint8_t mods = get_mods();
    const uint8_t weak_mods = get_mods();
    const uint8_t oneshot_mods = get_mods();
    const uint8_t effective_mods = mods | weak_mods | oneshot_mods;
    const bool is_shift_pressed = (effective_mods & MOD_MASK_SHIFT) != 0;

    clear_weak_mods();
    clear_oneshot_mods();
    clear_mods();

    switch (keycode) {
        case NAV_COPY: tap_code16(is_shift_pressed ? C(DE_X) : C(DE_C)); break;
        case NAV_PSTE: tap_code16(C(DE_V)); break;
        case NAV_UNDO: tap_code16(is_shift_pressed ? C(DE_Y) : C(DE_Z)); break;
        case NAV_MUTE: tap_code16(KC_MUTE); break;
    }

    // Reset modifiers to original state
    set_mods(mods);
    set_weak_mods(weak_mods);
    set_oneshot_mods(oneshot_mods);

    return false;
}

uint16_t g_tapping_term = TAPPING_TERM;

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        default: return g_tapping_term;
    }
}

void print_number(const char* str) {
    for (; *str != '\0'; ++str) {
        switch (*str) {
            case '0': tap_code(DE_0);
            case '1': tap_code(DE_1);
            case '2': tap_code(DE_2);
            case '3': tap_code(DE_3);
            case '4': tap_code(DE_4);
            case '5': tap_code(DE_5);
            case '6': tap_code(DE_6);
            case '7': tap_code(DE_7);
            case '8': tap_code(DE_8);
            case '9': tap_code(DE_9);
        }
    }
}

bool process_tapping_term(uint16_t keycode, keyrecord_t *record) {
    if (keycode <= _TAPPING_START || _TAPPING_STOP <= keycode) {
        return true;
    }

    switch (keycode) {
        case TAP_INCR: g_tapping_term += 10; break;
        case TAP_DECR: g_tapping_term -= 10; break;
        case TAP_SHOW: {
            const char* str = get_u16_str(g_tapping_term, ' ');
            for (; *str != '\0'; ++str) {
                if ('0' <= *str && *str <= '9') {
                    const uint8_t lut[] = { DE_0, DE_1, DE_2, DE_3, DE_4, DE_5, DE_6, DE_7, DE_8, DE_9 };
                    tap_code(lut[*str - '0']);
                }
            }
            break;
        }
    }

    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    return (
        process_symbols(keycode, record) &&
        process_navigation(keycode, record) &&
        process_tapping_term(keycode, record)
    );
}

#define TO_BASE TO(_BASE)
#define TO_UMLT TO(_UMLT)
#define TO_SYMB TO(_SYMB)
#define TO_NMBR TO(_NMBR)
#define TO_MATH TO(_MATH)
#define TO_FUNC TO(_FUNC)
#define TO_NAVI TO(_NAVI)

#define MO_UMLT MO(_UMLT)
#define MO_SYMB MO(_SYMB)
#define MO_NMBR MO(_NMBR)
#define MO_MATH MO(_MATH)
#define MO_FUNC MO(_FUNC)
#define MO_NAVI MO(_NAVI)

#define UMLT(kc) LT(_UMLT, kc)
#define SYMB(kc) LT(_SYMB, kc)
#define NMBR(kc) LT(_NMBR, kc)
#define MATH(kc) LT(_MATH, kc)
#define FUNC(kc) LT(_FUNC, kc)
#define NAVI(kc) LT(_NAVI, kc)

#define LG(kc) LGUI_T(kc)
#define LC(kc) LCTL_T(kc)
#define LS(kc) LSFT_T(kc)
#define LA(kc) LALT_T(kc)
#define RG(kc) RGUI_T(kc)
#define RC(kc) RCTL_T(kc)
#define RS(kc) RSFT_T(kc)

#define _X_ KC_NO
#define __XXX__ KC_NO

#define TUP TAP_INCR
#define TDN TAP_DECR
#define TSH TAP_SHOW

#define DENSE_LAYOUT(L31, L32, L33, L34, L35, R35, R34, R33, R32, R31, L20, L21, L22, L23, L24, L25, R25, R24, R23, R22, R21, R20, L11, L12, L13, L14, L15, R15, R14, R13, R12, R11, L01, L02, L03, L04, R04, R03, R02, R01) \
    LAYOUT( _X_, L31, L32, L33, L34, L35,                     R35, R34, R33, R32, R31, _X_, \
            L20, L21, L22, L23, L24, L25,                     R25, R24, R23, R22, R21, R20, \
            _X_, L11, L12, L13, L14, L15, TDN, TUP, TSH, _X_, R15, R14, R13, R12, R11, _X_, \
                           _X_, L01, L02, L03, L04, R04, R03, R02, R01, _X_ )

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = DENSE_LAYOUT(
              DE_V,     DE_W,     DE_M,     DE_G,       DE_K,              DE_J,     FUNC(SYM_COMM), SYM_DOT,  SYM_QUOT,  DE_B,
        DE_Q, LG(DE_S), LC(DE_C), LS(DE_N), SYMB(DE_T), TO_MATH,           CW_TOGG,  NAVI(DE_A),     RS(DE_E), RC(DE_I),  RG(DE_H), DE_Z,
              DE_X,     DE_P,     DE_L,     UMLT(DE_D), TO_NAVI,           TO_SYMB,  NMBR(DE_U),     DE_O,     DE_Y,      DE_F,
                        KC_DEL,   KC_BSPC,  LA(DE_R),   KC_TAB,            KC_ESC,   LA(KC_SPC),     KC_ENT,   KC_INS
    ),
    [_UMLT] = DENSE_LAYOUT(
                    _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______,
           _______, DE_SS,   KC_LCTL, KC_LSFT, _______, __XXX__,            CW_TOGG, DE_ADIA, _______, _______, _______, _______,
                    _______, _______, _______, MO_UMLT, __XXX__,            __XXX__, DE_UDIA, DE_ODIA, _______, _______,
                             _______, _______, KC_LALT, _______,            _______, KC_SPC,  _______, _______
    ),
    [_SYMB] = DENSE_LAYOUT(
                    __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,            SYM_TILD, SYM_EXLM, SYM_DLR,  SYM_EQL, SYM_CIRC,
           __XXX__, KC_LGUI, KC_LCTL, KC_LSFT, MO_SYMB, __XXX__,            __XXX__, SYM_LPRN, SYM_RPRN, SYM_LCBR, SYM_RCBR, SYM_EURO,
                    __XXX__, __XXX__, __XXX__, __XXX__, TO_NAVI,             TO_BASE, SYM_SLSH, SYM_ASTR, SYM_PLUS, SYM_PERC,
                             _______, _______, KC_LALT, _______,            _______,  SYM_MINS, _______,  _______
    ),
    [_NMBR] = DENSE_LAYOUT(
                       __XXX__, DE_7,    DE_8,    DE_9, __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
              __XXX__, __XXX__, DE_1,    DE_2,    DE_3, TO_MATH,            __XXX__, __XXX__, KC_RSFT, KC_RCTL, KC_RGUI, __XXX__,
                       __XXX__, DE_4,    DE_5,    DE_6, TO_NAVI,             TO_SYMB,  MO_NMBR,  __XXX__, __XXX__, __XXX__,
                                _______, _______, DE_0, _______,            _______, KC_LALT, _______, _______
    ),
    [_MATH] = DENSE_LAYOUT(
                       __XXX__, DE_7,    DE_8,    DE_9, __XXX__,            NUM_TILD, NUM_COMM, NUM_DOT,  NUM_EQL,  NUM_CIRC,
              __XXX__, __XXX__, DE_1,    DE_2,    DE_3, TO_BASE,            __XXX__,  NUM_LPRN, NUM_RPRN, NUM_LABK, NUM_RABK, NUM_EURO,
                       __XXX__, DE_4,    DE_5,    DE_6, TO_NAVI,            TO_SYMB,  NUM_SLSH, NUM_ASTR, NUM_PLUS, NUM_PERC,
                                _______, _______, DE_0, _______,            _______,  NUM_MINS, _______, _______
    ),
    [_FUNC] = DENSE_LAYOUT(
                     KC_F10, KC_F7,   KC_F8,   KC_F9,   KC_MUTE,            __XXX__, MO_FUNC,   __XXX__, __XXX__, __XXX__,
            __XXX__, KC_F11, KC_F1,   KC_F2,   KC_F3,   KC_VOLD,            __XXX__, __XXX__, KC_RSFT, KC_RCTL, KC_RGUI, __XXX__,
                     KC_F12, KC_F4,   KC_F5,   KC_F6,   KC_VOLU,             TO_SYMB,  __XXX__, __XXX__, __XXX__, __XXX__,
                             _______, _______, KC_LALT, _______,            _______, KC_LALT, _______, _______
    ),
    [_NAVI] = DENSE_LAYOUT(
                 KC_PGUP, NAV_COPY, NAV_PSTE, KC_UP,    KC_END,             __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
        KC_HOME, KC_LEFT, KC_LCTL,  KC_LSFT,  KC_RIGHT, __XXX__,            __XXX__, MO_NAVI,  KC_RSFT, KC_RCTL, KC_RGUI, __XXX__,
                 KC_PGDN, NAV_UNDO, NAV_MUTE, KC_DOWN,  __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
                          _______,  _______,  KC_LALT,  _______,            _______, KC_LALT, _______, _______
    ),
};
