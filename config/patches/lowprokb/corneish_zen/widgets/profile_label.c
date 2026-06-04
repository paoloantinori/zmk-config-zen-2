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
#include "profile_label.h"
#include "profile_labels.h"
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/ble.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct profile_label_state {
    int active_profile_index;
};

static struct profile_label_state get_state(const zmk_event_t *_eh) {
    return (struct profile_label_state){
        .active_profile_index = zmk_ble_active_profile_index(),
    };
}

static void profile_label_update_cb(struct profile_label_state state) {
    static int prev_index = -1;

    if (state.active_profile_index == prev_index) {
        return;
    }
    prev_index = state.active_profile_index;

    const char *label = profile_label_get(state.active_profile_index);
    const char *text = label ? label : "\xe2\x80\x94";

    struct zmk_widget_profile_label *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        lv_label_set_text(widget->obj, text);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_profile_label, struct profile_label_state,
                            profile_label_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_profile_label, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(widget_profile_label, zmk_activity_state_changed);

int zmk_widget_profile_label_init(struct zmk_widget_profile_label *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    const char *label = profile_label_get(zmk_ble_active_profile_index());
    lv_label_set_text(widget->obj, label ? label : "\xe2\x80\x94");

    sys_slist_append(&widgets, &widget->node);
    widget_profile_label_init();

    return 0;
}

lv_obj_t *zmk_widget_profile_label_obj(struct zmk_widget_profile_label *widget) {
    return widget->obj;
}
