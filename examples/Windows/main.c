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

const char* vertexSource =
    "attribute vec4 position;                     \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.xyz, 1.0);     \n"
    "}                                            \n";
const char* fragmentSource =
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);   \n"
    "}                                            \n";
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_context* context = gp_system_context_new(system);
  
  gp_target* target1 = gp_context_target_new(context);
  gp_target* target2 = gp_context_target_new(context);
  
  gp_array* array = gp_context_array_new(context);
  gp_shader* shader = gp_context_shader_new(context);
  
  gp_array_set_data(array, vertexData, 6);
  
  gp_shader_compile(shader, vertexSource, fragmentSource);
  
  gp_pipeline* pipeline = gp_target_get_pipeline(target1);
  gp_operation* draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_pipeline_add_operation(pipeline, draw);
  
  
  pipeline = gp_target_get_pipeline(target2);
  draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_pipeline_add_operation(pipeline, draw);
  
  gp_system_run(system);
  
  return EXIT_SUCCESS;
}
