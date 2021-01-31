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
#include <stdio.h>

const char* vertexSource =
    "attribute vec4 position;                     \n"
    "uniform vec2 Offset;                         \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.x+Offset.x, position.y+Offset.y, position.z, 1.0);\n"
    "}                                            \n";
const char* fragmentSource =
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);   \n"
    "}                                            \n";
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

#define TIMEOUT .01

typedef struct
{
  gp_target* target;
  gp_uniform* offset;
  int frame;
} animation_data;

void TimerCallback(gp_timer* timer)
{
  gp_timer_arm(timer, TIMEOUT);
  
  animation_data* data = gp_timer_get_userdata(timer);
  
  float offset = data->frame%200/200.0;
  if(offset > .5) offset = 1.0-offset;
  offset -= .25;
  
  float o[] = {offset, 0.0f};
  gp_uniform_set_vec2(data->offset, o);
  
  gp_target_redraw(data->target);
  
  ++data->frame;
}

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_context* context = gp_context_new(system);
  
  gp_target* target = gp_target_new(context);
  gp_array* array = gp_array_new(context);
  gp_shader* shader = gp_shader_new(context);
  
  animation_data* data = malloc(sizeof(animation_data));
  data->target = target;
  data->frame = 0;
  
  gp_timer* timer = gp_system_timer_new(system);
  gp_timer_set_callback(timer, TimerCallback);
  gp_timer_set_userdata(timer, data);
  gp_timer_arm(timer, TIMEOUT);
  
  gp_array_set_data(array, vertexData, 6);
  
  gp_shader_compile(shader, vertexSource, fragmentSource);
  
  float o[] = {0.0f, 0.0f};
  data->offset = gp_shader_uniform_new_by_name(shader, "Offset");
  gp_uniform_set_vec2(data->offset, o);
  
  gp_pipeline* pipeline = gp_target_get_pipeline(target);
  
  gp_operation* clear = gp_operation_clear_new();
  gp_pipeline_add_operation(pipeline, clear);
  
  gp_operation* draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_set_uniform(draw, data->offset);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_pipeline_add_operation(pipeline, draw);
  
  gp_system_run(system);
  
  return EXIT_SUCCESS;
}
