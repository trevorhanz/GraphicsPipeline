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
#include <GraphicsPipeline/Desktop.h>

#include <stdlib.h>

#include "../Common.h"

// Shader sources
const char* vertexSource = GLSL(
  in highp vec4 position;
  void main()
  {
    gl_PointSize = 64.0f;
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

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_context* context = gp_context_new(system);
  
  gp_window* window = gp_window_new(context);
  gp_array* array = gp_array_new(context);
  gp_shader* shader = gp_shader_new(context);
  
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
  gp_uniform* color1 = gp_uniform_vec4_new_by_name(shader, "Color");
  gp_uniform_vec4_set(color1, c);
  
  c[1] = 1.0f;
  gp_uniform* color2 = gp_uniform_vec4_new_by_name(shader, "Color");
  gp_uniform_vec4_set(color2, c);
  
  gp_pipeline* pipeline = gp_window_get_pipeline(window);
  
  gp_operation* clear = gp_operation_clear_new();
  gp_pipeline_add_operation(pipeline, clear);
  gp_object_unref((gp_object*)clear);
  
  gp_operation* draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_set_uniform(draw, color1);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_pipeline_add_operation(pipeline, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)color1);
  
  draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_set_uniform(draw, color2);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_operation_draw_set_mode(draw, GP_MODE_POINTS);
  gp_pipeline_add_operation(pipeline, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)color2);
  
  gp_object_unref((gp_object*)shader);
  gp_object_unref((gp_object*)array);
  
  gp_window_show(window);
  
  gp_system_run(system);
  
  gp_object_unref((gp_object*)window);
  gp_object_unref((gp_object*)context);
  gp_object_unref((gp_object*)system);
  
  return EXIT_SUCCESS;
}
