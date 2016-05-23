#pragma once

#include <pebble.h>
#include "config.h"
#include "color.h"

static Layer *s_window_layer;
static GRect s_bounds;
static GPoint s_center;

static BitmapLayer *s_connection_layer;
static GBitmap *s_connection_bitmap;

static Layer *s_battery_layer;
static int s_battery_level;
 
uint32_t connection_icon;

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
	
	#if defined(PBL_ROUND) 
  int32_t angle_start = DEG_TO_TRIGANGLE(0);
	int32_t angle_end = DEG_TO_TRIGANGLE((int32_t)(360 * s_battery_level / 100));
	graphics_context_set_fill_color(ctx, GColorIslamicGreen);
	graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 8, angle_start, angle_end);
	#elif defined(PBL_RECT)
	int32_t decre_length = (int32_t)(604 - 604 * s_battery_level / 100);
	int32_t decre_1 = 0, decre_2 = 0, decre_3 = 0, decre_4 = 0, decre_5 = 0;
	graphics_context_set_fill_color(ctx, GColorIslamicGreen);
	graphics_context_set_stroke_color(ctx , GColorIslamicGreen);
	if(decre_length < 67) {
		decre_1 = decre_length;
	}
	else if(decre_length < 230) {
		decre_1 = 67;
		decre_2 = decre_length - 67;
	}
	else if(decre_length < 373) {
		decre_1 = 67;
		decre_2 = 163;
		decre_3 = decre_length - 230;
	}
	else if(decre_length < 536) {
		decre_1 = 67;
		decre_2 = 163;
		decre_3 = 143;
		decre_4 = decre_length - 373;
	}
	else if(decre_length <=  604) {
		decre_1 = 67;
		decre_2 = 163;
		decre_3 = 143;
		decre_4 = 163;
		decre_5 = decre_length - 536;
	}
	graphics_fill_rect(ctx, GRect(5, 0, 67 - decre_1, 5), 0, GCornerNone);
	graphics_fill_rect(ctx, GRect(0, decre_2, 5, 163 - decre_2), 0, GCornerNone);
	graphics_fill_rect(ctx, GRect(decre_3, 163, 139 - decre_3, 5), 0, GCornerNone);
	graphics_fill_rect(ctx, GRect(139, 5, 5, 163 - decre_4), 0, GCornerNone);
	graphics_fill_rect(ctx, GRect(72, 0, 72 - decre_5, 5), 0, GCornerNone);
	#endif
}

static void handle_bluetooth(bool connected) {
	if(bluetooth_connection_service_peek()) {
		bitmap_layer_set_bitmap(s_connection_layer, NULL);
	} else {
		vibes_double_pulse();
		bitmap_layer_set_bitmap(s_connection_layer, s_connection_bitmap);
	}
}

static void handle_battery(BatteryChargeState charge_state) {
		s_battery_level = charge_state.charge_percent;
}

void monitor_connection() {
	if(persist_read_bool(KEY_CONNECTION)) {
		handle_bluetooth(bluetooth_connection_service_peek());
		layer_add_child(s_window_layer, bitmap_layer_get_layer(s_connection_layer));
	}
}

void monitor_battery() {
	if(persist_read_bool(KEY_SHOW_BATTERY)) {
		handle_battery(battery_state_service_peek());
		layer_set_update_proc(s_battery_layer, battery_update_proc);
		layer_add_child(s_window_layer, s_battery_layer);
	}
}

void init_status(Layer *window_layer) {
	s_window_layer = window_layer;
  s_bounds = layer_get_bounds(window_layer);
  s_center = grect_center_point(&s_bounds);
	
	s_battery_layer = layer_create(s_bounds);
	s_connection_layer = bitmap_layer_create(PBL_IF_ROUND_ELSE(GRect(80, 115, 20, 36), GRect(62, 105, 20, 36)));
	s_connection_bitmap = gbitmap_create_with_resource(connection_icon);
}

void deinit_status() {
	layer_destroy(s_battery_layer);
	bitmap_layer_destroy(s_connection_layer);
	gbitmap_destroy(s_connection_bitmap);
}

void status_service_on() {
	bluetooth_connection_service_subscribe(handle_bluetooth);
	battery_state_service_subscribe(handle_battery);
}

void status_service_off() {
	bluetooth_connection_service_unsubscribe();
	battery_state_service_unsubscribe();
}