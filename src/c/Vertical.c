#include "utils.h"
#include "pebble-effect-layer/pebble-effect-layer.h"
#include "pebble-simple-health/pebble-simple-health.h"

EffectLayer *effect_data_layer;
TextLayer *time_layer;

char s_time[6];
char data_strings[5][20];
uint_least16_t health_steps, health_step_goal, health_distance, health_distance_goal, health_time_active, health_time_active_goal, health_calories_active, health_calories_active_goal, health_heart_rate;

void health_metrics_update()
{

  health_steps = health_get_metric_sum(HealthMetricStepCount);
  health_step_goal = health_get_metric_goal(HealthMetricStepCount);

  health_distance = health_get_metric_sum(HealthMetricWalkedDistanceMeters);
  health_distance_goal = health_get_metric_goal(HealthMetricWalkedDistanceMeters);

  health_time_active = health_get_metric_sum(HealthMetricActiveSeconds);
  health_time_active_goal = health_get_metric_goal(HealthMetricActiveSeconds);

  health_calories_active = health_get_metric_sum(HealthMetricActiveKCalories);
  health_calories_active_goal = health_get_metric_goal(HealthMetricActiveKCalories);

  health_heart_rate =  health_service_peek_current_value(HealthMetricHeartRateBPM);

  if (showing_health_data)
  {
    snprintf(data_strings[0], 20, "%d", health_steps);
    snprintf(data_strings[1], 20, "%d.%d mi", health_distance / 1609, health_distance * 1000 / 1609 % 1000 / 100);
    snprintf(data_strings[2], 20, "%d", health_calories_active);
    snprintf(data_strings[3], 20, "%02d:%02d", health_time_active / 3600, (health_time_active % 3600) / 60);
    snprintf(data_strings[4], 20, "%d", health_heart_rate);
  
    layer_mark_dirty(data_layer);
    layer_mark_dirty(graphics_layer);
  }
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{

  if (units_changed & MINUTE_UNIT)
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

  // Update date only when day changes
  if (units_changed & DAY_UNIT && !showing_health_data)
  {
    // Update date strings
    strftime(data_strings[0], 20, "%A", tick_time); // Day name
    strftime(data_strings[1], 20, "%B", tick_time); // Month name
    strftime(data_strings[2], 20, "%d", tick_time); // Day number
    strftime(data_strings[3], 20, "%Y", tick_time); // Year

    // Convert to uppercase
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; data_strings[i][j]; j++)
      {
        if (data_strings[i][j] >= 'a' && data_strings[i][j] <= 'z')
        {
          data_strings[i][j] = data_strings[i][j] - 'a' + 'A';
        }
      }
    }

    // Force redraw of data layer
    layer_mark_dirty(data_layer);
    layer_mark_dirty(graphics_layer);
  }
}

static void battery_state_handler(BatteryChargeState charge_state)
{
  if (!showing_health_data)
  {
    snprintf(data_strings[4], 20, "%d%%", charge_state.charge_percent);
    layer_mark_dirty(data_layer);     // Redraw data layer for battery update
    layer_mark_dirty(graphics_layer); // Redraw graphics layer for battery bar update
  }
}

static void tap_handler(AccelAxisType axis, int32_t direction)
{
  if (!health_is_available()){
    return;
  }

  showing_health_data = !showing_health_data; // Toggle health data display

  if (showing_health_data) {
    health_metrics_update();
  } else {
    // Get a time structure so that the face doesn't start blank
    time_t temp = time(NULL);
    struct tm *t = localtime(&temp);

    // Manually call the tick handler when the window is loading
    tick_handler(t, DAY_UNIT | MINUTE_UNIT);

    // Manually call the battery handler to show battery percentage on load
    battery_state_handler(battery_state_service_peek());
    }

}

// check if value is over 100 then return 100
static int clamp_percentage(int value) {
  return (value > 100) ? 100 : value;
}

static void graphics_update_proc(Layer *layer, GContext *ctx)
{

  clear_screen(ctx);

  draw_white_columns(ctx);

  if (showing_health_data)
  {
    int step_percent = clamp_percentage(health_steps * 100 / health_step_goal);
    int distance_percent = clamp_percentage(health_distance * 100 / health_distance_goal);
    int time_active_percent = clamp_percentage(health_time_active * 100 / health_time_active_goal);
    int calories_active_percent = clamp_percentage(health_calories_active * 100 / health_calories_active_goal);
    int heart_rate_percent = clamp_percentage(health_heart_rate * 100 / 220); // Assuming max heart rate of 220 bpm

    draw_grey_columns(ctx, (int[]){step_percent, distance_percent, calories_active_percent, time_active_percent, heart_rate_percent});

  }
  else
  {
    // Calculate dynamic percentages based on current date
    time_t temp = time(NULL);
    struct tm *current_time = localtime(&temp);

    int day_of_week_percent = ((current_time->tm_wday == 0 ? 7 : current_time->tm_wday) * 100) / 7; // Sunday=7, Monday=1, etc.
    int month_percent = (current_time->tm_mon + 1) * 100 / 12;                                      // tm_mon is 0-11, so add 1
    int day_of_month_percent = current_time->tm_mday * 100 / 31;                                    // Approximate, assuming max 31 days
    int year_percent = (current_time->tm_year + 1900) * 100 / 5000;                                 // tm_year is years since 1900
    int battery_percent = battery_state_service_peek().charge_percent;                              // Battery percentage

    draw_grey_columns(ctx, (int[]){day_of_week_percent, month_percent, day_of_month_percent, year_percent, battery_percent});
  }
}

static void data_update_proc(Layer *layer, GContext *ctx)
{
  // Convert 2D array to array of pointers for compatibility
  const char *text_lines[5];

  text_lines[0] = data_strings[0];
  text_lines[1] = data_strings[1];
  text_lines[2] = data_strings[2];
  text_lines[3] = data_strings[3];
  text_lines[4] = data_strings[4];

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
  int time_height = bounds.size.h / 2.7;  

  int time_y = bounds.size.h - time_height;
  time_layer = text_layer_create(GRect(0, time_y, bounds.size.w, time_height));

  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
  #if PBL_DISPLAY_HEIGHT == 228
    text_layer_set_font(time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_72)));
  #else
    text_layer_set_font(time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_50)));
  #endif
  health_init(health_metrics_update);
  health_metrics_update();

  tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT, tick_handler);
  battery_state_service_subscribe(battery_state_handler);

  accel_tap_service_subscribe(&tap_handler);

  // Get a time structure so that the face doesn't start blank
  time_t temp = time(NULL);
  struct tm *t = localtime(&temp);

  // Manually call the tick handler when the window is loading
  tick_handler(t, DAY_UNIT | MINUTE_UNIT);

  // Manually call the battery handler to show battery percentage on load
  battery_state_handler(battery_state_service_peek());
}

static void prv_deinit(void)
{
  // Unsubscribe from services
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  accel_tap_service_unsubscribe();
  health_deinit();

  // Destroy layers
  text_layer_destroy(time_layer);
  effect_layer_destroy(effect_data_layer);
  layer_destroy(data_layer);
  layer_destroy(graphics_layer);

  // Destroy window
  window_destroy(s_window);
}

int main(void)
{
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
