/************************************************************************
* Copyright (C) 2020 Trevor Hanz
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

#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#define GLSL(src) "#version 400\n" #src
#else
#define GLSL(src) "#version 300 es\n" #src
#endif

const char* vertexSource = GLSL(
  in highp vec4 position;
  void main()
  {
    gl_Position = vec4(position.xyz, 1.0);
  });
const char* fragmentSource = GLSL(
  uniform highp vec4 Color;
  out highp vec4 fragColor;
  void main()
  {
    fragColor = Color;
  });
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

void mouse_click_callback(const gp_event_click_t* click, gp_pointer* userData)
{
  gp_log("Click: %d, %d, %d, %d", click->button, click->state, click->x, click->y);
}

void mouse_scroll_callback(const gp_event_scroll_t* scroll, gp_pointer* userData)
{
  gp_log("Scroll: %f", scroll->scroll);
}

void mouse_track_callback(const gp_event_track_t* track, gp_pointer* userData)
{
  gp_log("Track: %d, %d", track->x, track->y);
}

void mouse_enter_callback(const gp_event_enter_t* enter, gp_pointer* userData)
{
  if(enter->enter) gp_log("Enter");
  else gp_log("Leave");
}

void key_callback(const gp_event_key_t* key, gp_pointer* userData)
{
  if(key->state) gp_log("Press: %d", key->key);
  else gp_log("Release: %d", key->key);
}

void window_resize_callback(const gp_event_resize_t* resize, gp_pointer* userData)
{
  gp_log("Resize: %d, %d", resize->width, resize->height);
}

void window_move_callback(const gp_event_move_t* move, gp_pointer* userData)
{
  gp_log("Move: %dx%d", move->x, move->y);
}

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_context* context = gp_context_new(system);
  
  gp_window* window = gp_window_new(context);
  gp_array* array = gp_array_new(context);
  gp_shader* shader = gp_shader_new(context);
  
  gp_window_set_click_callback(window, mouse_click_callback, NULL);
  gp_window_set_scroll_callback(window, mouse_scroll_callback, NULL);
  gp_window_set_track_callback(window, mouse_track_callback, NULL);
  gp_window_set_enter_callback(window, mouse_enter_callback, NULL);
  gp_window_set_key_callback(window, key_callback, NULL);
  gp_window_set_resize_callback(window, window_resize_callback, NULL);
  gp_window_set_move_callback(window, window_move_callback, NULL);
  
  gp_array_data* ad = gp_array_data_new();
  gp_array_data_set(ad, vertexData, 6*sizeof(float));
  gp_array_set_data(array, ad);
  gp_object_unref((gp_object*)ad);
  
  gp_shader_source* source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSource);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  gp_shader_compile(shader, source);
  gp_object_unref((gp_object*)source);
  
  float c[] = {1.0f, 0.0f, 0.0f, 1.0f};
  gp_uniform* color = gp_uniform_vec4_new_by_name(shader, "Color");
  gp_uniform_vec4_set(color, c);
  
  gp_pipeline* pipeline = gp_window_get_pipeline(window);
  
  gp_operation* clear = gp_operation_clear_new();
  gp_pipeline_add_operation(pipeline, clear);
  gp_object_unref((gp_object*)clear);
  
  gp_operation* draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_set_uniform(draw, color);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_pipeline_add_operation(pipeline, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)color);
  gp_object_unref((gp_object*)shader);
  gp_object_unref((gp_object*)array);
  
  gp_window_show(window);
  
  gp_system_run(system);
  
  gp_object_unref((gp_object*)window);
  gp_object_unref((gp_object*)context);
  gp_object_unref((gp_object*)system);
  
  return EXIT_SUCCESS;
}

