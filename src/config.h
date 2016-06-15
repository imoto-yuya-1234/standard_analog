#pragma once

#include <pebble.h>

#define KEY_INIT					0
#define KEY_INVERT        1
#define KEY_CONNECTION    2
#define KEY_SHOW_TICKS    3
#define KEY_SHOW_DAY     	4
#define KEY_SHOW_SECOND   5
#define KEY_SHOW_BATTERY  6
#define KEY_LANG					7

extern uint32_t g_connection_icon;
extern GColor g_bg_color, g_ticks_color, g_minute_color, g_second_color;

void reload_window();

static void in_recv_handler(DictionaryIterator *iter, void *context) {
	Tuple *invert_t = dict_find(iter, KEY_INVERT);
  if(invert_t && invert_t->value->int8 > 0) {  // Read boolean as an integer
    // Persist value
    persist_write_bool(KEY_INVERT, true);
  } 
	else {
    persist_write_bool(KEY_INVERT, false);
  }
	
	Tuple *connection_t = dict_find(iter, KEY_CONNECTION);
  if(connection_t && connection_t->value->int8 > 0) {  // Read boolean as an integer
    // Persist value
    persist_write_bool(KEY_CONNECTION, true);
  } 
	else {
    persist_write_bool(KEY_CONNECTION, false);
  }
	
	Tuple *show_ticks_t = dict_find(iter, KEY_SHOW_TICKS);
	if(show_ticks_t && show_ticks_t->value->int8 > 0) {
		persist_write_bool(KEY_SHOW_TICKS, true);
	} 
	else {
		persist_write_bool(KEY_SHOW_TICKS, false);
	}
	
	Tuple *show_day_t = dict_find(iter, KEY_SHOW_DAY);
	if(show_day_t && show_day_t->value->int8 > 0) {
		persist_write_bool(KEY_SHOW_DAY, true);
	} 
	else {
		persist_write_bool(KEY_SHOW_DAY, false);
	}
	
	Tuple *show_second_t = dict_find(iter, KEY_SHOW_SECOND);
	if(show_second_t && show_second_t->value->int8 > 0) {
		persist_write_bool(KEY_SHOW_SECOND, true);
	} 
	else {
		persist_write_bool(KEY_SHOW_SECOND, false);
	}
	
	Tuple *show_battery_t = dict_find(iter, KEY_SHOW_BATTERY);
	if(show_battery_t && show_battery_t->value->int8 > 0) {
		persist_write_bool(KEY_SHOW_BATTERY, true);
	} 
	else {
		persist_write_bool(KEY_SHOW_BATTERY, false);
	}
	
	Tuple *lang_t = dict_find(iter, KEY_LANG);
	persist_write_string(KEY_LANG, lang_t->value->cstring);
		
	reload_window();
}

void initialize_value() {
	if(!persist_read_bool(KEY_INIT)) {
		persist_write_bool(KEY_INIT, true);
		persist_write_bool(KEY_INVERT, true);
		persist_write_bool(KEY_CONNECTION, true);
		persist_write_bool(KEY_SHOW_TICKS, true);
		persist_write_bool(KEY_SHOW_DAY, true);
		persist_write_bool(KEY_SHOW_SECOND, true);
		persist_write_bool(KEY_SHOW_BATTERY, false);
		persist_write_string(KEY_LANG, "en");
	}
	
	if(persist_read_bool(KEY_INVERT)) {
		g_connection_icon = RESOURCE_ID_NOT_CONNECTION_STATE_BLACK;
		g_bg_color = GColorBlack;
		g_ticks_color = GColorWhite;
		g_minute_color = GColorWhite;
		g_second_color = GColorWhite;
	}
	else {
		g_connection_icon = RESOURCE_ID_NOT_CONNECTION_STATE_WHITE;
		g_bg_color = GColorWhite;
		g_ticks_color = GColorBlack;
		g_minute_color = GColorBlack;
		g_second_color = GColorBlack;
	}
}

void config_service() {
	app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}