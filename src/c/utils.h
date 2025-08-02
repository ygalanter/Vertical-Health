#include <pebble.h>

static Window *s_window;

GRect bounds; GPoint center;

Layer *graphics_layer, *window_layer;

void draw_white_columns(GContext *ctx);

void clear_screen(GContext *ctx);

void draw_grey_columns(GContext *ctx, int percentages[5]);