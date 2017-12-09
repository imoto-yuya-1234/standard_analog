#include <pebble.h>
#include "config.h"
#include "monitor_status.h"

Layer *g_window_layer;
GRect g_bounds;
GPoint g_center;
GColor g_ticks_color;

static int s_battery_level;
static Layer *s_ticks_layer, *s_edge_layer;

static void ticks_update_proc(Layer *layer, GContext *ctx) {
	int32_t angle;
	int16_t length, width;
	#if defined(PBL_ROUND) 
	const int16_t radius = g_bounds.size.w / 2;
	for(int i = 0; i < 60; ++i) {
		angle = TRIG_MAX_ANGLE * i / 60;
		if(i % 5 == 0) {
			length = 8;
			width = 3;
		} else {
			length = 4;
			width = 1;
		}
		GPoint start = {
    	.x = (int16_t)(sin_lookup(angle) * (int32_t)radius / TRIG_MAX_RATIO) + g_center.x,
    	.y = (int16_t)(-cos_lookup(angle) * (int32_t)radius / TRIG_MAX_RATIO) + g_center.y,
  	};
		GPoint end = {
    	.x =  (int16_t)(sin_lookup(angle) * (int32_t)(radius - length) / TRIG_MAX_RATIO) + g_center.x,
    	.y =  (int16_t)(-cos_lookup(angle) * (int32_t)(radius - length) / TRIG_MAX_RATIO) + g_center.y,
  	};
		graphics_context_set_stroke_color(ctx, g_ticks_color);
		graphics_context_set_stroke_width(ctx, width);
		graphics_draw_line(ctx, start, end);
	}
	#elif defined(PBL_RECT)
	for(int j = 0; j < 60; ++j) {
		angle = TRIG_MAX_ANGLE * j / 60;
		if(j % 5 == 0) {
			length = 8;
			width = 3;
		} else {
			length = 4;
			width = 1;
		}
		graphics_context_set_stroke_color(ctx, g_ticks_color);
		graphics_context_set_stroke_width(ctx, width);
		if(j < 7 || j >= 54) {
			GPoint start = {
				.x = 72 + (int16_t)(84 * sin_lookup(angle) / cos_lookup(angle)), 
				.y = 0,
			};
			GPoint end = {
				.x = 72 + (int16_t)((84 - length) * sin_lookup(angle) / cos_lookup(angle)), 
				.y = length,
			};
			graphics_draw_line(ctx, start, end);
		}
		else if(j < 24) {
			GPoint start = {
				.x = 144, 
				.y = 84 - (int16_t)(72 * cos_lookup(angle) / sin_lookup(angle)),
			};
			GPoint end = {
				.x = 144 - length, 
				.y = 84 - (int16_t)((72 - length) * cos_lookup(angle) / sin_lookup(angle)),
			};
			graphics_draw_line(ctx, start, end);
		}
		else if(j < 37) {
			GPoint start = {
				.x = 72 - (int16_t)(84 * sin_lookup(angle) / cos_lookup(angle)), 
				.y = 168,
			};
			GPoint end = {
				.x = 72 - (int16_t)((84 - length) * sin_lookup(angle) / cos_lookup(angle)), 
				.y = 168 - length,
			};
			graphics_draw_line(ctx, start, end);
		}
		else if(j < 54) {
			GPoint start = {
				.x = 0, 
				.y = 84 + (int16_t)(72 * cos_lookup(angle) / sin_lookup(angle)),
			};
			GPoint end = {
				.x = length, 
				.y = 84 + (int16_t)((72 - length) * cos_lookup(angle) / sin_lookup(angle)),
			};
			graphics_draw_line(ctx, start, end);
		}
	}
	#endif
}

void display_ticks() {
	if(persist_read_bool(KEY_SHOW_TICKS)) {
		layer_set_update_proc(s_ticks_layer, ticks_update_proc);
		layer_add_child(g_window_layer, s_ticks_layer);
	}
}

static void edge_update_proc(Layer *layer, GContext *ctx) {
	if (battery_state_service_peek().is_plugged) {
		s_battery_level = 100;
	} else {
		s_battery_level = battery_state_service_peek().charge_percent;
	}
	#if defined(PBL_ROUND) 
	int32_t angle_start = DEG_TO_TRIGANGLE(0);
	int32_t angle_end = DEG_TO_TRIGANGLE((int32_t)(360 * s_battery_level / 100));
	graphics_context_set_fill_color(ctx, GColorIslamicGreen);
	graphics_fill_radial(ctx, g_bounds, GOvalScaleModeFitCircle, 8, angle_start, angle_end);
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

void display_edge() {
	if(persist_read_bool(KEY_SHOW_BATTERY)) {
		layer_set_update_proc(s_edge_layer, edge_update_proc);
		layer_add_child(g_window_layer, s_edge_layer);
	}
}

void init_ornament() {
	s_ticks_layer = layer_create(g_bounds);
	s_edge_layer = layer_create(g_bounds);
}

void deinit_ornament() {
	layer_destroy(s_ticks_layer);
	layer_destroy(s_edge_layer);
}