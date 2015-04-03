#include <pebble.h>
#include "settings.h"

static Window *main_window;

static SimpleMenuLayer *menu_layer;
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem menu_items[NUM_MENU_ITEMS];

static void menu_select_callback(int index, void *ctx) {
  //Handle the changes on that specific cell
  menu_items[index].subtitle = "You've hit select here!";
  layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
}

static void main_window_load(Window *window) {

  //Add the top title
  menu_sections[0] = (SimpleMenuSection) {
    .title = "Settings",
    .num_items = NUM_MENU_ITEMS,
    .items = menu_items,
  };

  //Add all the settings cells
  menu_items[0] = (SimpleMenuItem) {
    .title = "First Item",
    .callback = menu_select_callback,
  };
  menu_items[1] = (SimpleMenuItem) {
    .title = "Second Item",
    .subtitle = "Here's a subtitle",
    .callback = menu_select_callback,
  };
  menu_items[2] = (SimpleMenuItem) {
    .title = "Third Item",
    .subtitle = "This has an icon",
    .callback = menu_select_callback
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  //Create the menu layer and add it to the window
  menu_layer = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));
}

void main_window_unload(Window *window) {
  simple_menu_layer_destroy(menu_layer);
}

void settings_init() {
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
}

void settings_deinit() {
  window_destroy(main_window);
}

void settings_show() {
  window_stack_push(main_window, true);
}