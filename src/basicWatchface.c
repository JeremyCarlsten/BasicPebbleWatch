#include <pebble.h>
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1


// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_bitham_30_black;
static GFont s_res_roboto_condensed_21;
static TextLayer *s_time_text_layer;
static TextLayer *s_temperature_text_layer;
static BitmapLayer *s_bitmaplayer_1;
static TextLayer *s_date_text_layer;
// static char degreeSymbol[6] = "\u00B0";
static TextLayer *s_direction_text_layer;


static void update_time(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  static char s_date_buffer[10];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  strftime(s_date_buffer, sizeof(s_date_buffer), "%D" , tick_time);

  text_layer_set_text(s_time_text_layer, s_buffer);
  text_layer_set_text(s_date_text_layer, s_date_buffer);
}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, 1);
  #endif

  s_res_bitham_30_black = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  // s_time_text_layer
  s_time_text_layer = text_layer_create(GRect(0, 16, 144, 36));
  text_layer_set_text(s_time_text_layer, "03:04");
  text_layer_set_text_alignment(s_time_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_text_layer, s_res_bitham_30_black);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time_text_layer);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time_text_layer);

  // s_temperature_text_layer
  s_temperature_text_layer = text_layer_create(GRect(2, 141, 150, 49));
  text_layer_set_background_color(s_temperature_text_layer, GColorBlack);
  text_layer_set_text_color(s_temperature_text_layer, GColorWhite);
  text_layer_set_text(s_temperature_text_layer, "...");
  text_layer_set_text_alignment(s_temperature_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_temperature_text_layer, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_temperature_text_layer);

  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(3, 110, 33, 31));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);

  // s_date_layer
  s_date_text_layer = text_layer_create(GRect(15, 52, 116, 25));
  text_layer_set_text(s_date_text_layer, "01/24/2016");
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_text_layer, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_date_text_layer);

  // // s_direction_text_layer
  // s_direction_text_layer = text_layer_create(GRect(102, 143, 38, 21));
  // text_layer_set_background_color(s_direction_text_layer, GColorBlack);
  // text_layer_set_text_color(s_direction_text_layer, GColorWhite);
  // text_layer_set_text(s_direction_text_layer, "N");
  // text_layer_set_font(s_direction_text_layer, s_res_roboto_condensed_21);
  // layer_add_child(window_get_root_layer(s_window), (Layer *)s_direction_text_layer);
  // layer_add_child(window_get_root_layer(s_window), (Layer *)s_direction_text_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_time_text_layer);
  text_layer_destroy(s_temperature_text_layer);
  bitmap_layer_destroy(s_bitmaplayer_1);
  text_layer_destroy(s_date_text_layer);
  text_layer_destroy(s_direction_text_layer);
}
// END AUTO-GENERATED UI CODE

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();

  // Get weather update every 30 minutes
 if(tick_time->tm_min % 30 == 0) {
   // Begin dictionary
   DictionaryIterator *iter;
   app_message_outbox_begin(&iter);

   // Add a key-value pair
   dict_write_uint8(iter, 0, 0);

   // Send the message!
   app_message_outbox_send();
 }
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];

  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);

  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);

    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    text_layer_set_text(s_temperature_text_layer, weather_layer_buffer);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


static void init() {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);

  //start date/time
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  tick_timer_service_subscribe(DAY_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static void deinit() {
  window_stack_remove(s_window, true);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
