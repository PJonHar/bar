#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define STATUS_BAR_HEIGHT 16
#define MY_UUID { 0x95, 0xD4, 0x0E, 0xBE, 0xAD, 0x89, 0x44, 0x6F, 0x81, 0x68, 0x04, 0x0D, 0xB5, 0x02, 0x7D, 0x1C }
PBL_APP_INFO(MY_UUID,
             "Bar Clock", "PJonHar",
             0, 3, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);
			 
Window window;
Layer bars_layer;
TextLayer text_hour_layer, text_min_layer, text_sec_layer, text_date_layer;
int i, iBarWidth, iBarHeight, iBarStart;
int iPointOne[2], iPointTwo[2], iPointThree[2], iPointFour;
PblTm current_time;

void bar_Calculations() {
    
    iBarStart=97;
    
    // Bar Dimensions
    iBarHeight=SCREEN_HEIGHT-(SCREEN_HEIGHT-iBarStart);
    iBarWidth=(SCREEN_WIDTH-20)/3;
    
    // Bar Width Start
    iPointOne[0] = 5;
    iPointTwo[0] = iPointOne[0] + iBarWidth + 5;
    iPointThree[0] = iPointOne[0] + iBarWidth + iPointTwo[0];
    
    // Bar Width End
    iPointOne[1] = iPointOne[0] + iBarWidth;
    iPointTwo[1] = iPointTwo[0] + iBarWidth;
    iPointThree[1] = iPointThree[0] + iBarWidth;
    
}

// Taken from Simplicity
void handle_tick(AppContextRef ctx, PebbleTickEvent *t) {
    (void)t;
    (void)ctx;
    
    // Need to be static because they're used by the system later.
    static char hour_text[] = "00";
	static char min_text[] = "00";
	static char sec_text[] = "00";
    static char date_text[] = "Xxxxxxxxx 00";
    
    // time
    string_format_time(hour_text, sizeof(hour_text), "%R", t->tick_time);
	string_format_time(min_text, sizeof(min_text), "%M", t->tick_time);
	string_format_time(sec_text, sizeof(sec_text), "%S", t->tick_time);
    
    // date
    string_format_time(date_text, sizeof(date_text), "%B %e", t->tick_time);
        
    text_layer_set_text(&text_hour_layer, hour_text);
	text_layer_set_text(&text_min_layer, min_text);
	text_layer_set_text(&text_sec_layer, sec_text);
	text_layer_set_text(&text_date_layer, date_text);
    layer_mark_dirty(&bars_layer);
}

void bars_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;
    
    //Get the current time
    get_time(&current_time);
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    // Hours
    for (i=0; i<((current_time.tm_hour*iBarHeight)/24); i++) {
        iPointFour=iBarStart-(i);
        graphics_draw_line(ctx, GPoint(iPointOne[0], iPointFour), GPoint(iPointOne[1], iPointFour));
    }
    
    // Minutes
    for (i=0; i<((current_time.tm_min*iBarHeight)/60); i++) {
        iPointFour=iBarStart-(i);
        graphics_draw_line(ctx, GPoint(iPointTwo[0], iPointFour), GPoint(iPointTwo[1], iPointFour));
    }
    
    // Seconds
    for (i=0; i<((current_time.tm_sec*iBarHeight)/60); i++) {
        iPointFour=iBarStart-(i);
        graphics_draw_line(ctx, GPoint(iPointThree[0], iPointFour), GPoint(iPointThree[1], iPointFour));
    }

}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
    
    resource_init_current_app(&APP_RESOURCES);
    
    bar_Calculations();
    
    // bars
    layer_init(&bars_layer, window.layer.frame);
    bars_layer.update_proc = &bars_layer_update_callback;
    layer_add_child(&window.layer, &bars_layer);
    
    // time
	// hour
    text_layer_init(&text_hour_layer, window.layer.frame);
    text_layer_set_text_color(&text_hour_layer, GColorWhite);
    text_layer_set_background_color(&text_hour_layer, GColorClear);
    layer_set_frame(&text_hour_layer.layer, GRect(iPointOne[0]+(iBarWidth/3)-2.5, 100, iPointOne[1], SCREEN_HEIGHT-100));
    text_layer_set_font(&text_hour_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    layer_add_child(&window.layer, &text_hour_layer.layer);

	// min
	text_layer_init(&text_min_layer, window.layer.frame);
    text_layer_set_text_color(&text_min_layer, GColorWhite);
    text_layer_set_background_color(&text_min_layer, GColorClear);
    layer_set_frame(&text_min_layer.layer, GRect(iPointTwo[0]+(iBarWidth/3)-2.5, 100, iPointTwo[1], SCREEN_HEIGHT-100));
    text_layer_set_font(&text_min_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    layer_add_child(&window.layer, &text_min_layer.layer);

	// sec
	text_layer_init(&text_sec_layer, window.layer.frame);
    text_layer_set_text_color(&text_sec_layer, GColorWhite);
    text_layer_set_background_color(&text_sec_layer, GColorClear);
    layer_set_frame(&text_sec_layer.layer, GRect(iPointThree[0]+(iBarWidth/3)-2.5, 100, iPointThree[1], SCREEN_HEIGHT-100));
    text_layer_set_font(&text_sec_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    layer_add_child(&window.layer, &text_sec_layer.layer);

	// date
    text_layer_init(&text_date_layer, window.layer.frame);
    text_layer_set_text_color(&text_date_layer, GColorWhite);
    text_layer_set_background_color(&text_date_layer, GColorClear);
    layer_set_frame(&text_date_layer.layer, GRect(iPointOne[0]+((iBarWidth/4)*3), 125, SCREEN_WIDTH-iPointOne[0]+(iBarWidth/2), SCREEN_HEIGHT-125));
    text_layer_set_font(&text_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    layer_add_child(&window.layer, &text_date_layer.layer);
    
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
      .tick_info = {
          .tick_handler = &handle_tick,
          .tick_units = SECOND_UNIT
      }
  };
  app_event_loop(params, &handlers);
}


