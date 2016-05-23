#include <pebble.h>
#include "config.h"
#include "color.h"
#include "display_day.h"
#include "display_time.h"
#include "ornament.h"
#include "monitor_status.h"

static Window *s_window;
GColor bg_color;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
	
	key_initialize();
	set_color();
	
	window_set_background_color(s_window, bg_color);
	
	init_status(window_layer);
	monitor_battery();
	
	init_ornament(window_layer);
	display_ticks();
	
	monitor_connection();

	init_day(window_layer);
	display_day();
	
	init_time(window_layer);
	display_time();
}

static void window_unload(Window *window) {
	deinit_time();
	deinit_day();
	deinit_ornament();
	deinit_status();
}

static void load_window() {
	s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

static void reload_window() {
	window_stack_remove(s_window, true);
	load_window();
	
	time_service_off();
	time_service_on();
}

static void init() {
	load_window();
	
	config_service();
	time_service_on();
	status_service_on();
}

static void deinit() {
	time_service_off();
	status_service_off();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
