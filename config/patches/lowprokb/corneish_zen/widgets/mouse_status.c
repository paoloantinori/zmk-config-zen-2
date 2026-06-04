/*
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include "mouse_status.h"
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/keymap.h>

#define MOUSE_LAYER_INDEX 5

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static char saved_profile_text[32] = "";
static bool displayed = false;

struct mouse_status_state {
    bool active;
};

static struct mouse_status_state get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_id_t id = zmk_keymap_layer_index_to_id(MOUSE_LAYER_INDEX);
    return (struct mouse_status_state){
        .active = zmk_keymap_layer_active(id),
    };
}

static void mouse_status_update_cb(struct mouse_status_state state) {
    if (state.active == displayed) {
        return;
    }
    struct zmk_widget_mouse_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        if (state.active) {
            if (widget->profile_target) {
                strncpy(saved_profile_text, lv_label_get_text(widget->profile_target),
                        sizeof(saved_profile_text) - 1);
                saved_profile_text[sizeof(saved_profile_text) - 1] = '\0';
            }
            if (widget->battery_obj) {
                lv_obj_add_flag(widget->battery_obj, LV_OBJ_FLAG_HIDDEN);
            }
            if (widget->output_obj) {
                lv_obj_add_flag(widget->output_obj, LV_OBJ_FLAG_HIDDEN);
            }
            if (widget->layer_target) {
                lv_obj_add_flag(widget->layer_target, LV_OBJ_FLAG_HIDDEN);
            }
            if (widget->profile_target) {
                lv_obj_align(widget->profile_target, LV_ALIGN_TOP_MID, 0, 2);
                lv_label_set_text_static(widget->profile_target,
                    "MOUSE\n"
                    "\n"
                    "JKL  move\n"
                    "YUOP scrl\n"
                    "M,.  btns\n"
                    "L R click");
            }
            displayed = true;
        } else {
            if (widget->battery_obj) {
                lv_obj_clear_flag(widget->battery_obj, LV_OBJ_FLAG_HIDDEN);
                lv_obj_invalidate(widget->battery_obj);
            }
            if (widget->output_obj) {
                lv_obj_clear_flag(widget->output_obj, LV_OBJ_FLAG_HIDDEN);
                lv_obj_invalidate(widget->output_obj);
            }
            if (widget->layer_target) {
                lv_obj_clear_flag(widget->layer_target, LV_OBJ_FLAG_HIDDEN);
                lv_obj_invalidate(widget->layer_target);
            }
            if (widget->profile_target) {
                lv_obj_align(widget->profile_target, LV_ALIGN_TOP_MID, 0, 78);
                lv_label_set_text(widget->profile_target, saved_profile_text);
            }
            displayed = false;
        }
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_mouse_status, struct mouse_status_state,
                            mouse_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_mouse_status, zmk_layer_state_changed);
ZMK_SUBSCRIPTION(widget_mouse_status, zmk_activity_state_changed);

int zmk_widget_mouse_status_init(struct zmk_widget_mouse_status *widget,
                                 lv_obj_t *battery, lv_obj_t *output,
                                 lv_obj_t *profile_label, lv_obj_t *layer_label) {
    widget->battery_obj = battery;
    widget->output_obj = output;
    widget->profile_target = profile_label;
    widget->layer_target = layer_label;

    sys_slist_append(&widgets, &widget->node);
    widget_mouse_status_init();

    return 0;
}
