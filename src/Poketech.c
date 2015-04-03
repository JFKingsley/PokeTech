#include <pebble.h>
#include "controllers/clock.h"
#include "controllers/settings.h"

static void init(void) {
  clock_init();
  settings_init();
}

static void deinit(void) {
  clock_deinit();
  settings_deinit();
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushing digital clock window...");

  app_event_loop();
  deinit();
}
