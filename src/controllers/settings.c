#include <pebble.h>
#include "settings.h"

static Window *main_window;

static SimpleMenuLayer *menu_layer;
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem menu_items[NUM_MENU_ITEMS];
static SimpleMenuItem about_items[2];

static GBitmap *jon_headshot_bitmap;
static bool isWatchfaceAnalog;
static int current_hat;
const char *hatTypes[] = {"None","Fedora","Fez"};

static void menu_select_callback(int index, void *ctx) {
  //Handle the changes on that specific cell
  if(index == 0) {
    if(current_hat == 2) {
      current_hat = 0;
    } else {
      current_hat++;
    }

    menu_items[index].subtitle = hatTypes[current_hat];
    layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
  } else if(index == 1) {
    //Flip the current type and update the cell
    isWatchfaceAnalog = !isWatchfaceAnalog;
    menu_items[index].subtitle = (isWatchfaceAnalog ? "Analog" : "Digital");
    layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
  } else if(index == 2) {
    //Reset all settings to default
    persist_write_int(PIKACHU_HAT_KEY, 0);
    persist_write_bool(WATCH_TYPE_KEY, false);

    //Pop the top window stack (self) to finish reset
    window_stack_pop(true);
  }
}

static void update_text() {
  menu_items[0].subtitle = hatTypes[current_hat];
  menu_items[1].subtitle = (isWatchfaceAnalog ? "Analog" : "Digital");
  layer_mark_dirty(simple_menu_layer_get_layer(menu_layer));
}

static void main_window_load(Window *window) {
  jon_headshot_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_JON_HEADSHOT);

  //Add the top title
  menu_sections[0] = (SimpleMenuSection) {
    .title = "Settings",
    .num_items = NUM_MENU_ITEMS,
    .items = menu_items,
  };

  //Add all the settings cells
  menu_items[0] = (SimpleMenuItem) {
    .title = "Pikachu Hat",
    .subtitle = "None",
    .callback = menu_select_callback,
  };
  menu_items[1] = (SimpleMenuItem) {
    .title = "Watch Type",
    .subtitle = "Analog",
    .callback = menu_select_callback,
  };
  menu_items[2] = (SimpleMenuItem) {
    .title = "Reset Data",
    .subtitle = "Clear all settings",
    .callback = menu_select_callback
  };

  //Add the made by header and item's
  menu_sections[1] = (SimpleMenuSection) {
    .title = "Made By",
    .num_items = 2,
    .items = about_items,
  };

  about_items[0] = (SimpleMenuItem) {
    .title = "Jon Kingsley",
    .subtitle = "@JFKingsley",
    .icon = jon_headshot_bitmap
  };

  about_items[1] = (SimpleMenuItem) {
    .title = "Gabi Fisher",
    .subtitle = "@NotSure"
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  //Create the menu layer and add it to the window
  menu_layer = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));

  update_text();
}

void main_window_unload(Window *window) {
  simple_menu_layer_destroy(menu_layer);
  gbitmap_destroy(jon_headshot_bitmap);
}

void settings_init() {
  main_window = window_create();
  isWatchfaceAnalog = persist_read_bool(WATCH_TYPE_KEY);
  current_hat = 0;
  if (persist_exists(PIKACHU_HAT_KEY)) {
    // Load stored count
    current_hat = persist_read_int(PIKACHU_HAT_KEY);
  }

  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
}

void settings_deinit() {
  persist_write_bool(WATCH_TYPE_KEY, isWatchfaceAnalog);
  persist_write_int(PIKACHU_HAT_KEY, current_hat);
  window_destroy(main_window);
}

void settings_show() {
  window_stack_push(main_window, true);
}