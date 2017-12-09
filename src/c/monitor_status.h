#pragma once

#include <pebble.h>
#include "effect_layer/effect_layer.h"
#include "config.h"

Layer *g_window_layer;
GRect g_bounds;
GPoint g_center;
uint32_t g_connection_icon;

static EffectLayer *s_effect_layer;
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
	#if defined(PBL_PLATFORM_APLITE)
	#else
	if (battery_state_service_peek().is_plugged && persist_read_bool(KEY_ROTATE)) {
		effect_layer_remove_effect(s_effect_layer);
		effect_layer_add_effect(s_effect_layer,effect_rotate_90_degrees,(void*)true);
	} else {
		effect_layer_remove_effect(s_effect_layer);	
	}
	#endif
}

void init_status() {
	s_connection_layer = bitmap_layer_create(PBL_IF_ROUND_ELSE(GRect(80, 115, 20, 36), GRect(62, 105, 20, 36)));
	s_connection_bitmap = gbitmap_create_with_resource(g_connection_icon);
	
	#if defined(PBL_PLATFORM_APLITE)
	#else
	s_effect_layer = effect_layer_create(GRect(0,(g_bounds.size.h-g_bounds.size.w)/2-1,g_bounds.size.w,g_bounds.size.w));
	#endif
}

void deinit_status() {
	bitmap_layer_destroy(s_connection_layer);
	gbitmap_destroy(s_connection_bitmap);
	
	#if defined(PBL_PLATFORM_APLITE)
	#else
	effect_layer_destroy(s_effect_layer);
	#endif
}

void valid_rot() {
	#if defined(PBL_PLATFORM_APLITE)
	#else
	handle_battery(battery_state_service_peek());
	layer_add_child(g_window_layer, effect_layer_get_layer(s_effect_layer));
	#endif
}

void status_service_on() {
	bluetooth_connection_service_subscribe(handle_bluetooth);
	battery_state_service_subscribe(handle_battery);
}

void status_service_off() {
	bluetooth_connection_service_unsubscribe();
	battery_state_service_unsubscribe();
}