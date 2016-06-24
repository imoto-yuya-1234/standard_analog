#include <pebble.h>
#include "config.h"
#include "display_day.h"
#include "display_time.h"
#include "ornament.h"
#include "monitor_status.h"

extern Layer *g_window_layer;
extern GRect g_bounds;
extern GPoint g_center;

GColor g_bg_color;

static Window *s_window;

static void window_load(Window *window) {
  g_window_layer = window_get_root_layer(window);
  g_bounds = layer_get_bounds(g_window_layer);
  g_center = grect_center_point(&g_bounds);
	
	initialize_value();
	
	window_set_background_color(s_window, g_bg_color);
	
	init_day();
	display_day();
	
	init_status();
	display_connection();
	
	init_rot();
	
	init_ornament();
	display_edge();
	display_ticks();
	
	init_time();
	display_time();
}

static void window_unload(Window *window) {
	deinit_time();
	deinit_day();
	deinit_status();
	deinit_rot();
	deinit_ornament();
}

static void load_window() {
	s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

void reload_window() {
	window_stack_remove(s_window, true);
	load_window();
	
	time_service_off();
	time_service_on();
}

static void init() {
	load_window();
	
	config_service();
	status_service_on();
	time_service_on();
}

static void deinit() {
	status_service_off();
	time_service_off();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
