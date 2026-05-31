/*
 * SPDX-License-Identifier: MIT
 *
 * Static key label lookup table for verify mode.
 * Maps [layer][position] to a short display string.
 * Keep in sync with corneish_zen.keymap.
 *
 * Key positions (36 keys):
 *   0   1   2   3   4  |  5   6   7   8   9
 *  10  11  12  13  14  | 15  16  17  18  19
 *  20  21  22  23  24  | 25  26  27  28  29
 *          30  31  32  | 33  34  35
 */

#pragma once

#define VERIFY_LAYERS 7
#define VERIFY_KEYS 36

static const char *const key_labels[VERIFY_LAYERS][VERIFY_KEYS] = {
    [0] = { /* BASE */
        "Q",     "W",     "E",     "R",     "T",
        "Y",     "U",     "I",     "O",     "P",
        "A",     "aS",    "sD",    "cF",    "G",
        "cH",    "sJ",    "aK",    "L",     "RET",
        "cZ",    "X",     "C",     "V",     "B",
        "N",     "M",     ",",     ".",      "/",
                 "aQCK",  "nSPC",  "---",
                 "moNAV", "sSPC",  "moFN",
    },
    [1] = { /* NAV */
        "ESC",   "ScL",   "mUP",   "ScR",   "ScU",
        "HOME",  "PgUp",  "UP",    "PgDn",  "BSPC",
        "SHFT",  "mLT",   "mDN",   "mRT",   "ScD",
        "END",   "LEFT",  "DOWN",  "RGHT",  "---",
        "`|",    "CTRL",  "LCLK",  "RCLK",  "MCLK",
        "INS",   "cTAB",  "csTA",  "PSCR",  "@'",
                 "---",   "---",   "---",
                 "---",   "LCLK",  "RCLK",
    },
    [2] = { /* SYMB */
        "1",     "2",     "3",     "4",     "5",
        "6",     "7",     "8",     "9",     "0",
        "<",     "a(",    "s{",    "c[",    "-",
        "c=",    "s~",    "a#",    "'",     "\\",
        ">",     ")",     "}",     "]",     "_",
        "+",     "@",     ";",     ":",     "|",
                 "---",   "&",     "---",
                 "---",   "*",     "---",
    },
    [3] = { /* FUNC */
        "F1",    "F2",    "F3",    "F4",    "F5",
        "F6",    "F7",    "F8",    "F9",    "F10",
        "MUTE",  "VOL-",  "VOL+",  "BRI-",  "BRI+",
        "cSPC",  "sSPC",  "aSPC",  "---",   "F11",
        "BT0",   "BT1",   "BT2",   "BT3",   "BT4",
        "BOOT",  "CLR",   "PRV",   "NXT",   "F12",
                 "---",   "DISC",  "---",
                 "---",   "OTOG",  "---",
    },
    [4] = { /* QUICK */
        "`",     "TAB",   "...",   "...",   "...",
        "...",   "...",   "...",   "...",   "...",
        "TAB",   "...",   "...",   "...",   "...",
        "...",   "...",   "...",   "...",   "...",
        "...",   "...",   "...",   "...",   "...",
        "...",   "...",   "...",   "...",   "...",
                 "ALT",   "...",   "---",
                 "---",   "...",   "---",
    },
    [5] = { /* MOUSE */
        "---",   "---",   "---",   "---",   "---",
        "ScL",   "ScD",   "mUP",   "ScU",   "ScR",
        "CTRL",  "GUI",   "ALT",   "SHFT",  "---",
        "---",   "mLT",   "mDN",   "mRT",   "---",
        "---",   "---",   "---",   "---",   "---",
        "---",   "MB4",   "MCLK",  "MB5",   "---",
                 "---",   "---",   "---",
                 "---",   "LCLK",  "RCLK",
    },
    [6] = { /* LOCK */
        "---",   "---",   "---",   "---",   "---",
        "---",   "---",   "---",   "---",   "---",
        "---",   "---",   "---",   "---",   "---",
        "---",   "---",   "---",   "---",   "---",
        "---",   "---",   "---",   "---",   "---",
        "---",   "---",   "---",   "---",   "---",
                 "SOFF",  "---",   "---",
                 "---",   "---",   "---",
    },
};

static const char *const key_labels_shifted_base[VERIFY_KEYS] = {
    "Q",     "W",     "E",     "R",     "T",
    "Y",     "U",     "I",     "O",     "P",
    "A",     NULL,    NULL,    NULL,    "G",
    NULL,    NULL,    NULL,    "L",     "RET",
    "Z",     "X",     "C",     "V",     "B",
    "N",     "M",     "<",     ">",     "?",
             NULL,    NULL,    NULL,
             NULL,    NULL,    NULL,
};

static const char *const key_labels_shifted_symb[VERIFY_KEYS] = {
    "!",     "\"",    "#",     "$",     "%",
    "^",     "&",     "*",     "(",     ")",
    NULL,    NULL,    NULL,    NULL,    NULL,
    NULL,    NULL,    NULL,    NULL,    NULL,
    NULL,    NULL,    NULL,    NULL,    NULL,
    NULL,    NULL,    NULL,    NULL,    NULL,
             NULL,    NULL,    NULL,
             NULL,    NULL,    NULL,
};

static inline const char *get_shifted_label(uint8_t layer, uint32_t position) {
    if (position >= VERIFY_KEYS) return NULL;
    switch (layer) {
    case 0: return key_labels_shifted_base[position];
    case 2: return key_labels_shifted_symb[position];
    default: return NULL;
    }
}
