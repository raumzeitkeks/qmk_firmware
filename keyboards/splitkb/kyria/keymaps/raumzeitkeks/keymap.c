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
    _UML,
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
    BK_DQUO = KC_INTERNATIONAL_9,
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
        case BK_DQUO: {
            remap_keycode = DE_DQUO;
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
#define MO_UML  MO(_UML)
#define MO_NAV  MO(_NAV)
#define MO_NUM  MO(_NUM)
#define MO_FN   MO(_FN)
#define MO_VOL  MO(_VOL)

#define SYM1(kc) LT(_SYM1, kc)
#define SYM2(kc) LT(_SYM2, kc)
#define UML(kc)  LT(_UML, kc)
#define NAV(kc)  LT(_NAV, kc)
#define NUM(kc)  LT(_NUM, kc)
#define FN(kc)   LT(_FN, kc)
#define VOL(kc)  LT(_VOL, kc)

#define LG(kc) LGUI_T(kc)
#define LC(kc) LCTL_T(kc)
#define LS(kc) LSFT_T(kc)
#define LA(kc) LALT_T(kc)
#define RG(kc) RGUI_T(kc)
#define RC(kc) RCTL_T(kc)
#define RS(kc) RSFT_T(kc)

#define LA_COPY LA(KC_COPY)
#define LG_CUT  LA(KC_CUT)

#define __XXX__ KC_NO

#define TTI TAPPING_TERM_INC
#define TTD TAPPING_TERM_DEC
#define TTO TAPPING_TERM_OUT

#define DENSE_LAYOUT(L30, L31, L32, L33, L34, L35, R35, R34, R33, R32, R31, R30, L20, L21, L22, L23, L24, L25, R25, R24, R23, R22, R21, R20, L11, L12, L13, L14, L15, R15, R14, R13, R12, R11, L00, L01, L02, L03, L04, R04, R03, R02, R01, R00) \
    LAYOUT( L30, L31, L32, L33, L34, L35,                     R35, R34, R33, R32, R31, R30, \
            L20, L21, L22, L23, L24, L25,                     R25, R24, R23, R22, R21, R20, \
            TTO, L11, L12, L13, L14, L15, TTD, TTI, TTI, TTD, R15, R14, R13, R12, R11, TTO, \
                           L00, L01, L02, L03, L04, R04, R03, R02, R01, R00 )

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = DENSE_LAYOUT(
            KC_ESC,  KC_BSPC,  DE_W,      DE_M,       DE_G,       DE_J,                 DE_Q,        DE_D,      DE_COMM,    DE_QUES,   KC_BSPC,  KC_ESC,
            CW_TOGG, LS(DE_S), LC(DE_N),  SYM1(DE_R), SYM2(DE_T), UML(DE_P),            FN(BK_DQUO), NUM(DE_H), NAV(DE_E),  RC(DE_I),  RS(DE_A), CW_TOGG,
                     KC_DEL,   VOL(DE_F), DE_L,       DE_K,       DE_Z,                 DE_Y,        DE_B,      DE_O,       VOL(DE_U), KC_DEL,
                     MO_FN,    MO_NUM,    LA(DE_C),   DE_V,       LG(DE_X),             KC_RGUI,     KC_ENT,    LA(KC_SPC), MO_SYM1,   MO_SYM2
    ),
    [_SYM1] = DENSE_LAYOUT(
                       KC_ESC,  KC_BSPC, __XXX__, __XXX__, KC_HOME, __XXX__,            DE_AT,   DE_SLSH, DE_DOT,  DE_TILD, KC_BSPC, KC_ESC,
                       __XXX__, KC_LSFT, KC_LCTL, MO_SYM1, KC_LEFT, KC_RGHT,            DE_QUOT, DE_LPRN, DE_LBRC, DE_LABK, DE_ASTR, __XXX__,
                                KC_DEL,  __XXX__, __XXX__, KC_END,  KC_UNDO,            DE_BSLS, DE_LCBR, DE_DLR,  DE_PERC, KC_DEL,
                                __XXX__, __XXX__, LA_COPY, KC_PSTE, LG_CUT,             __XXX__, KC_ENT,  DE_UNDS, MO_SYM1, __XXX__
    ),
    [_SYM2] = DENSE_LAYOUT(
                      KC_ESC,  KC_BSPC, __XXX__, KC_TAB,  __XXX__,  __XXX__,            DE_SECT, DE_HASH, DE_EQL,  DE_EXLM, KC_BSPC, KC_ESC,
                      __XXX__, KC_LSFT, KC_LCTL, KC_UP,   MO_SYM2,  __XXX__,            DE_GRV,  DE_RPRN, DE_RBRC, DE_RABK, DE_PLUS, __XXX__,
                               KC_DEL,  __XXX__, KC_DOWN, __XXX__,  KC_UNDO,            DE_CIRC, DE_RCBR, DE_PIPE, DE_AMPR, KC_DEL,
                               __XXX__, __XXX__, LA_COPY, KC_PSTE,  LG_CUT,             __XXX__, KC_ENT,  DE_MINS, __XXX__, MO_SYM2
    ),
    [_UML] = DENSE_LAYOUT(
                       KC_ESC,  KC_BSPC, __XXX__, __XXX__, __XXX__, __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, KC_BSPC, KC_ESC,
                       __XXX__, KC_LSFT, KC_LCTL, KC_PGUP, __XXX__, MO_UML,             __XXX__, __XXX__, __XXX__, __XXX__, DE_ADIA, __XXX__,
                                KC_DEL,  __XXX__, KC_PGDN, __XXX__, KC_UNDO,            __XXX__, DE_SS,   DE_ODIA, DE_UDIA, KC_DEL,
                                __XXX__, __XXX__, LA_COPY, KC_PSTE, LG_CUT,             __XXX__, KC_ENT,  __XXX__, __XXX__, __XXX__
    ),
    [_NAV] = DENSE_LAYOUT(
                      KC_ESC,  KC_BSPC, __XXX__, KC_TAB,  KC_HOME,  __XXX__,            __XXX__, KC_HOME, __XXX__, __XXX__, KC_BSPC, KC_ESC,
                      __XXX__, KC_LSFT, KC_LCTL, KC_UP,   KC_LEFT,  KC_RGHT,            KC_LEFT, KC_RGHT, MO_NAV,  KC_RCTL, KC_RSFT, __XXX__,
                               KC_DEL,  __XXX__, KC_DOWN, KC_END,   KC_UNDO,            __XXX__, KC_END,  __XXX__, __XXX__, KC_DEL,
                               __XXX__, __XXX__, LA_COPY, KC_PSTE,  LG_CUT,             KC_RGUI, KC_ENT,  KC_LALT, __XXX__, __XXX__
    ),
    [_NUM] = DENSE_LAYOUT(
                           KC_ESC,  KC_BSPC, DE_7,   DE_8, DE_9,    DE_PERC,            __XXX__, __XXX__, KC_TAB,      __XXX__, KC_BSPC, KC_ESC,
                           __XXX__, DE_DOT,  DE_1,   DE_2, DE_3,    DE_0,               __XXX__, MO_NUM,  KC_UP,       KC_RCTL, KC_RSFT, __XXX__,
                                    KC_DEL,  DE_4,   DE_5, DE_6,    DE_EURO,            __XXX__, __XXX__, KC_DOWN,     __XXX__, KC_DEL,
                                    __XXX__, MO_NUM, DE_0, __XXX__, __XXX__,            KC_RGUI, KC_ENT,  LA(KC_MINS), __XXX__, __XXX__
    ),
    [_FN] = DENSE_LAYOUT(
                          KC_ESC,  KC_BSPC, KC_F7,   KC_F8,  KC_F9,  KC_F12,            __XXX__, __XXX__, __XXX__, __XXX__, KC_BSPC, KC_ESC,
                          __XXX__, __XXX__, KC_F1,   KC_F2,  KC_F3,  KC_F10,            MO_FN,   __XXX__, KC_PGUP, KC_RCTL, KC_RSFT, KC_INS,
                                   KC_DEL,  KC_F4,   KC_F5,  KC_F6,  KC_F11,            __XXX__, __XXX__, KC_PGDN, __XXX__, KC_DEL,
                                   MO_FN,   __XXX__, KC_F10, KC_F11, KC_F12,            KC_RGUI, KC_ENT,  KC_LALT, __XXX__, __XXX__
    ),
    [_VOL] = DENSE_LAYOUT(
                       __XXX__, __XXX__, __XXX__, KC_MUTE, __XXX__, __XXX__,            __XXX__, __XXX__, KC_MUTE, __XXX__, __XXX__, __XXX__,
                       __XXX__, __XXX__, __XXX__, KC_VOLU, __XXX__, __XXX__,            __XXX__, __XXX__, KC_VOLU, __XXX__, __XXX__, __XXX__,
                                __XXX__, MO_VOL,  KC_VOLD, __XXX__, __XXX__,            __XXX__, __XXX__, KC_VOLD, MO_VOL,  __XXX__,
                                __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__
    ),
};

const uint16_t PROGMEM combo_umlaut_ae[] = {RS(DE_A),  NAV(DE_E), COMBO_END};
const uint16_t PROGMEM combo_umlaut_oe[] = {DE_O,      NAV(DE_E), COMBO_END};
const uint16_t PROGMEM combo_umlaut_ue[] = {VOL(DE_U), NAV(DE_E), COMBO_END};
const uint16_t PROGMEM combo_umlaut_sz[] = {LS(DE_S),  DE_Z,      COMBO_END};

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
