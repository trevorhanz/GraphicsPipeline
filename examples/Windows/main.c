/************************************************************************
* Copyright (C) 2021 Trevor Hanz
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
************************************************************************/

#include <GraphicsPipeline/GP.h>
#include <GraphicsPipeline/Desktop.h>

#include <stdlib.h>

#include "../Common.h"

const char* vertexSource = GLSL(
  in highp vec4 position;
  uniform vec2 size;
  void main()
  {
    gl_Position = vec4(position.x/(size.x/2.0)-1.0, position.y/(size.y/2.0)-1.0, position.z, 1.0);
  });
const char* fragmentSource = GLSL(
  out highp vec4 fragColor;
  void main()
  {
    fragColor = vec4(0, 1, 1, 1);
  });
float vertexData[] = {512.0f, 744.0f, 1000.0f, 24.0f, 24.0f, 24.0f};

typedef struct
{
  gp_window*        window;
  gp_uniform*       size;
} callbackData;

void window_resize_callback(const gp_event_resize_t* resize, gp_pointer* userData)
{
  callbackData* data = (callbackData*)gp_pointer_get_pointer(userData);
  
  float size[2];
  size[0] = resize->width;
  size[1] = resize->height;
  gp_uniform_vec2_set(data->size, size);
  
  gp_window_redraw(data->window);
}

typedef struct
{
  gp_window*        window1;
  gp_window*        window2;
  gp_operation*     draw;
  int               state;
} timerCallbackData;

void window_timer_callback(gp_timer* timer)
{
  timerCallbackData* data = (timerCallbackData*)gp_pointer_get_pointer(gp_timer_get_userdata(timer));
  if(data->state == 0)
  {
    gp_log("Add Operation to Window1");
    gp_pipeline* pipeline = gp_window_get_pipeline(data->window1);
    gp_pipeline_add_operation(pipeline, data->draw);
    gp_window_redraw(data->window1);
    ++data->state;
  }
  else if(data->state == 1)
  {
    gp_log("Move Operation to Window2");
    gp_pipeline* pipeline = gp_window_get_pipeline(data->window2);
    gp_pipeline_add_operation(pipeline, data->draw);
    gp_window_redraw(data->window1);
    gp_window_redraw(data->window2);
    ++data->state;
  }
  else if(data->state == 2)
  {
    gp_log("Remove Operation from Window2");
    gp_pipeline* pipeline = gp_window_get_pipeline(data->window2);
    gp_pipeline_remove_operation(pipeline, data->draw);
    gp_window_redraw(data->window2);
    data->state = 0;
  }
  
  gp_timer_arm(timer, 1.0);
}

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_context* context = gp_context_new(system);
  
  gp_window* window1 = gp_window_new(context);
  gp_window* window2 = gp_window_new(context);
  
  gp_window_set_title(window1, "Example Window 1");
  gp_window_set_title(window2, "Example Window 2");
  
  gp_window_set_type(window2, GP_WINDOW_TYPE_UTILITY);
  
  gp_window_set_size(window1, 1024, 768);
  gp_window_set_size(window2, 640, 480);
  gp_window_set_min_size(window1, 640, 480);
  gp_window_set_max_size(window1, 1024, 768);
  
  unsigned int x, y;
  gp_window_get_position(window2, &x, &y);
  gp_window_set_position(window2, x+10, y+10);
  
  callbackData window1Data;
  callbackData window2Data;
  gp_window_set_resize_callback(window1, window_resize_callback, gp_pointer_new(&window1Data, NULL));
  gp_window_set_resize_callback(window2, window_resize_callback, gp_pointer_new(&window2Data, NULL));
  window1Data.window = window1;
  window2Data.window = window2;
  
  gp_array* array = gp_array_new(context);
  gp_shader* shader = gp_shader_new(context);
  
  gp_array_data* ad = gp_array_data_new();
  gp_array_data_set(ad, vertexData, 6);
  gp_array_set_data(array, ad);
  gp_object_unref((gp_object*)ad);
  
  gp_shader_source* source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSource);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  gp_shader_compile(shader, source);
  gp_object_unref((gp_object*)source);
  
  window1Data.size = gp_uniform_vec2_new_by_name(shader, "size");
  window2Data.size = gp_uniform_vec2_new_by_name(shader, "size");
  
  float size[2];
  size[0] = 1024;
  size[1] = 768;
  gp_uniform_vec2_set(window1Data.size, size);
  gp_uniform_vec2_set(window2Data.size, size);
  
  gp_pipeline* pipeline = gp_window_get_pipeline(window1);
  gp_operation* clear = gp_operation_clear_new();
  gp_pipeline_add_operation(pipeline, clear);
  gp_object_unref((gp_object*)clear);
  
  gp_operation* draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_set_uniform(draw, window1Data.size);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_pipeline_add_operation(pipeline, draw);
  
  pipeline = gp_window_get_pipeline(window2);
  clear = gp_operation_clear_new();
  gp_pipeline_add_operation(pipeline, clear);
  gp_object_unref((gp_object*)clear);
  
  gp_object_unref((gp_object*)shader);
  gp_object_unref((gp_object*)array);
  
  timerCallbackData* data = malloc(sizeof(timerCallbackData));
  data->window1 = window1;
  data->window2 = window2;
  data->draw = draw;
  data->state = 1;
  gp_pointer* pointer = gp_pointer_new(data, free);
  gp_timer* timer = gp_timer_new(system);
  gp_timer_set_callback(timer, window_timer_callback);
  gp_timer_set_userdata(timer, pointer);
  gp_timer_arm(timer, 1.0);
  gp_object_unref((gp_object*)pointer);
  
  // Test different show functions.
  if(gp_window_get_shown(window1))
    gp_window_hide(window1);
  else
    gp_window_show(window1);
  gp_window_show(window2);
  
  gp_system_run(system);
  
  gp_object_unref((gp_object*)window1);
  gp_object_unref((gp_object*)window2);
  gp_object_unref((gp_object*)context);
  gp_object_unref((gp_object*)system);
  
  return EXIT_SUCCESS;
}
