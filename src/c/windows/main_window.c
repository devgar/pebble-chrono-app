#include "main_window.h"

static Window *s_window;
static TextLayer *s_time_layer, *s_txts_layer;
static BitmapLayer *s_bb1_layer, *s_bb2_layer;
static GFont s_time_font, s_mult_font;
static GRect bounds;
static AppTimer * timer;
static char s_title_arr[8] = "00:00:0", s_multime_arr[100] = "";
static uint64_t START_MS = 0, PAUSE_MS=0, LAPS[16];
static uint LAPS_NUM = 0;

static void bb_layer_load(Layer *window_layer){
  
  int bSize = (bounds.size.h-50)/2;
  
  s_bb1_layer = bitmap_layer_create(
    GRect(bounds.size.w - 24, 50, 24, bSize) );
  bitmap_layer_set_background_color(s_bb1_layer, GColorLightGray);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bb1_layer));
  
  s_bb2_layer = bitmap_layer_create(
    GRect(bounds.size.w - 24, 50 + bSize, 24,  bSize));
  bitmap_layer_set_background_color(s_bb2_layer, GColorBlack);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bb2_layer));
}

static void title_layer_load(Layer *window_layer) {
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VT_42));
  s_mult_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VT_20));
  s_time_layer = text_layer_create(GRect(0,  0, bounds.size.w, 50));

  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorClear);
  text_layer_set_text(s_time_layer, s_title_arr);
  text_layer_set_font(s_time_layer, s_time_font);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  s_txts_layer = text_layer_create(GRect(0,  50, bounds.size.w, 136));

  text_layer_set_text_color(s_txts_layer, GColorBlack);
  text_layer_set_text(s_txts_layer, s_multime_arr);
  text_layer_set_font(s_txts_layer, s_mult_font);
  

  layer_add_child(window_layer, text_layer_get_layer(s_txts_layer));
}
static uint64_t getTime()
{
  return time(NULL) * 1000 + time_ms(NULL, NULL);
}

static void emptyArr(char o[], uint max)
{
  for(uint i = 0; i<max; i++)
    o[i] = '\0';
}

static void timer_display(uint64_t ms, char o[], uint i){
  uint s = ms / 1000 % 60;
  uint m = ms / 1000 / 60 % 60;
  o[i + 7] = '\0';
  o[i + 6] = ms / 100 % 10 + '0';
  o[i + 5] = ':';
  o[i + 4] = s % 10 + '0';
  o[i + 3] = s / 10  % 6 + '0';
  o[i + 2] = ':';
  o[i + 1] = m % 10 + '0';
  o[i + 0] = m / 10 % 6 + '0';
}

static void multime()
{
  if(LAPS_NUM < 1) return;
  timer_display(LAPS[0] - START_MS, s_multime_arr, 0);
  s_multime_arr[7] = ' ';
  timer_display(LAPS[0] - START_MS, s_multime_arr, 8);
  s_multime_arr[15] = '\n';
  for(uint i=1; i<LAPS_NUM; i++){
    timer_display(LAPS[i] - LAPS[i-1], s_multime_arr, i*16);
    s_multime_arr[7 + i*16] = ' ';
    timer_display(LAPS[i] - START_MS, s_multime_arr, 8+i*16);
    s_multime_arr[15 + i*16] = '\n';
  }
  timer_display(getTime() - LAPS[LAPS_NUM-1], s_multime_arr, LAPS_NUM* 16);
  s_multime_arr[7 + LAPS_NUM*16] = ' ';
  timer_display(getTime() - START_MS, s_multime_arr, 8+LAPS_NUM* 16);
}

static void timer_callback(){
  timer_display(getTime() - START_MS, s_title_arr, 0);
  text_layer_set_text(s_time_layer, s_title_arr);
  if(LAPS_NUM != 0 )
  {
    multime();
    text_layer_set_text(s_txts_layer, s_multime_arr);
  }
  timer = app_timer_register(100, timer_callback, NULL);
}

static void start_chronno(){
  uint64_t NOW = getTime();
  START_MS = NOW - (PAUSE_MS - START_MS);
  for(uint i = 0; i < LAPS_NUM; i++)
    LAPS[i] = NOW - (PAUSE_MS - LAPS[i]);
  PAUSE_MS = 0;
  
  timer = app_timer_register(100, timer_callback, NULL);
}

static void pause_chronno(){
  PAUSE_MS = getTime();
  app_timer_cancel(timer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(START_MS == 0 || PAUSE_MS >0) start_chronno();
  else pause_chronno();
}

static void down_click_handler(ClickRecognizerRef recognizer, void * context){
  if(PAUSE_MS == 0 && START_MS > 0)
  {
    if(LAPS_NUM < 4) LAPS[LAPS_NUM++] = getTime();
  } else {
    LAPS_NUM = 0;
    emptyArr(s_multime_arr, 100);
    text_layer_set_text(s_txts_layer, s_multime_arr);
  }
}

static void click_config_provider(void * context){
  ButtonId id = BUTTON_ID_SELECT;  // The Select button
  window_single_click_subscribe(id, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);
  title_layer_load(window_layer);
  bb_layer_load(window_layer);
  window_set_click_config_provider(window, click_config_provider);
}

static void window_unload(Window *window) {
  window_destroy(s_window);
  text_layer_destroy(s_time_layer);
  bitmap_layer_destroy(s_bb1_layer);
  bitmap_layer_destroy(s_bb2_layer);
  fonts_unload_custom_font(s_time_font);
}

void main_window_push() {
  if(!s_window) {
    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
  }
  window_stack_push(s_window, true);
}
