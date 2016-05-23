#include <pebble.h>

#include "config.h"
#include "color.h"

static Layer *s_window_layer;
static Layer *s_ticks_layer;
GColor ticks_color;
static GRect s_bounds;
static GPoint s_center;

static void ticks_update_proc(Layer *layer, GContext *ctx) {
	int32_t angle;
	int16_t length, width;
	#if defined(PBL_ROUND) 
	const int16_t radius = s_bounds.size.w / 2;
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
    	.x = (int16_t)(sin_lookup(angle) * (int32_t)radius / TRIG_MAX_RATIO) + s_center.x,
    	.y = (int16_t)(-cos_lookup(angle) * (int32_t)radius / TRIG_MAX_RATIO) + s_center.y,
  	};
		GPoint end = {
    	.x =  (int16_t)(sin_lookup(angle) * (int32_t)(radius - length) / TRIG_MAX_RATIO) + s_center.x,
    	.y =  (int16_t)(-cos_lookup(angle) * (int32_t)(radius - length) / TRIG_MAX_RATIO) + s_center.y,
  	};
		graphics_context_set_stroke_color(ctx, ticks_color);
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
		graphics_context_set_stroke_color(ctx, ticks_color);
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
		layer_add_child(s_window_layer, s_ticks_layer);
	}
}

void init_ornament(Layer *window_layer) {
	s_window_layer = window_layer;
  s_bounds = layer_get_bounds(window_layer);
  s_center = grect_center_point(&s_bounds);
	
	s_ticks_layer = layer_create(s_bounds);
}

void deinit_ornament() {
	layer_destroy(s_ticks_layer);
}