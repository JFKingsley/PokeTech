#include <pebble.h>
#include "clock.h"

static Window *main_window;

static TextLayer *text_layer;
static Layer *bg_layer;
static BitmapLayer *pikachu_icon_layer;
static GBitmap *pikachu_icon_bitmap = NULL;

static const uint32_t PIKACHU_ICONS[] = {
  RESOURCE_ID_IMAGE_PIKACHU, //0
  RESOURCE_ID_IMAGE_PIKACHU_MOUSTACHE //1
};

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
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
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(main_window);
  GRect bounds = layer_get_bounds(window_layer);

  //Draw all background coloring
  bg_layer = layer_create(bounds);
  layer_set_update_proc(bg_layer, bg_layer_draw);
  layer_add_child(window_layer, bg_layer);

  //Add the pikachu icon layer onto the background
  pikachu_icon_layer = bitmap_layer_create(GRect(5, (bounds.size.h - 44), 70, 43));
  layer_add_child(bg_layer, bitmap_layer_get_layer(pikachu_icon_layer));

  //Add the image to the later and handle transparency
  pikachu_icon_bitmap = gbitmap_create_with_resource(PIKACHU_ICONS[(USE_MOUSTACHE ? 1 : 0)]);
  bitmap_layer_set_bitmap(pikachu_icon_layer, pikachu_icon_bitmap);
  bitmap_layer_set_compositing_mode(pikachu_icon_layer, GCompOpSet);

  //Draw the clock text, soon
  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(bg_layer, text_layer_get_layer(text_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void clock_init() {
  main_window = window_create();

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