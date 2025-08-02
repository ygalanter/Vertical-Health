#include <pebble.h>
#pragma once

static Window *s_window;

GRect bounds; GPoint center;

Layer *data_layer, *graphics_layer, *window_layer;

void draw_white_columns(GContext *ctx);

void clear_screen(GContext *ctx);

void draw_grey_columns(GContext *ctx, int percentages[5]);

void draw_text_lines(GContext *ctx, const char* lines[], int line_count);