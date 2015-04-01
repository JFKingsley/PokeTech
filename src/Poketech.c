#include <pebble.h>
#include "controllers/clock.h"

static void init(void) {
  clock_init();
}

static void deinit(void) {
  clock_deinit();
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed digital clock window");

  app_event_loop();
  deinit();
}
