#pragma once

#include <pebble.h>
#include "config.h"

Layer *g_window_layer;
GRect g_bounds;
GPoint g_center;
uint32_t g_connection_icon;

static BitmapLayer *s_connection_layer;
static GBitmap *s_connection_bitmap;

static void handle_bluetooth(bool connected) {
	if(bluetooth_connection_service_peek()) {
		bitmap_layer_set_bitmap(s_connection_layer, NULL);
	} else {
		vibes_double_pulse();
		bitmap_layer_set_bitmap(s_connection_layer, s_connection_bitmap);
	}
}

void display_connection() {
	if(persist_read_bool(KEY_CONNECTION)) {
		handle_bluetooth(bluetooth_connection_service_peek());
		layer_add_child(g_window_layer, bitmap_layer_get_layer(s_connection_layer));
	}
}

void handle_battery(BatteryChargeState charge_state) {
	//int battery_level = charge_state.charge_percent;
}

void init_status() {
	s_connection_layer = bitmap_layer_create(PBL_IF_ROUND_ELSE(GRect(80, 115, 20, 36), GRect(62, 105, 20, 36)));
	s_connection_bitmap = gbitmap_create_with_resource(g_connection_icon);
}

void status_service_on() {
	bluetooth_connection_service_subscribe(handle_bluetooth);
	battery_state_service_subscribe(handle_battery);
}

void status_service_off() {
	bluetooth_connection_service_unsubscribe();
	battery_state_service_unsubscribe();
}