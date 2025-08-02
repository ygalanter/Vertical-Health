#include "utils.h"
#include "pebble-effect-layer/pebble-effect-layer.h"

EffectLayer *effect_data_layer;
TextLayer *time_layer;

char s_time[] = "00:00";

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  char format[5];

  // building format 12h/24h
  if (clock_is_24h_style())
  {
    strcpy(format, "%H:%M"); // e.g "14:46"
  }
  else
  {
    strcpy(format, "%l:%M"); // e.g " 2:46" -- with leading space
  }

  strftime(s_time, sizeof(s_time), format, tick_time);

  if (s_time[0] == ' ')
  { // if in 12h mode we have leading space in time - don't display it (it will screw centering of text) start with next char
    text_layer_set_text(time_layer, &s_time[1]);
  }
  else
  {
    text_layer_set_text(time_layer, s_time);
  }
}

static void graphics_update_proc(Layer *layer, GContext *ctx)
{

  clear_screen(ctx);

  draw_white_columns(ctx);

  draw_grey_columns(ctx, (int[]){40, 80, 30, 20, 70});
}

static void data_update_proc(Layer *layer, GContext *ctx)
{

  const char *text_lines[] = {
      "Line One",
      "Line Two",
      "Line Three",
      "Line Four",
      "Line Five"};

  draw_text_lines(ctx, text_lines, 5);
}

static void prv_window_load(Window *window)
{
}

static void prv_window_unload(Window *window)
{
}

static void prv_init(void)
{
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = prv_window_load,
                                           .unload = prv_window_unload,
                                       });
  window_stack_push(s_window, false);

  window_layer = window_get_root_layer(s_window);
  bounds = layer_get_bounds(window_layer);
  center = grect_center_point(&bounds);

  graphics_layer = layer_create(bounds);
  layer_set_update_proc(graphics_layer, graphics_update_proc);
  layer_add_child(window_layer, graphics_layer);

  data_layer = layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.w));
  layer_set_update_proc(data_layer, data_update_proc);
  layer_add_child(window_layer, data_layer);

  effect_data_layer = effect_layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.w));
  effect_layer_add_effect(effect_data_layer, effect_rotate_90_degrees, (void *)0);
  layer_add_child(data_layer, effect_layer_get_layer(effect_data_layer));

  // Create time layer at bottom of screen (1/4 of height)
  int time_height = bounds.size.h / 2.8;
  int time_y = bounds.size.h - time_height;
  time_layer = text_layer_create(GRect(0, time_y, bounds.size.w, time_height));
  text_layer_set_font(time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_50)));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Get a time structure so that the face doesn't start blank
  time_t temp = time(NULL);
  struct tm *t = localtime(&temp);

  // Manually call the tick handler when the window is loading
  tick_handler(t, DAY_UNIT | MINUTE_UNIT);
}

static void prv_deinit(void)
{
  text_layer_destroy(time_layer);
  window_destroy(s_window);
}

int main(void)
{
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
