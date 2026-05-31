/*
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_lock_status {
    sys_snode_t node;
    lv_obj_t *profile_target;
    lv_obj_t *layer_target;
};

int zmk_widget_lock_status_init(struct zmk_widget_lock_status *widget,
                                lv_obj_t *profile_label, lv_obj_t *layer_label);
