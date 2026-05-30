/*
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

static inline void canvas_threshold(lv_obj_t *canvas, uint32_t size) {
    uint8_t *buf = (uint8_t *)lv_canvas_get_buf(canvas);
    for (uint32_t i = 0; i < size; i++) {
        buf[i] = buf[i] < 128 ? 0x00 : 0xFF;
    }
}
