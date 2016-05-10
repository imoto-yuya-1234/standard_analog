#include "pebble.h"

static Window *s_window;
static Layer *s_bg_layer, *s_ticks_layer;

static Layer *s_hands_layer;

static Layer *s_date_layer;
static TextLayer *s_day_label, *s_num_label;
static char s_num_buffer[4], s_day_buffer[6];

static BitmapLayer *s_connection_layer;
static GBitmap *s_connection_bitmap;

static Layer *s_battery_layer;
static int s_battery_level;

static void bg_update_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
  	
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, bounds, 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorWhite);
}

static void ticks_update_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
	
	int32_t angle;
	int16_t length, width;
	const int16_t radius = PBL_IF_ROUND_ELSE(bounds.size.w / 2, bounds.size.h / 2);
	for(int i = 0; i < 60; ++i) {
		angle = TRIG_MAX_ANGLE * i / 60;
		if(i % 5 == 0) {
			length = PBL_IF_ROUND_ELSE(8, 8);
			width = 3;
		} else {
			length = PBL_IF_ROUND_ELSE(4, 4);
			width = 1;
		}
		GPoint start = {
    	.x = (int16_t)(sin_lookup(angle) * (int32_t)radius / TRIG_MAX_RATIO) + center.x,
    	.y = (int16_t)(-cos_lookup(angle) * (int32_t)radius / TRIG_MAX_RATIO) + center.y,
  	};
		GPoint end = {
    	.x =  (int16_t)(sin_lookup(angle) * (int32_t)(radius - length) / TRIG_MAX_RATIO) + center.x,
    	.y =  (int16_t)(-cos_lookup(angle) * (int32_t)(radius - length) / TRIG_MAX_RATIO) + center.y,
  	};
		graphics_context_set_stroke_color(ctx, GColorWhite);
		graphics_context_set_stroke_width(ctx, width);
		graphics_draw_line(ctx, start, end);
	}
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
	
	// hour/minute/second hand
	time_t now = time(NULL);
  struct tm *t = localtime(&now);
	int16_t hour = t->tm_hour;
	int16_t min = t->tm_min;
	
	// hour hand
	const int16_t hour_hand_length = PBL_IF_ROUND_ELSE(bounds.size.w / 2 - 35, bounds.size.w / 2 - 20);
	int32_t hour_hand_angle = (TRIG_MAX_ANGLE * (((hour % 12) * 6) + (min / 10))) / (12 * 6);
	GPoint hour_hand_start = {
		.x = (int16_t)(sin_lookup(hour_hand_angle) * (int32_t)(hour_hand_length) / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_hand_angle) * (int32_t)(hour_hand_length) / TRIG_MAX_RATIO) + center.y,
  };
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_stroke_width(ctx, 3);
  graphics_draw_line(ctx, hour_hand_start, center);
	
	// minute hand
	const int16_t minute_hand_length = PBL_IF_ROUND_ELSE(bounds.size.w / 2 - 15, bounds.size.w / 2);
	int32_t minute_hand_angle = TRIG_MAX_ANGLE * min / 60;
	GPoint minute_hand_start = {
		.x = (int16_t)(sin_lookup(minute_hand_angle) * (int32_t)(minute_hand_length) / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_hand_angle) * (int32_t)(minute_hand_length) / TRIG_MAX_RATIO) + center.y,
  };
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_stroke_width(ctx, 3);
  graphics_draw_line(ctx, minute_hand_start, center);
		
	// dot in the middle
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, center, 4);
	
	// second hand
  const int16_t second_hand_length = PBL_IF_ROUND_ELSE(bounds.size.w / 2, bounds.size.w / 2);
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  GPoint second_hand_start = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)(second_hand_length - 3) / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)(second_hand_length - 3) / TRIG_MAX_RATIO) + center.y,
  };
	GPoint second_hand_end = {
    .x =  - (int16_t)(sin_lookup(180 + second_angle) * 15 / TRIG_MAX_RATIO) + center.x,
    .y =  - (int16_t)(-cos_lookup(180 + second_angle) * 15 / TRIG_MAX_RATIO) + center.y,
  };
  graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
	graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, second_hand_start, second_hand_end);
	// second hand decorate
	GPoint second_hand_deco = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)(second_hand_length - 8) / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)(second_hand_length - 8) / TRIG_MAX_RATIO) + center.y,
  };
	graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
	graphics_fill_circle(ctx, second_hand_deco, 5);

  // dot in the middle
  graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
  graphics_fill_circle(ctx, center, 2);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
	text_layer_set_text(s_day_label, s_day_buffer);
	
	strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
	text_layer_set_text(s_num_label, s_num_buffer);
	
	graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorChromeYellow, GColorLightGray));
	graphics_context_set_stroke_color(ctx , PBL_IF_COLOR_ELSE(GColorChromeYellow, GColorWhite));
	graphics_fill_rect(ctx, PBL_IF_ROUND_ELSE(
    GRect(103, 82, 54, 20),
    GRect(80, 75, 54, 20)), 3, GCornersAll);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int32_t angle_start = DEG_TO_TRIGANGLE(0);
	int32_t angle_end = DEG_TO_TRIGANGLE((int32_t)(360 * s_battery_level / 100));
	
	graphics_context_set_fill_color(ctx, GColorIslamicGreen);
	graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 8, angle_start, angle_end);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
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

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

	s_bg_layer = layer_create(bounds);
	layer_set_update_proc(s_bg_layer, bg_update_proc);
	layer_add_child(window_layer, s_bg_layer);
	
	s_battery_layer = layer_create(bounds);
	handle_battery(battery_state_service_peek());
	layer_set_update_proc(s_battery_layer, battery_update_proc);
	layer_add_child(window_layer, s_battery_layer);
	
	s_ticks_layer = layer_create(bounds);
	layer_set_update_proc(s_ticks_layer, ticks_update_proc);
	layer_add_child(window_layer, s_ticks_layer);
	
	s_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_NOT_CONNECTION_STATE);
	s_connection_layer = bitmap_layer_create(PBL_IF_ROUND_ELSE(GRect(80, 115, 20, 36), GRect(62, 105, 20, 36)));
	handle_bluetooth(bluetooth_connection_service_peek());
	layer_add_child(window_layer, bitmap_layer_get_layer(s_connection_layer));

	s_day_buffer[0] = '\0';
	s_num_buffer[0] = '\0';
	
	s_date_layer = layer_create(bounds);
	layer_set_update_proc(s_date_layer, date_update_proc);
	layer_add_child(window_layer, s_date_layer);
	
	s_day_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(104, 75, 30, 30),
    GRect(81, 68, 30, 30)));
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, GColorClear);
  text_layer_set_text_color(s_day_label, GColorBlack);
  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));

  s_num_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(136, 75, 20, 30),
    GRect(113, 68, 20, 30)));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorClear);
  text_layer_set_text_color(s_num_label, GColorBlack);
  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));
	
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
}

static void window_unload(Window *window) {
	layer_destroy(s_bg_layer);
	layer_destroy(s_ticks_layer);
  layer_destroy(s_hands_layer);
	layer_destroy(s_date_layer);
	layer_destroy(s_battery_layer);
	text_layer_destroy(s_day_label);
	text_layer_destroy(s_num_label);
	gbitmap_destroy(s_connection_bitmap);
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
	
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
	bluetooth_connection_service_subscribe(handle_bluetooth);
	battery_state_service_subscribe(handle_battery);
}

static void deinit() {
  tick_timer_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	battery_state_service_unsubscribe();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
