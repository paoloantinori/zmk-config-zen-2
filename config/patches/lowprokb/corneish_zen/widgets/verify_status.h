/*
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_verify_status {
    sys_snode_t node;
    lv_obj_t *layer_target;
    lv_obj_t *key_target;
};

int zmk_widget_verify_status_init(struct zmk_widget_verify_status *widget,
                                  lv_obj_t *layer_label, lv_obj_t *key_label);
