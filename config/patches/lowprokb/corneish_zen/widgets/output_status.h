/*
 *
 * Copyright (c) 2021 Darryl deHaan
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

#define OUTPUT_CANVAS_W  80
#define OUTPUT_CANVAS_H  35
#define OUTPUT_CANVAS_CF LV_COLOR_FORMAT_L8
#define OUTPUT_CANVAS_BUF_SIZE                                                                     \
    LV_CANVAS_BUF_SIZE(OUTPUT_CANVAS_W, OUTPUT_CANVAS_H,                                          \
                       LV_COLOR_FORMAT_GET_BPP(OUTPUT_CANVAS_CF), LV_DRAW_BUF_STRIDE_ALIGN)

struct zmk_widget_output_status {
    sys_snode_t node;
    lv_obj_t *obj;
    uint8_t canvas_buf[OUTPUT_CANVAS_BUF_SIZE];
};

int zmk_widget_output_status_init(struct zmk_widget_output_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_output_status_obj(struct zmk_widget_output_status *widget);
