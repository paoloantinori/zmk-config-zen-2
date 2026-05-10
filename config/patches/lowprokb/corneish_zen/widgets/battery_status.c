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
#include <zmk/battery.h>
#include "battery_status.h"
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

LV_IMG_DECLARE(batt_0);
LV_IMG_DECLARE(batt_5);
LV_IMG_DECLARE(batt_25);
LV_IMG_DECLARE(batt_50);
LV_IMG_DECLARE(batt_75);
LV_IMG_DECLARE(batt_100);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_status_state {
    uint8_t level;
};

static const lv_image_dsc_t *get_batt_icon(uint8_t level) {
    if (level == 0) {
        return &batt_0;
    } else if (level <= 5) {
        return &batt_5;
    } else if (level <= 25) {
        return &batt_25;
    } else if (level <= 50) {
        return &batt_50;
    } else if (level <= 75) {
        return &batt_75;
    } else {
        return &batt_100;
    }
}

static void draw_battery_widget(lv_obj_t *canvas, uint8_t level) {
    lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);

    const lv_image_dsc_t *icon = get_batt_icon(level);

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    img_dsc.src = icon;

    lv_layer_t img_layer;
    lv_canvas_init_layer(canvas, &img_layer);
    lv_area_t img_coords = {8, 0, 8 + (int32_t)icon->header.w - 1,
                            (int32_t)icon->header.h - 1};
    lv_draw_image(&img_layer, &img_dsc, &img_coords);
    lv_canvas_finish_layer(canvas, &img_layer);

    char text[4];
    if (level == 0) {
        text[0] = '-';
        text[1] = '-';
        text[2] = '\0';
    } else {
        snprintf(text, sizeof(text), "%u", level);
    }

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_black();
    label_dsc.font = &lv_font_montserrat_16;
    label_dsc.text = text;

    lv_layer_t txt_layer;
    lv_canvas_init_layer(canvas, &txt_layer);
    lv_area_t txt_coords = {54, 7, 79, 30};
    lv_draw_label(&txt_layer, &label_dsc, &txt_coords);
    lv_canvas_finish_layer(canvas, &txt_layer);
}

static void set_battery_symbol(lv_obj_t *canvas, struct battery_status_state state) {
    uint8_t level = state.level;
    static uint8_t level_prev = 255;

    if (level != level_prev) {
        draw_battery_widget(canvas, level);
        level_prev = level;
    }
}

void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_battery_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_battery_symbol(widget->obj, state);
    }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_status_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);

int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent) {
    widget->obj = lv_canvas_create(parent);
    lv_canvas_set_buffer(widget->obj, widget->canvas_buf, BATT_CANVAS_W, BATT_CANVAS_H,
                         BATT_CANVAS_CF);

    draw_battery_widget(widget->obj, 0);

    sys_slist_append(&widgets, &widget->node);
    widget_battery_status_init();

    return 0;
}

lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget) {
    return widget->obj;
}
