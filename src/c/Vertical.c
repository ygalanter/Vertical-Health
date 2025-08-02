#include "utils.h"

static void graphics_update_proc(Layer *layer, GContext *ctx) {

  clear_screen(ctx);

  draw_white_columns(ctx);

  draw_grey_columns(ctx, (int[]){ 40, 80, 30, 20, 70 });
  
}

static void prv_window_load(Window *window) {

}

static void prv_window_unload(Window *window) {
 
}

static void prv_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, false);

  window_layer = window_get_root_layer(s_window);
  bounds = layer_get_bounds(window_layer);
  center = grect_center_point(&bounds);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Window bounds: %d x %d", bounds.size.w, bounds.size.h);

  graphics_layer = layer_create(bounds);
  layer_set_update_proc(graphics_layer, graphics_update_proc);
  layer_add_child(window_layer, graphics_layer);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
