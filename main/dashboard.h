void create_dashboard_UI(void);



extern lv_obj_t *fahrenheit_label;
extern lv_obj_t *fahrenheit_bar;

extern lv_obj_t *dirty_btn;
extern lv_obj_t *dirty_btn_label;

extern lv_obj_t *wet_btn;
extern lv_obj_t *wet_btn_label;

extern lv_obj_t *chart;
extern lv_chart_series_t* s1;

extern bool dashboard_needs_redrawn;

extern uint8_t brightness;
extern uint8_t water;
extern float fahrenheit;

