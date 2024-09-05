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

#define UMLAUT_T(kc) LT(_UMLAUT, kc)
#define NAV_T(kc) LT(_NAV, kc)
#define SYM_T(kc) LT(_SYM, kc)
#define NUM_T(kc) LT(_NUM, kc)
#define MATH_T(kc) LT(_MATH, kc)

enum custom_keycodes {
  SYM_GRV = SAFE_RANGE,
  SYM_EQL,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
     KC_NO, DE_V, DE_W, DE_M, DE_G, DE_K, DE_J, DE_COMM, DE_DOT, DE_QUOT, DE_B, KC_NO,
     DE_Q, LGUI_T(DE_S), LCTL_T(DE_C), LSFT_T(DE_N), SYM_T(DE_T), KC_NO, KC_NO, NAV_T(DE_A), RSFT_T(DE_E), RCTL_T(DE_I), RGUI_T(DE_H), DE_Z,
     KC_NO, DE_X, DE_P, DE_L, UMLAUT_T(DE_D), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, NUM_T(DE_U), DE_O, DE_Y, DE_F, KC_NO,
     KC_NO, KC_DEL, KC_BSPC, LALT_T(DE_R), KC_TAB, KC_ESC, LALT_T(KC_SPC), KC_ENT, KC_INS, KC_NO
    ),
    [_UMLAUT] = LAYOUT(
     KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO,
     KC_TRNS, DE_SS, KC_LCTL, KC_LSFT, KC_TRNS, KC_NO, KC_NO, DE_ADIA, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
     KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, MO(_UMLAUT), KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, DE_UDIA, DE_ODIA, KC_TRNS, KC_TRNS, KC_NO,
     KC_NO, KC_TRNS, KC_TRNS, KC_LALT, KC_TRNS, KC_TRNS, DE_MINS, KC_TRNS, KC_TRNS, KC_NO
    ),
    [_SYM] = LAYOUT(
     KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, DE_TILD, DE_EXLM, DE_DLR, SYM_EQL, SYM_GRV, KC_NO,
     KC_NO, KC_LGUI, KC_LCTL, KC_LSFT, MO(_SYM), KC_NO, KC_NO, DE_LPRN, DE_RPRN, DE_LCBR, DE_RCBR, DE_EURO,
     KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, DE_SLSH, DE_ASTR, DE_PLUS, DE_PERC, KC_NO,
     KC_NO, KC_TRNS, KC_TRNS, KC_LALT, KC_TRNS, KC_TRNS, KC_LALT, KC_TRNS, KC_TRNS, KC_NO
    ),
};

const key_override_t ko_base_comm = ko_make_with_layers(MOD_MASK_SHIFT, DE_COMM, DE_SCLN, (1 << _BASE));
const key_override_t ko_base_dot = ko_make_with_layers(MOD_MASK_SHIFT, DE_DOT, DE_COLN, (1 << _BASE));
const key_override_t ko_base_quot = ko_make_with_layers(MOD_MASK_SHIFT, DE_QUOT, DE_DQUO, (1 << _BASE));

const key_override_t ko_sym_tild = ko_make_with_layers(MOD_MASK_SHIFT, DE_TILD, KC_NO, (1 << _SYM));
const key_override_t ko_sym_excl = ko_make_with_layers(MOD_MASK_SHIFT, DE_EXLM, DE_QUES, (1 << _SYM));
const key_override_t ko_sym_dlr = ko_make_with_layers(MOD_MASK_SHIFT, DE_DLR, DE_HASH, (1 << _SYM));
const key_override_t ko_sym_lprn = ko_make_with_layers(MOD_MASK_SHIFT, DE_LPRN, DE_LBRC, (1 << _SYM));
const key_override_t ko_sym_rprn = ko_make_with_layers(MOD_MASK_SHIFT, DE_RPRN, DE_RBRC, (1 << _SYM));
const key_override_t ko_sym_lcbr = ko_make_with_layers(MOD_MASK_SHIFT, DE_LCBR, DE_LABK, (1 << _SYM));
const key_override_t ko_sym_rcbr = ko_make_with_layers(MOD_MASK_SHIFT, DE_RCBR, DE_RABK, (1 << _SYM));
const key_override_t ko_sym_euro = ko_make_with_layers(MOD_MASK_SHIFT, DE_EURO, DE_AT, (1 << _SYM));
const key_override_t ko_sym_slsh = ko_make_with_layers(MOD_MASK_SHIFT, DE_SLSH, DE_BSLS, (1 << _SYM));
const key_override_t ko_sym_astr = ko_make_with_layers(MOD_MASK_SHIFT, DE_ASTR, DE_PIPE, (1 << _SYM));
const key_override_t ko_sym_plus = ko_make_with_layers(MOD_MASK_SHIFT, DE_PLUS, DE_AMPR, (1 << _SYM));
const key_override_t ko_sym_perc = ko_make_with_layers(MOD_MASK_SHIFT, DE_PERC, KC_NO, (1 << _SYM));
const key_override_t ko_sym_mins = ko_make_with_layers(MOD_MASK_SHIFT, DE_MINS, DE_UNDS, (1 << _SYM));

const key_override_t *key_overrides[] = {
	&ko_base_comm,
    &ko_base_dot,
    &ko_base_quot,
    &ko_sym_tild,
    &ko_sym_excl,
    &ko_sym_dlr,
    &ko_sym_lprn,
    &ko_sym_rprn,
    &ko_sym_lcbr,
    &ko_sym_rcbr,
    &ko_sym_euro,
    &ko_sym_slsh,
    &ko_sym_astr,
    &ko_sym_plus,
    &ko_sym_perc,
    &ko_sym_mins,
};

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    const uint8_t mods = get_mods();

    switch (keycode) {
        case SYM_EQL: {
            if (record->event.pressed) {
                if(mods & MOD_MASK_SHIFT) {
                    // DE_ACUT
                    tap_code(KC_EQL);
                    del_mods(MOD_MASK_SHIFT);
                    tap_code(KC_SPC);
                } else {
                    // DE_EQL
                    add_mods(MOD_MASK_SHIFT);
                    tap_code(KC_0);
                }
            }
            return false;
        }
        case SYM_GRV: {
            if (record->event.pressed) {
                if(mods & MOD_MASK_SHIFT) {
                    // DE_CIRC
                    tap_code(KC_GRV);
                } else {
                    // DE_GRV
                    del_mods(MOD_MASK_SHIFT);
                    tap_code(KC_GRV);
                    tap_code(KC_SPC);
                }
            }
            return false;
        }
    }

    set_mods(mods);

    return true;
}
