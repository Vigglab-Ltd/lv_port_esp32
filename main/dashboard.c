
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "blecent.h"




lv_obj_t *fahrenheit_label = NULL;
lv_obj_t *fahrenheit_bar = NULL;
//lv_obj_t *dirtyLED = NULL;
lv_obj_t *dirty_btn = NULL;
lv_obj_t *dirty_btn_label = NULL;
lv_obj_t *wet_btn = NULL;
lv_obj_t *wet_btn_label = NULL;
lv_obj_t *chart = NULL;
lv_chart_series_t* s1 = NULL;

bool dashboard_needs_redrawn = true;
uint8_t brightness;
uint8_t water;
float fahrenheit;

int diii = 0;

void update_thermometer();
void update_dirty();
void update_water();

void update_dashboard(lv_task_t *task)
{
    // double di = diii++;
    // double di2 = (50.0f * sin(di / 10.0)) + 50.0f;
    // lv_chart_set_next(chart, s1, di2);

    // fahrenheit = (30.0f * sin(di / 10.0)) + 90.0f;
    // brightness = (60.0f * sin(di / 10.0)) + 60.0f;
    // water = (60.0f * sin(di / 10.0)) + 60.0f;

    if (dashboard_needs_redrawn)
    {
        dashboard_needs_redrawn = false;

        update_thermometer();
        update_dirty();
        update_water();
        lv_chart_set_next(chart, s1, brightness);
    }
}

void update_thermometer()
{
    lv_label_set_text_fmt(fahrenheit_label, "%.1f°F", fahrenheit);
    lv_bar_set_value(fahrenheit_bar, (int16_t)fahrenheit * 10.0f, LV_ANIM_ON);
}


void update_dirty()
{
    if (brightness > 100)
    {
        lv_btn_set_state(dirty_btn, LV_STATE_DEFAULT);
        lv_label_set_text(dirty_btn_label, "Clean");
    }
    else if (brightness > 70)
    {
        lv_btn_set_state(dirty_btn, LV_STATE_PRESSED);
        lv_label_set_text(dirty_btn_label, "Check");
    }
    else
    {
        lv_btn_set_state(dirty_btn, LV_STATE_CHECKED);
        lv_label_set_text(dirty_btn_label, "Dirty");
    }
}



void update_water()
{
    if (water < 10)
    {
        lv_btn_set_state(wet_btn, LV_STATE_DEFAULT);
        lv_label_set_text(wet_btn_label, "Dry");
    }
    else if (water < 70)
    {
        lv_btn_set_state(wet_btn, LV_STATE_PRESSED);
        lv_label_set_text(wet_btn_label, "Damp");
    }
    else
    {
        lv_btn_set_state(wet_btn, LV_STATE_CHECKED);
        lv_label_set_text(wet_btn_label, "Wet");
    }
}





void make_thermometer(lv_obj_t* scr)
{
//    lv_obj_t *thermometer_container = lv_cont_create(scr, NULL);
    //lv_obj_set_auto_realign(thermometer_container, true);
    //lv_cont_set_fit(thermometer_container, LV_FIT_TIGHT);
    //lv_cont_set_layout(thermometer_container, LV_LAYOUT_COLUMN_MID);

    fahrenheit_bar = lv_bar_create(scr, NULL);
    lv_bar_set_range(fahrenheit_bar, 600, 1200);       //60F to 120F
    lv_obj_set_pos(fahrenheit_bar, (160 + 128) / 2, 0);
    lv_obj_set_size(fahrenheit_bar, 10, 50);

    fahrenheit_label = lv_label_create(scr, NULL);
    lv_obj_set_pos(fahrenheit_label, 128, 55);
    lv_obj_set_size(fahrenheit_label, 30, 20);
}

void make_dirty_btn(lv_obj_t* scr)
{
    dirty_btn = lv_btn_create(scr, NULL);
    lv_obj_set_pos(dirty_btn, 0, 0);
    lv_obj_set_size(dirty_btn, 40, 14);

    dirty_btn_label = lv_label_create(dirty_btn, NULL);
    lv_label_set_text(dirty_btn_label, "Clean");
    lv_obj_set_size(dirty_btn_label, 30, 14);
}



void make_wet_btn(lv_obj_t* scr)
{
    wet_btn = lv_btn_create(scr, NULL);
    lv_obj_set_pos(wet_btn, 40, 0);
    lv_obj_set_size(wet_btn, 40, 14);

    wet_btn_label = lv_label_create(wet_btn, NULL);
    lv_label_set_text(wet_btn_label, "Damp");
    lv_obj_set_size(wet_btn_label, 30, 14);
}







void make_chart(lv_obj_t* scr)
{
    chart = lv_chart_create(scr, NULL);
    lv_obj_set_pos(chart, 0, 15);
    lv_obj_set_size(chart, 120, 60);



    //lv_obj_set_style_local_text_font(chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, lv_theme_get_font_small());
    //lv_obj_set_width_margin(chart, 1);
    //lv_obj_set_height_margin(chart, 1);
    //lv_chart_set_div_line_count(chart, 3, 0);
    //lv_chart_set_range(chart, 600, 1200);// temperature range
    lv_chart_set_range(chart, 0, 100);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    //lv_chart_set_div_line_count(chart, 1, 4);
    s1 = lv_chart_add_series(chart, LV_THEME_DEFAULT_COLOR_SECONDARY);
    //lv_chart_series_t * s2 = lv_chart_add_series(chart, LV_THEME_DEFAULT_COLOR_SECONDARY);
    lv_chart_set_point_count(chart, 120);

    static lv_style_t chart_style;
    lv_style_init(&chart_style);
    lv_style_set_pad_inner(&chart_style, LV_STATE_DEFAULT, 0);
    lv_style_set_size(&chart_style, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(chart, LV_CHART_PART_SERIES, &chart_style);

    //    for (int ii = 0; ii < 120; ii++)
    //    {
    //        double di = ii;
    //        double di2 = sin(di / 10.0);
    //        double di3 = 50.0f * di2;
    //        double di4 = 50.0 + di3;
    ////        printf("di4 = %g", di4);
    //
    //        lv_chart_set_next(chart, s1, di4);
    //        
    //    }
    //for (int ii = 0; ii < 12; ii++)
    //{
    //    lv_chart_set_next(chart, s1, (ii*8 + 10)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 90)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 30)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 60)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 10)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 90)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 30)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 60)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 10)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 90)/2);
    //    lv_chart_set_next(chart, s1, (ii*8 + 10)/2);
    //}



    //lv_chart_set_next(chart, s2, 32);
    //lv_chart_set_next(chart, s2, 66);
    //lv_chart_set_next(chart, s2, 5);
    //lv_chart_set_next(chart, s2, 47);
    //lv_chart_set_next(chart, s2, 32);
    //lv_chart_set_next(chart, s2, 32);
    //lv_chart_set_next(chart, s2, 66);
    //lv_chart_set_next(chart, s2, 5);
    //lv_chart_set_next(chart, s2, 47);
    //lv_chart_set_next(chart, s2, 66);
    //lv_chart_set_next(chart, s2, 5);
    //lv_chart_set_next(chart, s2, 47);
}





void create_dashboard_UI(void)
{
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);

    make_thermometer(scr);
    make_dirty_btn(scr);
    make_wet_btn(scr);
    make_chart(scr);

    dashboard_needs_redrawn = false;

    lv_task_create(update_dashboard, 100, LV_TASK_PRIO_MID, NULL);
}

