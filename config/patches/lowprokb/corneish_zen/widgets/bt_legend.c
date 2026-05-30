/*
 *
 * Copyright (c) 2021 Darryl deHaan
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>

#include <zmk/display.h>
#include "bt_legend.h"
#include "profile_labels.h"

int zmk_widget_bt_legend_init(struct zmk_widget_bt_legend *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    static char text[128];
    int pos = 0;

    for (int i = 0; i < PROFILE_COUNT; i++) {
        const char *label = profile_label_get(i);
        if (i > 0) {
            text[pos++] = '\n';
        }
        if (label != NULL) {
            pos += snprintf(text + pos, sizeof(text) - pos, "%d %s", i, label);
        } else {
            pos += snprintf(text + pos, sizeof(text) - pos, "%d \xe2\x80\x94", i);
        }
    }

    lv_label_set_text_static(widget->obj, text);

    return 0;
}

lv_obj_t *zmk_widget_bt_legend_obj(struct zmk_widget_bt_legend *widget) {
    return widget->obj;
}
