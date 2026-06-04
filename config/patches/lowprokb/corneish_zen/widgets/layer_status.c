/*
 *
 * Copyright (c) 2021 Darryl deHaan
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "layer_status.h"
#include <zmk/event_manager.h>

/*
 * Central (or non-split) side: use the keymap API to get the active layer.
 * Peripheral side: listen for relayed layer changes from the central.
 */
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

#elif IS_ENABLED(CONFIG_ZMK_SPLIT_PERIPHERAL_LAYER_STATE)

#include <zmk/events/split_peripheral_layer_changed.h>

#endif

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_status_state {
    uint8_t index;
    const char *label;
};

static void set_layer_symbol(lv_obj_t *label, struct layer_status_state state) {
    const char *layer_label = state.label;
    uint8_t active_layer_index = state.index;
    static uint8_t last_index = 255;

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_HIDE_MOMENTARY_LAYERS) &&                                       \
    (!IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL))
    if (!zmk_keymap_layer_momentary(active_layer_index) && last_index != active_layer_index) {
        last_index = active_layer_index;
        LOG_DBG("Last perm layer index updated to %i", active_layer_index);
    } else {
        return;
    }
#else
    if (active_layer_index == last_index) {
        return;
    }
    last_index = active_layer_index;
#endif

    if (layer_label == NULL || layer_label[0] == '\0') {
        char text[6] = {};

        snprintf(text, sizeof(text), " %i", active_layer_index);

        lv_label_set_text(label, text);
    } else {
        lv_label_set_text(label, layer_label);
    }
}

static void layer_status_update_cb(struct layer_status_state state) {
    struct zmk_widget_layer_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_symbol(widget->obj, state); }
}

#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
    return (struct layer_status_state){
        .index = index, .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index))};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

#elif IS_ENABLED(CONFIG_ZMK_SPLIT_PERIPHERAL_LAYER_STATE)

static const char *peripheral_layer_names[] = {"BASE",  "NAV",   "SYMB", "FUNC",
                                               "QUICK", "MOUSE", "LOCK", "VERIFY"};
#define PERIPHERAL_LAYER_NAME_COUNT (sizeof(peripheral_layer_names) / sizeof(peripheral_layer_names[0]))

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    const struct zmk_split_peripheral_layer_changed *ev = as_zmk_split_peripheral_layer_changed(eh);
    uint8_t index = (ev != NULL) ? ev->layer : 0;
    const char *label = (index < PERIPHERAL_LAYER_NAME_COUNT) ? peripheral_layer_names[index] : NULL;
    return (struct layer_status_state){.index = index, .label = label};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_split_peripheral_layer_changed);

#endif

int zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_set_width(widget->obj, 68);
    lv_label_set_long_mode(widget->obj, LV_LABEL_LONG_CLIP);

    sys_slist_append(&widgets, &widget->node);

    widget_layer_status_init();
    return 0;
}

lv_obj_t *zmk_widget_layer_status_obj(struct zmk_widget_layer_status *widget) {
    return widget->obj;
}
