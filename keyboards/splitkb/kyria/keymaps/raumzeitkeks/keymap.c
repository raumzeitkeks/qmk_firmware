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
    _SYML,
    _SYMH,
    _NMBR,
    _MATH,
    _FUNC,
    _NAVI,
};

enum custom_keycodes {
    TAP_INC = SAFE_RANGE,
    TAP_DEC,
    TAP_OUT,
};

uint16_t g_tapping_term = TAPPING_TERM;

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    return g_tapping_term;
}

bool process_tapping_term(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case TAP_INC: {
                g_tapping_term += 10;
                return false;
            }
            case TAP_DEC: {
                g_tapping_term -= 10;
                return false;
            }
            case TAP_OUT: {
                const char* str = get_u16_str(g_tapping_term, ' ');
                for (; *str != '\0'; ++str) {
                    if ('0' <= *str && *str <= '9') {
                        const uint8_t lut[] = { DE_0, DE_1, DE_2, DE_3, DE_4, DE_5, DE_6, DE_7, DE_8, DE_9 };
                        tap_code(lut[*str - '0']);
                    }
                }
                return false;
            }
        }
    }
    return true;
}

bool process_shortcut(uint16_t keycode, keyrecord_t *record) {
    const uint8_t mods = get_mods();
    const uint8_t weak_mods = get_mods();
    const uint8_t oneshot_mods = get_mods();
    const bool is_shift_pressed = ((mods | weak_mods | oneshot_mods) & MOD_MASK_SHIFT) != 0;

    uint16_t shortcut_keycode = KC_NO;
    switch (QK_MOD_TAP_GET_TAP_KEYCODE(keycode)) {
        case KC_CUT: {
            shortcut_keycode = C(DE_X);
            break;
        }
        case KC_COPY: {
            shortcut_keycode = C(DE_C);
            break;
        }
        case KC_PSTE: {
            shortcut_keycode = C(DE_V);
            break;
        }
        case KC_UNDO: {
            shortcut_keycode = is_shift_pressed ? C(DE_Y) : C(DE_Z);
            break;
        }
    }

    // Continue default handling if this is not a shortcut key.
    if (shortcut_keycode == KC_NO) {
        return true;
    }

    if (record->event.pressed) {
        clear_oneshot_mods();
        clear_weak_mods();
        clear_mods();

        tap_code16(shortcut_keycode);

        set_mods(mods);
        set_weak_mods(weak_mods);
        set_oneshot_mods(oneshot_mods);
    }

    return false;
}

bool is_dead_key(uint16_t keycode) {
    switch (keycode) {
        case DE_CIRC: return true;
        case DE_ACUT: return true;
        case DE_GRV:  return true;
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    // Continue default handling if this is a tap-hold being held.
    if ((IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode)) && record->tap.count == 0) {
        return true;
    }

    static uint16_t registered_dead_keycode = KC_NO;

    // Dead keys must always be released and finalized before the next event is handled.
    if (registered_dead_keycode != KC_NO) {
        unregister_code16(registered_dead_keycode);
        registered_dead_keycode = KC_NO;
        tap_code16(KC_SPACE);
    }

    if (record->event.pressed && is_dead_key(keycode)) {
        register_code16(keycode);
        registered_dead_keycode = keycode;
        return false;
    }

    return (
        process_tapping_term(keycode, record) &&
        process_shortcut(keycode, record)
    );
}

#define __XXX__ KC_NO

#define TO_BASE TO(_BASE)
#define TO_UMLT TO(_UMLT)
#define TO_SYMH TO(_SYML)
#define TO_SYML TO(_SYMH)
#define TO_NMBR TO(_NMBR)
#define TO_MATH TO(_MATH)
#define TO_FUNC TO(_FUNC)
#define TO_NAVI TO(_NAVI)

#define MO_UMLT MO(_UMLT)
#define MO_SYML MO(_SYML)
#define MO_SYMH MO(_SYMH)
#define MO_NMBR MO(_NMBR)
#define MO_MATH MO(_MATH)
#define MO_FUNC MO(_FUNC)
#define MO_NAVI MO(_NAVI)

#define UMLT(kc) LT(_UMLT, kc)
#define SYML(kc) LT(_SYML, kc)
#define SYMH(kc) LT(_SYMH, kc)
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
#define INC TAP_INC
#define DEC TAP_DEC
#define OUT TAP_OUT

#define DENSE_LAYOUT(L30, L31, L32, L33, L34, L35, R35, R34, R33, R32, R31, R30, L20, L21, L22, L23, L24, L25, R25, R24, R23, R22, R21, R20, L11, L12, L13, L14, L15, R15, R14, R13, R12, R11, L01, L02, L03, L04, R04, R03, R02, R01) \
    LAYOUT( L30, L31, L32, L33, L34, L35,                     R35, R34, R33, R32, R31, R30, \
            L20, L21, L22, L23, L24, L25,                     R25, R24, R23, R22, R21, R20, \
            _X_, L11, L12, L13, L14, L15, INC, DEC, OUT, _X_, R15, R14, R13, R12, R11, _X_, \
                           _X_, L01, L02, L03, L04, R04, R03, R02, R01, _X_ )

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = DENSE_LAYOUT(
              KC_ESC,   DE_V,     DE_W,     DE_M,       DE_G,       DE_K,               DE_J,    DE_DOT,     DE_COMM,    DE_QUOT,  DE_B,     CW_TOGG,
              LG(DE_Q), LS(DE_S), LC(DE_C), SYMH(DE_N), SYML(DE_T), KC_TAB,             DE_QUES, NAVI(DE_A), NMBR(DE_E), RC(DE_I), RS(DE_H), RG(DE_Z),
                        DE_X,     DE_P,     DE_L,       UMLT(DE_D), KC_MUTE,            KC_INS,  FUNC(DE_U), DE_O,       DE_Y,     DE_F,
                                  KC_DEL,   LA(DE_R),   KC_BSPC,    TO_NAVI,            TO_MATH, KC_ENT,     LA(KC_SPC), __XXX__
    ),
    [_UMLT] = DENSE_LAYOUT(
                       _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, CW_TOGG,
                       _______, DE_SS,   _______, _______, _______, _______,            _______, DE_ADIA, _______, _______, _______, DE_SS,
                                _______, _______, _______, MO_UMLT, _______,            _______, DE_UDIA, DE_ODIA, _______, _______,
                                         _______, _______, _______, __XXX__,            __XXX__, _______,  _______, _______
    ),
    [_SYML] = DENSE_LAYOUT(
                       _______, __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,            DE_TILD, DE_COLN, DE_DLR,  DE_DQUO, DE_CIRC, __XXX__,
                       KC_LGUI, KC_LSFT, KC_LCTL, __XXX__, MO_SYML, _______,            DE_EXLM, DE_LPRN, DE_RPRN, DE_LCBR, DE_RCBR, DE_EURO,
                                __XXX__, __XXX__, __XXX__, __XXX__, _______,            __XXX__, DE_SLSH, DE_ASTR, DE_PLUS, DE_PERC,
                                         _______, KC_LALT, _______, __XXX__,            __XXX__, _______, DE_MINS, _______
    ),
    [_SYMH] = DENSE_LAYOUT(
                       _______, __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,            __XXX__, DE_SCLN, DE_HASH, DE_GRV,  DE_DEG,  __XXX__,
                       KC_LGUI, KC_LSFT, KC_LCTL, MO_SYMH, __XXX__, _______,            DE_EQL,  DE_LBRC, DE_RBRC, DE_LABK, DE_RABK, DE_AT,
                                __XXX__, __XXX__, __XXX__, __XXX__, _______,            __XXX__, DE_BSLS, DE_PIPE, DE_AMPR, __XXX__,
                                         _______, KC_LALT, _______, __XXX__,            __XXX__, _______, DE_UNDS, _______
    ),
    [_NMBR] = DENSE_LAYOUT(
                          _______, __XXX__, DE_7,    DE_8, DE_9,    __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
                          __XXX__, __XXX__, DE_1,    DE_2, DE_3,    _______,            __XXX__, __XXX__, MO_NMBR, KC_RCTL, KC_RSFT, KC_RGUI,
                                   __XXX__, DE_4,    DE_5, DE_6,    _______,            _______, __XXX__, __XXX__, __XXX__, __XXX__,
                                            _______, DE_0, _______, __XXX__,            __XXX__, _______, KC_LALT, _______
    ),
    [_MATH] = DENSE_LAYOUT(
                          TO_BASE, __XXX__, DE_7,    DE_8, DE_9,    _______,            DE_TILD, DE_DOT,  DE_COMM, DE_QUOT, DE_CIRC, __XXX__,
                          __XXX__, __XXX__, DE_1,    DE_2, DE_3,    _______,            DE_EQL,  DE_LPRN, DE_RPRN, DE_LABK, DE_RABK, DE_EURO,
                                   __XXX__, DE_4,    DE_5, DE_6,    _______,            _______, DE_SLSH, DE_ASTR, DE_PLUS, DE_PERC,
                                            _______, DE_0, _______, __XXX__,            TO_BASE, _______, DE_MINS, _______
    ),
    [_FUNC] = DENSE_LAYOUT(
                        _______, KC_F10, KC_F7,   KC_F8,   KC_F9,   __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
                        __XXX__, KC_F11, KC_F1,   KC_F2,   KC_F3,   _______,            __XXX__, __XXX__, __XXX__, KC_RCTL, KC_RSFT, KC_RGUI,
                                 KC_F12, KC_F4,   KC_F5,   KC_F6,   _______,            _______, MO_FUNC, __XXX__, __XXX__, __XXX__,
                                         _______, KC_LALT, _______, __XXX__,            __XXX__, _______, KC_LALT, _______
    ),
    [_NAVI] = DENSE_LAYOUT(
          TO_BASE,     KC_PSTE,     KC_VOLU,     KC_LEFT, KC_RIGHT, KC_UNDO,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
          LG(KC_PGUP), LS(KC_PGDN), LC(KC_COPY), KC_UP,   KC_DOWN,  _______,            __XXX__, MO_NAVI, __XXX__, KC_RCTL, KC_RSFT, KC_RGUI,
                       KC_CUT,      KC_VOLD,     KC_HOME, KC_END,   _______,            _______, __XXX__, __XXX__, __XXX__, __XXX__,
                                    _______,     KC_LALT, _______,  TO_BASE,            __XXX__, _______, KC_LALT, _______
    ),
};
