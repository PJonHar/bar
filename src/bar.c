#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x95, 0xD4, 0x0E, 0xBE, 0xAD, 0x89, 0x44, 0x6F, 0x81, 0x68, 0x04, 0x0D, 0xB5, 0x02, 0x7D, 0x1C }
PBL_APP_INFO(MY_UUID,
             "Bar Clock", "PJonHar",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
Layer bars_layer;
TextLayer text_time_layer;
static char time_text[] = "00:00";
static int iScreenWidth=144;
static int iScreenHeight=168;
//static int iStatusBarHeight=16;


void handle_tick(AppContextRef ctx, PebbleTickEvent *t) {
    (void)t;
    (void)ctx;
    
    // Need to be static because they're used by the system later.
    static char time_text[] = "00:00";
    //static char date_text[] = "Xxxxxxxxx 00";
    
    char *time_format;
    
    // TODO: Only update the date when it's changed.
    // string_format_time(date_text, sizeof(date_text), "%B %e", t->tick_time);
    //text_layer_set_text(&text_date_layer, date_text);
    
    
    if (clock_is_24h_style()) {
        time_format = "%R";
    } else {
        time_format = "%I:%M";
    }
    
    string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);
    
    // Kludge to handle lack of non-padded hour format string
    // for twelve hour clock.
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
        memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }
    
    text_layer_set_text(&text_time_layer, time_text);
    
    layer_mark_dirty(&bars_layer);
}

void bars_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;

    PblTm time;
    int i, iBarWidth, iBarHeight, iBarStart;
    int iPointOne[2], iPointTwo[2], iPointThree[2], iPointFour;
    
    iBarStart=97;
    
    // Bar Dimensions
    iBarHeight=iScreenHeight-(iScreenHeight-iBarStart);
    iBarWidth=(iScreenWidth-20)/3;
    
    // Bar Width Start
    iPointOne[0] = 5;
    iPointTwo[0] = iPointOne[0] + iBarWidth + 5;
    iPointThree[0] = iPointOne[0] + iBarWidth + iPointTwo[0];
    
    // Bar Width End
    iPointOne[1] = iPointOne[0] + iBarWidth;
    iPointTwo[1] = iPointTwo[0] + iBarWidth;
    iPointThree[1] = iPointThree[0] + iBarWidth;
    
    //Get the current time
    get_time(&time);
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    // Hours
    for (i=0; i<((time.tm_hour*iBarHeight)/24); i++) {
        iPointFour=iBarStart-(i);
        graphics_draw_line(ctx, GPoint(iPointOne[0], iPointFour), GPoint(iPointOne[1], iPointFour));
    }
    
    // Minutes
    for (i=0; i<((time.tm_min*iBarHeight)/60); i++) {
        iPointFour=iBarStart-(i);
        graphics_draw_line(ctx, GPoint(iPointTwo[0], iPointFour), GPoint(iPointTwo[1], iPointFour));
    }
    
    // Seconds
    for (i=0; i<((time.tm_sec*iBarHeight)/60); i++) {
        iPointFour=iBarStart-(i);
        graphics_draw_line(ctx, GPoint(iPointThree[0], iPointFour), GPoint(iPointThree[1], iPointFour));
    }

}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
        
    // time
    text_layer_init(&text_time_layer, window.layer.frame);
    text_layer_set_text_color(&text_time_layer, GColorWhite);
    text_layer_set_background_color(&text_time_layer, GColorClear);
    layer_set_frame(&text_time_layer.layer, GRect(7, 120, iScreenWidth-7, iScreenHeight-120));
    text_layer_set_font(&text_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    layer_add_child(&window.layer, &text_time_layer.layer);
    
    // bars
    layer_init(&bars_layer, window.layer.frame);
    bars_layer.update_proc = &bars_layer_update_callback;
    layer_add_child(&window.layer, &bars_layer);
    
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


