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

  // Calculate bar height and spacing (rotated from column logic)
  // We have 5 bars and 6 spaces (before first, between bars, after last)
  int total_height = bounds.size.w;
  int bar_height = total_height / 7;  // 5 bars + 3 spaces between = 8 units
  int spacing = bar_height/2.5;
  
  // Draw 5 horizontal bars
  for (int i = 0; i < 5; i++) {
    int y = spacing + i * (bar_height + spacing) - 2;
    int bar_width = bounds.size.w * 3/4;
    int left_margin = bounds.size.w - bar_width;  // Gap on the left
    GRect bar_rect = GRect(left_margin, y, bar_width, bar_height);
    graphics_fill_rect(ctx, bar_rect, 0, GCornerNone);
  }
}


void draw_grey_columns(GContext *ctx, int percentages[5]) {
  // Set drawing color to grey
  graphics_context_set_fill_color(ctx, GColorLightGray);

  // Use same calculation as white bars for consistency
  int total_height = bounds.size.w;
  int bar_height = total_height / 7;
  int spacing = bar_height/2.5;
  int white_bar_width = bounds.size.w * 3/4;

  // Draw 5 grey bars based on percentages
  for (int i = 0; i < 5; i++) {
    int y = spacing + i * (bar_height + spacing) - 2;
    
    // Calculate grey bar width as percentage of white bar width
    int grey_width = (white_bar_width * percentages[i]) / 100;
    
    // Position grey bar at left edge of white bar (left-aligned within the white bar)
    int left_margin = bounds.size.w - white_bar_width;  // Same left margin as white bar
    int x = left_margin;
    
    GRect grey_rect = GRect(x, y, grey_width, bar_height);
    graphics_fill_rect(ctx, grey_rect, 0, GCornerNone);
  }
}

void draw_text_lines(GContext *ctx, const char* lines[], int line_count) {
  // Set text color to white
  graphics_context_set_text_color(ctx, GColorBlack);
  
  // Use a system font
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  
  // Use similar calculations as bars for consistency
  int total_width = bounds.size.w;  // Add this variable
  int total_height = bounds.size.w;  // Match the bar calculation
  int bar_height = total_height / 7;
  int spacing = bar_height/2.5;
  
  // Draw each line
  for (int i = 0; i < line_count; i++) {
    int bar_y = spacing + i * (bar_height + spacing) - 2;  // Match bar positioning exactly
    
    // Calculate text height and center it vertically within the bar
    GSize text_size = graphics_text_layout_get_content_size("A", font, GRect(0, 0, 100, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    int text_height = text_size.h;
    int y_offset = (bar_height - text_height) / 2;  // Center vertically
    int y = bar_y - y_offset - 2;  // Subtract offset to move up + 2 more pixels
    
    GRect text_rect = GRect(
      bounds.size.w / 4 + 5,  // Left margin to match the bar gap + extra spacing
      y,
      total_width * 3/4 - 5,  // Width matches the bar width minus the extra spacing
      text_height  // Use actual text height
    );
    
    graphics_draw_text(ctx, lines[i], font, text_rect, 
                      GTextOverflowModeTrailingEllipsis, 
                      GTextAlignmentLeft, NULL);
  }
}