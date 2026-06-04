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
#include "output_status.h"
#include "canvas_utils.h"
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>

LV_IMG_DECLARE(bluetooth_advertising);
LV_IMG_DECLARE(bluetooth_connected_right);
LV_IMG_DECLARE(bluetooth_disconnected_right);
LV_IMG_DECLARE(USB_connected);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct output_status_state {
    enum zmk_transport transport;
    int active_profile_index;
    bool active_profile_connected;
    bool active_profile_bonded;
};

static struct output_status_state get_state(const zmk_event_t *_eh) {
    return (struct output_status_state){
        .transport = zmk_endpoint_get_selected().transport,
        .active_profile_index = zmk_ble_active_profile_index(),
        .active_profile_connected = zmk_ble_active_profile_is_connected(),
        .active_profile_bonded = !zmk_ble_active_profile_is_open(),
    };
}

static void draw_output_widget(lv_obj_t *canvas, struct output_status_state state) {
    lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);

    const lv_image_dsc_t *icon;
    bool show_profile = false;
    int profile = state.active_profile_index;
    char suffix = '\0';

    if (state.transport == ZMK_TRANSPORT_USB) {
        icon = &USB_connected;
    } else {
        show_profile = true;
        if (state.active_profile_bonded) {
            if (state.active_profile_connected) {
                icon = &bluetooth_connected_right;
            } else {
                icon = &bluetooth_disconnected_right;
                suffix = 'D';
            }
        } else {
            icon = &bluetooth_advertising;
            suffix = 'P';
        }
    }

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    img_dsc.src = icon;

    lv_layer_t img_layer;
    lv_canvas_init_layer(canvas, &img_layer);
    lv_area_t img_coords = {0, 0, (int32_t)icon->header.w - 1, (int32_t)icon->header.h - 1};
    lv_draw_image(&img_layer, &img_dsc, &img_coords);
    lv_canvas_finish_layer(canvas, &img_layer);

    if (show_profile) {
        char text[4];
        if (suffix) {
            snprintf(text, sizeof(text), "%d%c", profile, suffix);
        } else {
            snprintf(text, sizeof(text), "%d", profile);
        }

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_black();
        label_dsc.font = &lv_font_montserrat_16;
        label_dsc.text = text;

        lv_layer_t txt_layer;
        lv_canvas_init_layer(canvas, &txt_layer);
        lv_area_t txt_coords = {56, 10, 79, 28};
        lv_draw_label(&txt_layer, &label_dsc, &txt_coords);
        lv_canvas_finish_layer(canvas, &txt_layer);
    }

    canvas_threshold(canvas, OUTPUT_CANVAS_BUF_SIZE);
}

static void output_status_update_cb(struct output_status_state state) {
    static struct output_status_state prev = {.active_profile_index = -1};

    if (state.transport == prev.transport &&
        state.active_profile_index == prev.active_profile_index &&
        state.active_profile_connected == prev.active_profile_connected &&
        state.active_profile_bonded == prev.active_profile_bonded) {
        return;
    }
    prev = state;

    struct zmk_widget_output_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        draw_output_widget(widget->obj, state);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);
ZMK_SUBSCRIPTION(widget_output_status, zmk_activity_state_changed);
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
#endif

int zmk_widget_output_status_init(struct zmk_widget_output_status *widget, lv_obj_t *parent) {
    widget->obj = lv_canvas_create(parent);
    lv_canvas_set_buffer(widget->obj, widget->canvas_buf, OUTPUT_CANVAS_W, OUTPUT_CANVAS_H,
                         OUTPUT_CANVAS_CF);

    struct output_status_state initial = get_state(NULL);
    draw_output_widget(widget->obj, initial);

    sys_slist_append(&widgets, &widget->node);
    widget_output_status_init();

    return 0;
}

lv_obj_t *zmk_widget_output_status_obj(struct zmk_widget_output_status *widget) {
    return widget->obj;
}
