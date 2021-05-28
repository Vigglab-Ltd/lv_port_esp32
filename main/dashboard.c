
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
//lv_obj_t *fahrenheit_bar = NULL;
//lv_obj_t *dirtyLED = NULL;
lv_obj_t *dirty_btn = NULL;
lv_obj_t *dirty_btn_label = NULL;
lv_obj_t *wet_btn = NULL;
lv_obj_t *wet_btn_label = NULL;
lv_obj_t *chart = NULL;
lv_chart_series_t* chart_fahrenheit_series = NULL;
lv_chart_series_t* chart_activity_series = NULL;

bool dashboard_needs_redrawn = true;
uint8_t brightness;
uint8_t water;
float fahrenheit;

int diii = 0;

void update_thermometer();
void update_dirty();
void update_water();
void update_chart();

double SinRange(double input, double range_min, double range_max)
{
    double range = range_max - range_min;
    double v1 = sin(input);
    double v2 = (range / 2.0) * v1;
    double v3 = v2 + ((range_max + range_min) / 2.0);
    return v3;
}
void update_dashboard(lv_task_t *task)
{
    //  double di = diii++;

    //  fahrenheit = SinRange(di / 10.0, 90.0, 105.0);
    //  brightness = SinRange(di / 10.0, 0.0, 120.0);
    //  water      = SinRange(di / 10.0, 0.0, 120.0);
    //  dashboard_needs_redrawn = true;

    if (dashboard_needs_redrawn)
    {
        dashboard_needs_redrawn = false;

        update_thermometer();
        update_dirty();
        update_water();
        update_chart();
    }
}

void update_chart()
{
    float scale_max = 105.0;
    float scale_min = 90.0;
    float temp = (fahrenheit - scale_min) / (scale_max - scale_min) ;
    lv_coord_t scaled_fahrenheit = 1000 * temp;

    lv_chart_set_next(chart, chart_fahrenheit_series, scaled_fahrenheit);
}

void update_thermometer()
{
    lv_label_set_text_fmt(fahrenheit_label, "#F0F0F0 %.1f°F #", fahrenheit);
    //if (fahrenheit > 100.0f)
    //{
    //    lv_label_set_text_fmt(fahrenheit_label, "#FF0000 %.1f°F #", fahrenheit);
    //}
    //else if (fahrenheit < 97.0f)
    //{
    //    lv_label_set_text_fmt(fahrenheit_label, "#0000FF %.1f°F #", fahrenheit);
    //}
    //else
    //{
    //    lv_label_set_text_fmt(fahrenheit_label, "#00FF00 %.1f°F #", fahrenheit);
    //}
//    lv_bar_set_value(fahrenheit_bar, (int16_t)fahrenheit * 10.0f, LV_ANIM_ON);
}


void update_dirty()
{
    if (brightness > 100)
    {
        lv_btn_set_state(dirty_btn, LV_BTN_STATE_RELEASED);
        lv_label_set_text(dirty_btn_label, "Clean");
    }
    else if (brightness > 70)
    {
        lv_btn_set_state(dirty_btn, LV_BTN_STATE_PRESSED);
        lv_label_set_text(dirty_btn_label, "Check");
    }
    else
    {
        lv_btn_set_state(dirty_btn, LV_BTN_STATE_DISABLED);
        lv_label_set_text(dirty_btn_label, "Dirty");
    }
}



void update_water()
{
    if (water < 10)
    {
        lv_btn_set_state(wet_btn, LV_BTN_STATE_RELEASED);
        lv_label_set_text(wet_btn_label, "Dry");
    }
    else if (water < 30)
    {
        lv_btn_set_state(wet_btn, LV_BTN_STATE_PRESSED);
        lv_label_set_text(wet_btn_label, "Damp");
    }
    else
    {
        lv_btn_set_state(wet_btn, LV_BTN_STATE_DISABLED);
        lv_label_set_text(wet_btn_label, "Wet");
    }
}





void make_thermometer(lv_obj_t* scr)
{
//    lv_obj_t *thermometer_container = lv_cont_create(scr, NULL);
    //lv_obj_set_auto_realign(thermometer_container, true);
    //lv_cont_set_fit(thermometer_container, LV_FIT_TIGHT);
    //lv_cont_set_layout(thermometer_container, LV_LAYOUT_COLUMN_MID);

    //fahrenheit_bar = lv_bar_create(scr, NULL);
    //lv_bar_set_range(fahrenheit_bar, 900, 1050);       //90F to 105F
    //lv_obj_set_pos(fahrenheit_bar, (160 + 128) / 2, 0);
    //lv_obj_set_size(fahrenheit_bar, 10, 50);

    //{
    //    static lv_style_t style_bar_part_bg;
    //    lv_style_init(&style_bar_part_bg);
    //    lv_style_set_line_color(&style_bar_part_bg, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
    //    lv_style_set_line_color(&style_bar_part_bg, LV_STATE_DEFAULT, LV_COLOR_ORANGE);

    //    lv_style_set_bg_color(&style_bar_part_bg, LV_STATE_DEFAULT, LV_COLOR_RED);
    //    lv_style_set_bg_grad_dir(&style_bar_part_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    //    lv_style_set_bg_grad_color(&style_bar_part_bg, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    //    lv_style_set_border_color(&style_bar_part_bg, LV_STATE_DEFAULT, LV_COLOR_YELLOW);

    //    lv_obj_add_style(fahrenheit_bar, LV_BAR_PART_BG, &style_bar_part_bg);   /*Add the red style on top of the current */
    //}
    //{
    //    static lv_style_t style_bar_part_indic;
    //    lv_style_init(&style_bar_part_indic);
    //    lv_style_set_bg_color(&style_bar_part_indic, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    //    lv_style_set_line_rounded(&style_bar_part_indic, LV_STATE_DEFAULT, false);
    //    lv_style_set_line_color(&style_bar_part_indic, LV_STATE_DEFAULT, LV_COLOR_ORANGE);

    //    //lv_style_set_outline_width(&style_bar_part_indic, LV_STATE_DEFAULT, 4);
    //    //lv_style_set_outline_color(&style_bar_part_indic, LV_STATE_DEFAULT, LV_COLOR_ORANGE);

    //    //lv_style_set_bg_grad_dir(&style_bar_part_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    //    //lv_style_set_bg_grad_color(&style_bar_part_indic, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    //    //lv_style_set_border_color(&style_bar_part_indic, LV_STATE_DEFAULT, LV_COLOR_YELLOW);

    //    lv_obj_add_style(fahrenheit_bar, LV_BAR_PART_INDIC, &style_bar_part_indic);   /*Add the red style on top of the current */
    //}


    {
        lv_obj_t* container;
        container = lv_cont_create(scr, NULL);
        lv_cont_set_layout(container, LV_LAYOUT_COLUMN_RIGHT);
        lv_obj_set_pos(container, 110, 0);
        lv_obj_set_size(container, 50, 14);
        lv_obj_set_auto_realign(container, true);
        {
            static lv_style_t container_style;
            lv_style_init(&container_style);
            lv_style_set_bg_color(&container_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
            lv_style_set_pad_all(&container_style, LV_STATE_DEFAULT, 0);
            lv_style_set_border_color(&container_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
            lv_obj_add_style(container, LV_CONT_PART_MAIN, &container_style);
        }

        fahrenheit_label = lv_label_create(container, NULL);
        lv_label_set_recolor(fahrenheit_label, true);
        lv_obj_align(fahrenheit_label, NULL, LV_ALIGN_CENTER, 0, 0);

//        lv_obj_set_pos(fahrenheit_label, 110, 0);
//        lv_obj_set_size(fahrenheit_label, 30, 20);

        {
            static lv_style_t fahrenheit_label_style;
            lv_style_init(&fahrenheit_label_style);
            lv_style_set_text_font(&fahrenheit_label_style, LV_STATE_DEFAULT, &lv_font_montserrat_12 );
            lv_obj_add_style(fahrenheit_label, LV_LABEL_PART_MAIN, &fahrenheit_label_style);
        }
    }
}

void make_dirty_btn(lv_obj_t* scr)
{

    dirty_btn = lv_btn_create(scr, NULL);
    
    lv_obj_set_pos(dirty_btn, 0, 0);
    lv_obj_set_size(dirty_btn, 40, 14);

    lv_btn_set_checkable(dirty_btn, true);
    static lv_style_t style_btn_red;
    lv_style_init(&style_btn_red);

    //Clean
    lv_style_set_bg_color(&style_btn_red,       LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_grad_color(&style_btn_red,  LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_style_set_text_color(&style_btn_red,     LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_border_color(&style_btn_red,   LV_STATE_DEFAULT, LV_COLOR_GRAY);

    //Check
    lv_style_set_bg_color(&style_btn_red,       LV_STATE_PRESSED, LV_COLOR_ORANGE);
    lv_style_set_bg_grad_color(&style_btn_red,  LV_STATE_PRESSED, LV_COLOR_MAROON);
    lv_style_set_text_color(&style_btn_red,     LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_style_set_border_color(&style_btn_red,   LV_STATE_PRESSED, LV_COLOR_GRAY);

    //Dirty
    lv_style_set_bg_color(&style_btn_red,       LV_STATE_DISABLED, LV_COLOR_MAKE(0xFF, 0x00, 0x00));
    lv_style_set_bg_grad_color(&style_btn_red,  LV_STATE_DISABLED, LV_COLOR_RED);
    lv_style_set_text_color(&style_btn_red,     LV_STATE_DISABLED, LV_COLOR_WHITE);
    lv_style_set_border_color(&style_btn_red,   LV_STATE_DISABLED, LV_COLOR_GRAY);

    lv_obj_add_style(dirty_btn, LV_BTN_PART_MAIN, &style_btn_red);   /*Add the red style on top of the current */

    dirty_btn_label = lv_label_create(dirty_btn, NULL);
    lv_label_set_text(dirty_btn_label, "Clean");
    lv_obj_set_size(dirty_btn_label, 30, 14);
}



void make_wet_btn(lv_obj_t* scr)
{
    wet_btn = lv_btn_create(scr, NULL);
    lv_obj_set_pos(wet_btn, 40, 0);
    lv_obj_set_size(wet_btn, 40, 14);

    lv_btn_set_checkable(wet_btn, true);
    static lv_style_t style_btn_red;
    lv_style_init(&style_btn_red);

    //Clean
    lv_style_set_bg_color(&style_btn_red,       LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_grad_color(&style_btn_red,  LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_style_set_text_color(&style_btn_red,     LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_border_color(&style_btn_red,   LV_STATE_DEFAULT, LV_COLOR_GRAY);

    //Check
    lv_style_set_bg_color(&style_btn_red,       LV_STATE_PRESSED, LV_COLOR_ORANGE);
    lv_style_set_bg_grad_color(&style_btn_red,  LV_STATE_PRESSED, LV_COLOR_MAROON);
    lv_style_set_text_color(&style_btn_red,     LV_STATE_PRESSED, LV_COLOR_WHITE);
    lv_style_set_border_color(&style_btn_red,   LV_STATE_PRESSED, LV_COLOR_GRAY);

    //Dirty
    lv_style_set_bg_color(&style_btn_red,       LV_STATE_DISABLED, LV_COLOR_MAKE(0xFF, 0x00, 0x00));
    lv_style_set_bg_grad_color(&style_btn_red,  LV_STATE_DISABLED, LV_COLOR_RED);
    lv_style_set_text_color(&style_btn_red,     LV_STATE_DISABLED, LV_COLOR_WHITE);
    lv_style_set_border_color(&style_btn_red,   LV_STATE_DISABLED, LV_COLOR_GRAY);

    lv_obj_add_style(wet_btn, LV_BTN_PART_MAIN, &style_btn_red);   /*Add the red style on top of the current */


    wet_btn_label = lv_label_create(wet_btn, NULL);
    lv_label_set_text(wet_btn_label, "Damp");
    lv_obj_set_size(wet_btn_label, 30, 14);
}







void make_chart(lv_obj_t* scr)
{
    chart = lv_chart_create(scr, NULL);
    lv_obj_set_pos(chart, 0, 15);
    lv_obj_set_size(chart, 120, 40);
    lv_chart_set_range(chart, 0, 1000);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(chart, 0, 0);
    chart_fahrenheit_series = lv_chart_add_series(chart, LV_COLOR_MAKE(0x00, 0x7F, 0x7F));
     //= lv_chart_add_series(chart, LV_THEME_DEFAULT_COLOR_SECONDARY);

    lv_chart_set_point_count(chart, 120);
    lv_chart_init_points(chart, chart_fahrenheit_series, 0);

    {
        static lv_style_t chart_part_series_style;
        lv_style_init(&chart_part_series_style);
        lv_style_set_pad_inner(&chart_part_series_style, LV_STATE_DEFAULT, 0);
        lv_style_set_size(&chart_part_series_style, LV_STATE_DEFAULT, 1);

        lv_obj_add_style(chart, LV_CHART_PART_SERIES, &chart_part_series_style);
    }
    {
        static lv_style_t chart_part_bg_style;
        lv_style_init(&chart_part_bg_style);
        lv_style_set_bg_color(&chart_part_bg_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_style_set_bg_grad_color(&chart_part_bg_style, LV_STATE_DEFAULT, LV_COLOR_MAKE(50,50,50));
        lv_style_set_bg_grad_dir(&chart_part_bg_style, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_style_set_bg_main_stop(&chart_part_bg_style, LV_STATE_DEFAULT, 20);
        lv_style_set_bg_grad_stop(&chart_part_bg_style, LV_STATE_DEFAULT, 240);

        lv_style_set_border_color(&chart_part_bg_style, LV_STATE_DEFAULT, LV_COLOR_MAKE(50,50,50));

        lv_obj_add_style(chart, LV_CHART_PART_BG, &chart_part_bg_style);
    }
    {
        static lv_style_t chart_part_series_bg_style;
        lv_style_init(&chart_part_series_bg_style);

        //lv_style_set_outline_color(&chart_part_series_bg_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
        //lv_style_set_border_color(&chart_part_series_bg_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);

        lv_obj_add_style(chart, LV_CHART_PART_SERIES_BG, &chart_part_series_bg_style);
    }
    {
        static lv_style_t chart_part_cursor_style;
        lv_style_init(&chart_part_cursor_style);

        //lv_style_set_outline_color(&chart_part_cursor_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
        //lv_style_set_border_color(&chart_part_cursor_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);

        lv_obj_add_style(chart, LV_CHART_PART_CURSOR, &chart_part_cursor_style);

    }


    lv_obj_set_style_local_bg_opa       (chart,  LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir  (chart,  LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop (chart,  LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop (chart,  LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);      /*Transparent on the bottom*/

    //from lv_theme_material.c
            //list = lv_obj_get_style_list(obj, LV_CHART_PART_BG);
            //_lv_style_list_add_style(list, &styles->bg);
            //_lv_style_list_add_style(list, &styles->chart_bg);
            //_lv_style_list_add_style(list, &styles->pad_small);

            //list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES_BG);
            //_lv_style_list_add_style(list, &styles->pad_small);
            //_lv_style_list_add_style(list, &styles->chart_series_bg);

            //list = lv_obj_get_style_list(obj, LV_CHART_PART_CURSOR);
            //_lv_style_list_add_style(list, &styles->chart_series_bg);

            //list = lv_obj_get_style_list(obj, LV_CHART_PART_SERIES);
            //_lv_style_list_add_style(list, &styles->chart_series);


    //LV_CHART_PART_SERIES_BG
    //lv_obj_set_style_local_text_font(chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, lv_theme_get_font_small());
    //lv_obj_set_width_margin(chart, 1);
    //lv_obj_set_height_margin(chart, 1);
    //lv_chart_set_div_line_count(chart, 3, 0);
    //lv_chart_set_range(chart, 600, 1200);// temperature range
}





void create_dashboard_UI(void)
{
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);

    lv_obj_set_style_local_bg_color (lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_bg_opa( lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);

    //lv_disp_set_bg_color(NULL, LV_COLOR_BLACK);
    //lv_disp_set_bg_opa(NULL, 129);
    
    make_thermometer(scr);
    make_dirty_btn(scr);
    make_wet_btn(scr);
    make_chart(scr);

    dashboard_needs_redrawn = false;

    lv_task_create(update_dashboard, 100, LV_TASK_PRIO_MID, NULL);
}

