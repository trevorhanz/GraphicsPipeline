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

#define TIMEOUT .01

typedef struct
{
  gp_target* target;
  gp_array* array;
  int frame;
} animation_data;

void TimerCallback(gp_timer* timer)
{
  gp_timer_arm(timer, TIMEOUT);
  
  animation_data* data = gp_timer_get_userdata(timer);
  
  float offset = data->frame%200/200.0;
  if(offset > .5) offset = 1.0-offset;
  offset -= .25;
  float vertexData2[] = {0.0f+offset, 0.5f, 0.5f+offset, -0.5f, -0.5f+offset, -0.5f};
  
  gp_array_set_data(data->array, vertexData2, 6);
  
  gp_target_redraw(data->target);
  
  ++data->frame;
}

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_context* context = gp_system_context_new(system);
  
  gp_target* target = gp_context_target_new(context);
  gp_array* array = gp_context_array_new(context);
  gp_shader* shader = gp_context_shader_new(context);
  
  animation_data* data = malloc(sizeof(animation_data));
  data->target = target;
  data->array = array;
  data->frame = 0;
  
  gp_timer* timer = gp_system_timer_new(system);
  gp_timer_set_callback(timer, TimerCallback);
  gp_timer_set_userdata(timer, data);
  gp_timer_arm(timer, TIMEOUT);
  
  gp_array_set_data(array, vertexData, 6);
  
  gp_shader_compile(shader, vertexSource, fragmentSource);
  
  gp_pipeline* pipeline = gp_target_get_pipeline(target);
  
  gp_pipeline_add_draw(pipeline, shader, array);
  
  gp_system_run(system);
  
  return EXIT_SUCCESS;
}
