/*
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "verify_status.h"
#include "verify_labels.h"
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/hid.h>
#include <dt-bindings/zmk/modifiers.h>

#define VERIFY_LAYER_INDEX 7

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static char saved_profile_text[32] = "";
static bool text_saved = false;

struct verify_status_state {
    bool active;
    bool key_pressed;
    uint32_t position;
    uint8_t layer_index;
    zmk_mod_flags_t mods;
};

static uint8_t find_layer_below_verify(void) {
    zmk_keymap_layers_state_t active = zmk_keymap_layer_state();
    active &= (1UL << VERIFY_LAYER_INDEX) - 1;
    return active ? (31 - __builtin_clz(active)) : 0;
}

static struct verify_status_state get_state(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *pos_ev = as_zmk_position_state_changed(eh);

    zmk_keymap_layer_id_t verify_id = zmk_keymap_layer_index_to_id(VERIFY_LAYER_INDEX);
    bool active = zmk_keymap_layer_active(verify_id);
    bool key_pressed = false;
    uint32_t position = 0;
    uint8_t layer_index = 0;
    zmk_mod_flags_t mods = 0;

    if (active && pos_ev != NULL && pos_ev->state) {
        key_pressed = true;
        position = pos_ev->position;
        layer_index = find_layer_below_verify();
        mods = zmk_hid_get_explicit_mods();
    }

    return (struct verify_status_state){
        .active = active,
        .key_pressed = key_pressed,
        .position = position,
        .layer_index = layer_index,
        .mods = mods,
    };
}

static void build_layer_display(char *buf, size_t size, const char *layer_name,
                                zmk_mod_flags_t mods) {
    size_t len = 0;
    while (*layer_name && len < size - 1) {
        buf[len++] = *layer_name++;
    }
    const char *mod_names[] = {NULL, NULL, NULL};
    int mod_count = 0;
    if (mods & (MOD_LSFT | MOD_RSFT)) mod_names[mod_count++] = "SHIFT";
    if (mods & (MOD_LCTL | MOD_RCTL)) mod_names[mod_count++] = "CTRL";
    if (mods & (MOD_LALT | MOD_RALT)) mod_names[mod_count++] = "ALT";
    for (int i = 0; i < mod_count && len < size - 1; i++) {
        buf[len++] = '+';
        const char *m = mod_names[i];
        while (*m && len < size - 1) {
            buf[len++] = *m++;
        }
    }
    buf[len] = '\0';
}

static void verify_status_update_cb(struct verify_status_state state) {
    struct zmk_widget_verify_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        if (!state.active) {
            if (text_saved && widget->layer_target) {
                lv_label_set_text(widget->layer_target, saved_profile_text);
                text_saved = false;
            }
            return;
        }

        if (!state.key_pressed) {
            return;
        }

        if (!text_saved && widget->layer_target) {
            strncpy(saved_profile_text, lv_label_get_text(widget->layer_target),
                    sizeof(saved_profile_text) - 1);
            saved_profile_text[sizeof(saved_profile_text) - 1] = '\0';
            text_saved = true;
        }

        zmk_keymap_layer_id_t layer_id =
            zmk_keymap_layer_index_to_id(state.layer_index);
        const char *layer_name = zmk_keymap_layer_name(layer_id);
        if (layer_name == NULL || strlen(layer_name) == 0) {
            layer_name = "?";
        }

        char display_name[32];
        build_layer_display(display_name, sizeof(display_name), layer_name, state.mods);

        const char *key_text = "?";
        if (state.layer_index < VERIFY_LAYERS && state.position < VERIFY_KEYS) {
            const char *label = NULL;
            if (state.mods & (MOD_LSFT | MOD_RSFT)) {
                label = get_shifted_label(state.layer_index, state.position);
            }
            if (label == NULL) {
                label = key_labels[state.layer_index][state.position];
            }
            if (label != NULL) {
                key_text = label;
            }
        }

        if (widget->layer_target) {
            lv_label_set_text(widget->layer_target, display_name);
        }
        if (widget->key_target) {
            lv_label_set_text(widget->key_target, key_text);
        }
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_verify_status, struct verify_status_state,
                            verify_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_verify_status, zmk_position_state_changed);
ZMK_SUBSCRIPTION(widget_verify_status, zmk_layer_state_changed);

int zmk_widget_verify_status_init(struct zmk_widget_verify_status *widget,
                                  lv_obj_t *layer_label, lv_obj_t *key_label) {
    widget->layer_target = layer_label;
    widget->key_target = key_label;

    lv_obj_set_width(layer_label, 80);
    lv_label_set_long_mode(layer_label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(layer_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_width(key_label, 80);
    lv_label_set_long_mode(key_label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(key_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    sys_slist_append(&widgets, &widget->node);
    widget_verify_status_init();

    return 0;
}
