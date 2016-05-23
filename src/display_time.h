#pragma once

#include <pebble.h>
#include "config.h"
#include "color.h"

static Layer *s_window_layer;
static GRect s_bounds;
static GPoint s_center;

static Layer *s_hands_layer, *s_second_layer;
static TimeUnits s_event_time;

GColor minute_color, second_color;

static void hands_update_proc(Layer *layer, GContext *ctx) {
	// hour/minute hand
	time_t now = time(NULL);
  struct tm *t = localtime(&now);
	int16_t hour = t->tm_hour;
	int16_t min = t->tm_min;
	graphics_context_set_stroke_color(ctx, minute_color);
	
	// hour hand
	const int16_t hour_hand_length = PBL_IF_ROUND_ELSE(s_bounds.size.w / 2 - 30, s_bounds.size.w / 2 - 20);
	int32_t hour_hand_angle = (TRIG_MAX_ANGLE * (((hour % 12) * 6) + (min / 10))) / (12 * 6);
	GPoint hour_hand_start = {
		.x = (int16_t)(sin_lookup(hour_hand_angle) * (int32_t)(hour_hand_length) / TRIG_MAX_RATIO) + s_center.x,
    .y = (int16_t)(-cos_lookup(hour_hand_angle) * (int32_t)(hour_hand_length) / TRIG_MAX_RATIO) + s_center.y,
  };
	graphics_context_set_stroke_width(ctx, 3);
  graphics_draw_line(ctx, hour_hand_start, s_center);
	
	// minute hand
	const int16_t minute_hand_length = PBL_IF_ROUND_ELSE(s_bounds.size.w / 2 - 15, s_bounds.size.w / 2 - 5);
	int32_t minute_hand_angle = TRIG_MAX_ANGLE * min / 60;
	GPoint minute_hand_start = {
		.x = (int16_t)(sin_lookup(minute_hand_angle) * (int32_t)(minute_hand_length) / TRIG_MAX_RATIO) + s_center.x,
    .y = (int16_t)(-cos_lookup(minute_hand_angle) * (int32_t)(minute_hand_length) / TRIG_MAX_RATIO) + s_center.y,
  };
	graphics_context_set_stroke_width(ctx, 3);
  graphics_draw_line(ctx, minute_hand_start, s_center);
		
	// dot in the middle
	graphics_context_set_fill_color(ctx, minute_color);
	graphics_fill_circle(ctx, s_center, 4);
}

static void second_update_proc(Layer *layer, GContext *ctx) {
	time_t now = time(NULL);
  struct tm *t = localtime(&now);
	graphics_context_set_stroke_color(ctx, second_color);
	
	// second hand
  const int16_t second_hand_length = PBL_IF_ROUND_ELSE(s_bounds.size.w / 2, s_bounds.size.w / 2);
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
	graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, second_color));
  GPoint second_hand_start = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)(second_hand_length - 5) / TRIG_MAX_RATIO) + s_center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)(second_hand_length - 5) / TRIG_MAX_RATIO) + s_center.y,
  };
	GPoint second_hand_end = {
    .x =  - (int16_t)(sin_lookup(180 + second_angle) * 15 / TRIG_MAX_RATIO) + s_center.x,
    .y =  - (int16_t)(-cos_lookup(180 + second_angle) * 15 / TRIG_MAX_RATIO) + s_center.y,
  };
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, second_hand_start, second_hand_end);
	
	// second hand decorate
	GPoint second_hand_deco = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)(second_hand_length - 8) / TRIG_MAX_RATIO) + s_center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)(second_hand_length - 8) / TRIG_MAX_RATIO) + s_center.y,
  };
	graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, second_color));
	graphics_fill_circle(ctx, second_hand_deco, 5);
	
  // dot in the middle
  graphics_fill_circle(ctx, s_center, 2);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_window_layer);
}

void display_time() {
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(s_window_layer, s_hands_layer);
	
	if(persist_read_bool(KEY_SHOW_SECOND)) {
		s_event_time = SECOND_UNIT;
		layer_set_update_proc(s_second_layer, second_update_proc);
		layer_add_child(s_window_layer, s_second_layer);
	}
	else {
		s_event_time = MINUTE_UNIT;
	}
}

void init_time(Layer *window_layer) {
	s_window_layer = window_layer;
  s_bounds = layer_get_bounds(window_layer);
  s_center = grect_center_point(&s_bounds);
	
	s_hands_layer = layer_create(s_bounds);
	s_second_layer = layer_create(s_bounds);
}

void deinit_time() {
  layer_destroy(s_hands_layer);
	layer_destroy(s_second_layer);
}

void time_service_on() {
  tick_timer_service_subscribe(s_event_time, handle_second_tick);
}

void time_service_off() {
  tick_timer_service_unsubscribe();
}