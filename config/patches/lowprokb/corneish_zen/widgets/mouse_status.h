/*
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_mouse_status {
    sys_snode_t node;
    lv_obj_t *battery_obj;
    lv_obj_t *output_obj;
    lv_obj_t *profile_target;
    lv_obj_t *layer_target;
};

int zmk_widget_mouse_status_init(struct zmk_widget_mouse_status *widget,
                                 lv_obj_t *battery, lv_obj_t *output,
                                 lv_obj_t *profile_label, lv_obj_t *layer_label);
