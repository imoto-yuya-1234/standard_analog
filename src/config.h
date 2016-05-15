#include <pebble.h>

#define KEY_INVERT        0
#define KEY_CONNECTION    1
#define KEY_SHOW_TICKS    2
#define KEY_SHOW_DATE     3
#define KEY_SHOW_SECOND   4
#define KEY_SHOW_BATTERY  5

static void reload_window();

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
	
	Tuple *show_date_t = dict_find(iter, KEY_SHOW_DATE);
	if(show_date_t && show_date_t->value->int8 > 0) {
		persist_write_bool(KEY_SHOW_DATE, true);
	} 
	else {
		persist_write_bool(KEY_SHOW_DATE, false);
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
		
	reload_window();
}