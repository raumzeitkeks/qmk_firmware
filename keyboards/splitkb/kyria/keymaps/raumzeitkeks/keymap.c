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
    _SYM1,
    _SYM2,
    _NAV,
    _NUM,
    _FN,
    _VOL,
};

enum custom_keycodes {
    TAPPING_TERM_INC = SAFE_RANGE,
    TAPPING_TERM_DEC,
    TAPPING_TERM_OUT,
    COMBO_TERM_INC,
    COMBO_TERM_DEC,
    COMBO_TERM_OUT,

    // Basic keycodes for LT()
    BK_QUO = KC_INTERNATIONAL_9,
};

uint16_t g_tapping_term = TAPPING_TERM;
uint16_t g_combo_term = COMBO_TERM;

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    return g_tapping_term;
}

uint16_t get_combo_term(uint16_t index, combo_t *combo) {
    return g_combo_term;
}

void send_term(uint16_t term, uint8_t id1, uint8_t id2) {
    tap_code(id1);
    tap_code(id2);
    const char* str = get_u16_str(term, ' ');
    for (; *str != '\0'; ++str) {
        if ('0' <= *str && *str <= '9') {
            const uint8_t lut[] = { DE_0, DE_1, DE_2, DE_3, DE_4, DE_5, DE_6, DE_7, DE_8, DE_9 };
            tap_code(lut[*str - '0']);
        }
    }
}

bool process_term(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case TAPPING_TERM_INC: {
                g_tapping_term += 10;
                return false;
            }
            case TAPPING_TERM_DEC: {
                g_tapping_term -= 10;
                return false;
            }
            case TAPPING_TERM_OUT: {
                send_term(g_tapping_term, DE_T, DE_T);
                return false;
            }
            case COMBO_TERM_INC: {
                g_combo_term += 10;
                return false;
            }
            case COMBO_TERM_DEC: {
                g_combo_term -= 10;
                return false;
            }
            case COMBO_TERM_OUT: {
                send_term(g_combo_term, DE_C, DE_T);
                return false;
            }
        }
    }
    return true;
}

bool process_remap(uint16_t keycode, keyrecord_t *record) {
    const uint8_t mods = get_mods();
    const uint8_t weak_mods = get_mods();
    const uint8_t oneshot_mods = get_mods();
    const bool is_shift_pressed = ((mods | weak_mods | oneshot_mods) & MOD_MASK_SHIFT) != 0;

    uint16_t remap_keycode = KC_NO;
    switch (QK_MODS_GET_BASIC_KEYCODE(keycode)) {
        case KC_CUT: {
            remap_keycode = C(DE_X);
            break;
        }
        case KC_COPY: {
            remap_keycode = C(DE_C);
            break;
        }
        case KC_PSTE: {
            remap_keycode = C(DE_V);
            break;
        }
        case KC_UNDO: {
            remap_keycode = is_shift_pressed ? C(DE_Y) : C(DE_Z);
            break;
        }
        case BK_QUO: {
            remap_keycode = is_shift_pressed ? DE_QUOT : DE_DQUO;
            break;
        }
    }

    // Continue default handling if this is not a remapped key.
    if (remap_keycode == KC_NO) {
        return true;
    }

    if (record->event.pressed) {
        clear_oneshot_mods();
        clear_weak_mods();
        clear_mods();

        tap_code16(remap_keycode);

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
        process_term(keycode, record) &&
        process_remap(keycode, record)
    );
}

bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case DE_A ... DE_Z:
        case DE_ADIA:
        case DE_ODIA:
        case DE_UDIA:
        case DE_SS: {
            add_weak_mods(MOD_BIT_LSHIFT);
            return true;
        }

        case KC_1 ... KC_0:
        case DE_MINS:
        case DE_UNDS:
        case KC_DEL:
        case KC_BSPC:
        case KC_LEFT:
        case KC_RIGHT:
        case LCTL(KC_LEFT):
        case LCTL(KC_RIGHT):
        case RCTL(KC_LEFT):
        case RCTL(KC_RIGHT):
        case LSFT(KC_LEFT):
        case LSFT(KC_RIGHT):
        case RSFT(KC_LEFT):
        case RSFT(KC_RIGHT):
        case LSFT(LCTL(KC_LEFT)):
        case LSFT(LCTL(KC_RIGHT)):
        case RSFT(RCTL(KC_LEFT)):
        case RSFT(RCTL(KC_RIGHT)): {
            return true;
        }
    }
    return false;
}

#define MO_SYM1 MO(_SYM1)
#define MO_SYM2 MO(_SYM2)
#define MO_NAV  MO(_NAV)
#define MO_NUM  MO(_NUM)
#define MO_FN   MO(_FN)

#define SYM1(kc) LT(_SYM1, kc)
#define SYM2(kc) LT(_SYM2, kc)
#define NAV(kc)  LT(_NAV, kc)
#define NUM(kc)  LT(_NUM, kc)
#define FN(kc)   LT(_FN, kc)

#define LG(kc) LGUI_T(kc)
#define LC(kc) LCTL_T(kc)
#define LS(kc) LSFT_T(kc)
#define LA(kc) LALT_T(kc)
#define RG(kc) RGUI_T(kc)
#define RC(kc) RCTL_T(kc)
#define RS(kc) RSFT_T(kc)

#define LA_CUT  LA(KC_CUT)
#define LG_PSTE LG(KC_PSTE)
#define LA_SPC  LA(KC_SPC)

#define XXX KC_NO
#define __XXX__ KC_NO

#define TTI TAPPING_TERM_INC
#define TTD TAPPING_TERM_DEC
#define TTO TAPPING_TERM_OUT

#define DENSE_LAYOUT(L32, L33, L34, L35, R35, R34, R33, R32, L20, L21, L22, L23, L24, L25, R25, R24, R23, R22, R21, R20, L11, L12, L13, L14, L15, R15, R14, R13, R12, R11, L00, L01, L03, L04, R04, R03, R01, R00) \
    LAYOUT( XXX, XXX, L32, L33, L34, L35,                     R35, R34, R33, R32, XXX, XXX, \
            L20, L21, L22, L23, L24, L25,                     R25, R24, R23, R22, R21, R20, \
            XXX, L11, L12, L13, L14, L15, XXX, XXX, TTI, TTD, R15, R14, R13, R12, R11, TTO, \
                           L00, L01, XXX, L03, L04, R04, R03, XXX, R01, R00 )

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = DENSE_LAYOUT(
                             DE_J,     DE_L,       DE_F,       DE_D,                KC_BSPC,    DE_DOT,    DE_MINS,   DE_Q,
             DE_K, LS(DE_S), LC(DE_R), SYM1(DE_N), SYM2(DE_T), DE_B,                FN(BK_QUO), NUM(DE_H), NAV(DE_E), RC(DE_I),  RS(DE_A), DE_Y,
                   DE_X,     DE_W,     DE_M,       DE_G,       DE_P,                KC_DEL,     DE_COMM,   DE_O,      DE_U,      DE_Z,
                             MO_FN,    MO_NUM,     LA(DE_C),   LG(DE_V),            KC_ENT,     KC_SPC,    MO_SYM1,   MO_SYM2
    ),
    [_SYM1] = DENSE_LAYOUT(
                                     __XXX__, __XXX__, KC_HOME, KC_PGUP,            KC_BSPC, DE_SLSH, DE_TILD, DE_QUES,
                    KC_ESC, KC_LSFT, KC_LCTL, MO_SYM1, KC_LEFT, KC_RGHT,            DE_GRV,  DE_LPRN, DE_LBRC, DE_LABK, DE_ASTR, DE_BSLS,
                            KC_DEL,  __XXX__, __XXX__, KC_END,  KC_PGDN,            KC_DEL,  DE_LCBR, DE_DLR,  DE_PERC, DE_EURO,
                                     __XXX__, __XXX__, LA_CUT,  LG_PSTE,            KC_ENT,  KC_SPC,  MO_SYM1, __XXX__
    ),
    [_SYM2] = DENSE_LAYOUT(
                                    __XXX__, KC_TAB,  __XXX__,  __XXX__,            KC_BSPC, DE_HASH, DE_EQL,  DE_EXLM,
                   KC_ESC, KC_LSFT, KC_LCTL, KC_UP,   MO_SYM2,  __XXX__,            DE_CIRC, DE_RPRN, DE_RBRC, DE_RABK, DE_PLUS, DE_AT,
                           KC_DEL,  __XXX__, KC_DOWN, __XXX__,  __XXX__,            KC_DEL,  DE_RCBR, DE_PIPE, DE_AMPR, DE_SECT,
                                    __XXX__, __XXX__, LA_CUT,  LG_PSTE,             KC_ENT,  KC_SPC,  __XXX__, MO_SYM2
    ),
    [_NAV] = DENSE_LAYOUT(
                                  S(KC_UNDO), KC_TAB,  KC_HOME, KC_PGUP,           KC_BSPC, __XXX__, __XXX__, __XXX__,
                 KC_ESC, KC_LSFT, KC_LCTL,    KC_UP,   KC_LEFT, KC_RGHT,           __XXX__, KC_RGHT, MO_NAV,  KC_RCTL, KC_RSFT, __XXX__,
                         KC_DEL,  KC_UNDO,    KC_DOWN, KC_END,  KC_PGDN,           KC_DEL,  __XXX__, __XXX__, __XXX__, KC_INS,
                                  __XXX__,    __XXX__, LA_CUT,  LG_PSTE,           KC_ENT,  LA_SPC,  __XXX__, __XXX__
    ),
    [_NUM] = DENSE_LAYOUT(
                                         __XXX__, DE_7,   DE_8, DE_9,               __XXX__, __XXX__, KC_MUTE, __XXX__,
                       __XXX__, KC_LSFT, KC_LCTL, DE_1,   DE_2, DE_3,               __XXX__, MO_NUM,  KC_VOLU, KC_RCTL, KC_RSFT, __XXX__,
                                DE_COMM, DE_DOT,  DE_4,   DE_5, DE_6,               __XXX__, __XXX__, KC_VOLD, __XXX__, __XXX__,
                                         __XXX__, MO_NUM, DE_0, DE_MINS,            KC_ENT,  LA_SPC,  __XXX__, __XXX__
    ),
    [_FN] = DENSE_LAYOUT(
                                      KC_F12, KC_F7,   KC_F8,   KC_F9,              __XXX__, __XXX__, __XXX__, __XXX__,
                    __XXX__, __XXX__, KC_F10, KC_F1,   KC_F2,   KC_F3,              MO_FN,   __XXX__, __XXX__, KC_RCTL, KC_RSFT, __XXX__,
                             __XXX__, KC_F11, KC_F4,   KC_F5,   KC_F6,              __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
                                      MO_FN,  __XXX__, __XXX__, KC_LGUI,            KC_ENT,  KC_LALT, __XXX__, __XXX__
    ),
};

const uint16_t PROGMEM combo_umlaut_ae[] = {RS(DE_A), NAV(DE_E), COMBO_END};
const uint16_t PROGMEM combo_umlaut_oe[] = {DE_O,     NAV(DE_E), COMBO_END};
const uint16_t PROGMEM combo_umlaut_ue[] = {DE_U,     NAV(DE_E), COMBO_END};
const uint16_t PROGMEM combo_umlaut_sz[] = {LS(DE_S), DE_Z,      COMBO_END};

combo_t key_combos[] = {
    COMBO(combo_umlaut_ae, DE_ADIA),
    COMBO(combo_umlaut_oe, DE_ODIA),
    COMBO(combo_umlaut_ue, DE_UDIA),
    COMBO(combo_umlaut_sz, DE_SS),
};

bool get_combo_must_tap(uint16_t index, combo_t *combo) {
    return true;
}

bool get_combo_must_press_in_order(uint16_t index, combo_t *combo) {
    return true;
}
