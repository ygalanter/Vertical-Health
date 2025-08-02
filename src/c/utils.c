#include "utils.h"


void clear_screen(GContext *ctx) {
  // Set drawing color to black
  graphics_context_set_fill_color(ctx, GColorBlack);
  
  // Fill the entire bounds with black
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

void draw_white_columns(GContext *ctx) {
// Set drawing color to white
  graphics_context_set_fill_color(ctx, GColorWhite);

  // Calculate column width and spacing
  // We have 5 columns and 6 spaces (before first, between columns, after last)
  int total_width = bounds.size.w;
  int column_width = total_width / 7;  // 5 columns + 3 spaces between = 8 units
  int spacing = column_width/2.5;
  
  // Draw 5 columns
  for (int i = 0; i < 5; i++) {
    int x = spacing + i * (column_width + spacing) - 2;
    GRect column_rect = GRect(x, 0, column_width, total_width *3/4);
    graphics_fill_rect(ctx, column_rect, 0, GCornerNone);
  }
}


void draw_grey_columns(GContext *ctx, int percentages[5]) {
  // Set drawing color to grey
  graphics_context_set_fill_color(ctx, GColorLightGray);

  // Use same calculation as white columns for consistency
  int total_width = bounds.size.w;
  int column_width = total_width / 7;
  int spacing = column_width/2.5;
  int white_column_height = total_width * 3/4;

  // Draw 5 grey columns based on percentages
  for (int i = 0; i < 5; i++) {
    int x = spacing + i * (column_width + spacing) - 2;
    
    // Calculate grey column height as percentage of white column height
    int grey_height = (white_column_height * percentages[i]) / 100;
    
    // Position grey column at bottom of white column
    int y = white_column_height - grey_height;
    
    GRect grey_rect = GRect(x, y, column_width, grey_height);
    graphics_fill_rect(ctx, grey_rect, 0, GCornerNone);
  }
}