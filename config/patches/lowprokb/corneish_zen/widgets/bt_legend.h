/*
 *
 * Copyright (c) 2021 Darryl deHaan
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <lvgl.h>

#include <zephyr/kernel.h>

struct zmk_widget_bt_legend {
    lv_obj_t *obj;
};

int zmk_widget_bt_legend_init(struct zmk_widget_bt_legend *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_bt_legend_obj(struct zmk_widget_bt_legend *widget);
