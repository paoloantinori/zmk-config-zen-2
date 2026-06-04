/*
 *
 * Copyright (c) 2021 Darryl de Haan
 * SPDX-License-Identifier: MIT
 *
 */

#include "widgets/battery_status.h"
#include "widgets/peripheral_status.h"
#include "widgets/output_status.h"
#include "widgets/layer_status.h"
#include "widgets/profile_label.h"
#include "widgets/bt_legend.h"
#include "widgets/lock_status.h"
#include "widgets/mouse_status.h"
#include "widgets/verify_status.h"
#include "custom_status_screen.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_status_widget;
static struct zmk_widget_profile_label profile_label_widget;
static struct zmk_widget_lock_status lock_status_widget;
static struct zmk_widget_mouse_status mouse_status_widget;
static struct zmk_widget_verify_status verify_status_widget;
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_PERIPHERAL_STATUS)
static struct zmk_widget_peripheral_status peripheral_status_widget;
static struct zmk_widget_bt_legend bt_legend_widget;
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

lv_obj_t *zmk_display_status_screen() {

    lv_obj_t *screen;
    screen = lv_obj_create(NULL);

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_MID, 0, 2);
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_MID, 0, 41);

    zmk_widget_profile_label_init(&profile_label_widget, screen);
    lv_obj_set_style_text_font(zmk_widget_profile_label_obj(&profile_label_widget),
                               &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(zmk_widget_profile_label_obj(&profile_label_widget), LV_ALIGN_TOP_MID, 0, 78);
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_PERIPHERAL_STATUS)
    zmk_widget_bt_legend_init(&bt_legend_widget, screen);
    lv_obj_set_style_text_font(zmk_widget_bt_legend_obj(&bt_legend_widget),
                               &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_line_space(zmk_widget_bt_legend_obj(&bt_legend_widget), -3,
                                     LV_PART_MAIN);
    lv_obj_set_width(zmk_widget_bt_legend_obj(&bt_legend_widget), 80);
    lv_obj_align(zmk_widget_bt_legend_obj(&bt_legend_widget), LV_ALIGN_TOP_LEFT, 0, 53);
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget),
                               &lv_font_montserrat_16, LV_PART_MAIN);
#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_PERIPHERAL_STATUS)
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_TOP_MID, 0, 33);
#else
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_TOP_MID, 0, 102);
#endif
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_OUTPUT_STATUS) && IS_ENABLED(CONFIG_CUSTOM_WIDGET_LAYER_STATUS)
    zmk_widget_lock_status_init(&lock_status_widget,
                                zmk_widget_profile_label_obj(&profile_label_widget),
                                zmk_widget_layer_status_obj(&layer_status_widget));
    zmk_widget_mouse_status_init(&mouse_status_widget,
                                  zmk_widget_battery_status_obj(&battery_status_widget),
                                  zmk_widget_output_status_obj(&output_status_widget),
                                  zmk_widget_profile_label_obj(&profile_label_widget),
                                  zmk_widget_layer_status_obj(&layer_status_widget));
    zmk_widget_verify_status_init(&verify_status_widget,
                                  zmk_widget_profile_label_obj(&profile_label_widget),
                                  zmk_widget_layer_status_obj(&layer_status_widget));
#endif

    return screen;
}
