/*
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "lock_status.h"
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/keymap.h>

#define LOCK_LAYER_INDEX 6

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static char saved_profile_text[32] = "";
static bool displayed = false;

struct lock_status_state {
    bool locked;
};

static struct lock_status_state get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_id_t lock_id = zmk_keymap_layer_index_to_id(LOCK_LAYER_INDEX);
    return (struct lock_status_state){
        .locked = zmk_keymap_layer_active(lock_id),
    };
}

static void lock_status_update_cb(struct lock_status_state state) {
    struct zmk_widget_lock_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        if (state.locked && !displayed) {
            if (widget->profile_target) {
                strncpy(saved_profile_text, lv_label_get_text(widget->profile_target),
                        sizeof(saved_profile_text) - 1);
                saved_profile_text[sizeof(saved_profile_text) - 1] = '\0';
                lv_label_set_text(widget->profile_target, "LOCKED");
            }
            if (widget->layer_target) {
                lv_label_set_text(widget->layer_target, "4 corners");
            }
            displayed = true;
        } else if (!state.locked && displayed) {
            if (widget->profile_target) {
                lv_label_set_text(widget->profile_target, saved_profile_text);
            }
            displayed = false;
        }
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_lock_status, struct lock_status_state,
                            lock_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_lock_status, zmk_layer_state_changed);
ZMK_SUBSCRIPTION(widget_lock_status, zmk_activity_state_changed);

int zmk_widget_lock_status_init(struct zmk_widget_lock_status *widget,
                                lv_obj_t *profile_label, lv_obj_t *layer_label) {
    widget->profile_target = profile_label;
    widget->layer_target = layer_label;

    sys_slist_append(&widgets, &widget->node);
    widget_lock_status_init();

    return 0;
}
