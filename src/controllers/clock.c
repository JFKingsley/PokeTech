#include <pebble.h>
#include "clock.h"
#include "settings.h"

static Window *main_window;

static TextLayer *text_layer;
static Layer *bg_layer;
static int current_hat;

static BitmapLayer *pikachu_icon_layer, *pikachu_hat_layer, *time_digits_layers[TOTAL_TIME_DIGITS];
static GBitmap *pikachu_icon_bitmap, *time_digits[TOTAL_TIME_DIGITS];

static const uint32_t PIKACHU_HATS[] = {
  RESOURCE_ID_IMAGE_PIKACHU_HAT_FEDORA, //0
  RESOURCE_ID_IMAGE_PIKACHU_HAT_FEZ //1
};

static const uint32_t CLOCK_NUMBERS[] = {
  RESOURCE_ID_IMAGE_CLOCK_AM, //0
  RESOURCE_ID_IMAGE_CLOCK_PM, //1
  RESOURCE_ID_IMAGE_CLOCK_COLON, //2
  RESOURCE_ID_IMAGE_CLOCK_ONE, //3
  RESOURCE_ID_IMAGE_CLOCK_TWO, //4
  RESOURCE_ID_IMAGE_CLOCK_THREE, //5
  RESOURCE_ID_IMAGE_CLOCK_FOUR, //6
  RESOURCE_ID_IMAGE_CLOCK_FIVE, //7
  RESOURCE_ID_IMAGE_CLOCK_SIX, //8
  RESOURCE_ID_IMAGE_CLOCK_SEVEN, //9
  RESOURCE_ID_IMAGE_CLOCK_EIGHT, //10
  RESOURCE_ID_IMAGE_CLOCK_NINE, //11
  RESOURCE_ID_IMAGE_CLOCK_ZERO, //12
};

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  settings_show();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);

  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, NULL);
}

void update_hat(int new_hat) {
  if(new_hat > 0) {
    layer_set_hidden(bitmap_layer_get_layer(pikachu_hat_layer), false);
    current_hat = new_hat;
    layer_mark_dirty(bitmap_layer_get_layer(pikachu_hat_layer));

  } else {
    layer_set_hidden(bitmap_layer_get_layer(pikachu_hat_layer), true);
  }
}

static void bg_layer_draw(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  // Draw the status bar and darker bottom lines
  graphics_context_set_fill_color(ctx, GColorMidnightGreen);

  //Status bar
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.h, 12), 0, GCornerNone);

  //Bottom bars
  graphics_fill_rect(ctx, GRect(0, (bounds.size.h - 7), bounds.size.w, 7), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(0, (bounds.size.h - 15), bounds.size.w, 4), 0, GCornerNone);

  //Add the pikachu icon layer onto the background
  pikachu_icon_layer = bitmap_layer_create(GRect(5, (bounds.size.h - 44), 70, 43));
  layer_add_child(layer, bitmap_layer_get_layer(pikachu_icon_layer));

  //Add the image to the later and handle transparency
  bitmap_layer_set_bitmap(pikachu_icon_layer, pikachu_icon_bitmap);
  bitmap_layer_set_compositing_mode(pikachu_icon_layer, GCompOpSet);

  //Add the pikachu hat layer onto the background
  pikachu_hat_layer = bitmap_layer_create(GRect(35, (bounds.size.h - 51), 40, 20));
  layer_add_child(layer, bitmap_layer_get_layer(pikachu_hat_layer));
  bitmap_layer_set_compositing_mode(pikachu_hat_layer, GCompOpSet);

  if(current_hat > 0) {
    //Add the image to the later and handle transparency
    bitmap_layer_set_bitmap(pikachu_hat_layer, gbitmap_create_with_resource(PIKACHU_HATS[current_hat - 1]));
  }
}

static void clock_draw(Layer *layer) {
  time_digits_layers[0] = NULL;
  time_digits[0] = NULL;
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(main_window);
  GRect bounds = layer_get_bounds(window_layer);
  current_hat = 0;
  if (persist_exists(PIKACHU_HAT_KEY)) {
    // Load stored count
    current_hat = persist_read_int(PIKACHU_HAT_KEY);
  }

  //Draw all background coloring
  bg_layer = layer_create(bounds);
  layer_set_update_proc(bg_layer, bg_layer_draw);
  layer_add_child(window_layer, bg_layer);

  //Draw the clock text, soon
  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(bg_layer, text_layer_get_layer(text_layer));

  clock_draw(bg_layer);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
  gbitmap_destroy(pikachu_icon_bitmap);
}

void clock_init() {
  main_window = window_create();
  pikachu_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PIKACHU);
  current_hat = 0;

  //Setup the window click handlers, background color and winow handlers
  window_set_click_config_provider(main_window, click_config_provider);
  window_set_background_color(main_window, GColorCadetBlue);
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });

  window_stack_push(main_window, true);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Digital clock window fully built and pushed.");
}

void clock_deinit() {
  window_destroy(main_window);
}

void clock_show() {
  window_stack_push(main_window, true);
}