#pragma once

#define TOTAL_TIME_DIGITS 4
#define PIKACHU_HAT_KEY 10

void clock_init();
void clock_deinit();
void clock_show();

void update_hat(int new_hat);