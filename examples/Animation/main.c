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
#include <sys/time.h>

#include "../Common.h"

const char* vertexSource = GLSL(
  precision highp float;
  in vec3 position;
  in vec2 UV;
  uniform vec2 Offset;
  out vec2 uv;
  void main()
  {
    uv = UV;
    gl_Position = vec4(position.x+Offset.x, position.y+Offset.y, position.z, 1.0);
  });
const char* fragmentSource = GLSL(
  precision highp float;
  in vec2 uv;
  uniform sampler2D Texture;
  uniform sampler2D Colormap;
  uniform float ColorShift;
  out vec4 fragColor;
  void main()
  {
    float value = texture(Texture, uv).r+ColorShift;
    fragColor = vec4(texture(Colormap, vec2(value, .5)).rgb, 1.0);
  });
float vertexData[] = {
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
  -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
  -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
  0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
};

#define TIMEOUT .01

typedef struct
{
  gp_window* window;
  gp_uniform* offset;
  gp_uniform* colorShift;
  struct timeval beginFrame;
  int frame;
  float direction[2];
  float position[2];
  float shift;
} animation_data;

void TimerCallback(gp_timer* timer, gp_pointer* userdata)
{
  gp_timer_arm(timer, TIMEOUT);
  
  animation_data* data = gp_pointer_get_pointer(userdata);
  
  data->position[0] += data->direction[0]*TIMEOUT;
  data->position[1] += data->direction[1]*TIMEOUT;
  
  if(data->position[0]>0.5f || data->position[0]<-0.5f)
    data->direction[0] *= -1;
  if(data->position[1]>0.5f || data->position[1]<-0.5f)
    data->direction[1] *= -1;
  
  gp_uniform_vec2_set(data->offset, data->position);
  
  data->shift += .01;
  gp_uniform_float_set(data->colorShift, data->shift);
  
  gp_window_redraw(data->window);
  
  struct timeval endFrame;
  gettimeofday(&endFrame, NULL);
  double delta = (endFrame.tv_sec-data->beginFrame.tv_sec) * 1000000 + (endFrame.tv_usec-data->beginFrame.tv_usec);
  if(delta > 1000000)
  {
    gp_log("FPS: %.2f", data->frame/(delta/1000000.0));
    data->beginFrame = endFrame;
    data->frame = 0;
  }
  
  ++data->frame;
}

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_context* context = gp_context_new(system);
  
  gp_window* window = gp_window_new(context);
  gp_array* array = gp_array_new(context);
  gp_texture* texture = gp_texture_new(context);
  gp_texture* colormap = gp_texture_new(context);
  gp_shader* shader = gp_shader_new(context);
  
  animation_data* data = malloc(sizeof(animation_data));
  data->window = window;
  data->frame = 0;
  data->direction[0] = .25;
  data->direction[1] = .5;
  data->position[0] = 0;
  data->position[1] = 0;
  data->shift = 0;
  gettimeofday(&data->beginFrame, NULL);
  gp_pointer* pointer = gp_pointer_new(data, 0);
  
  gp_timer* timer = gp_timer_new(system);
  gp_timer_set_callback(timer, TimerCallback, pointer);
  gp_timer_arm(timer, TIMEOUT);
  gp_object_unref((gp_object*)pointer);
  
  gp_array_data* ad = gp_array_data_new();
  gp_array_data_set(ad, vertexData, 30*sizeof(float));
  gp_array_set_data(array, ad);
  gp_object_unref((gp_object*)ad);
  
  float textureData[] = {
    1.0f, .5f, 1.0f,
    .5f, .0f, .5f,
    1.0f, .5f, 1.0f};
  gp_texture_data* td = gp_texture_data_new();
  gp_texture_data_set_2d(td, textureData, GP_FORMAT_R, GP_DATA_TYPE_FLOAT, 3, 3);
  gp_texture_set_data(texture, td);
  gp_object_unref((gp_object*)td);
  
  unsigned char colormapData[] = {
    255, 0, 0,
    0, 255, 0,
    0, 0, 255,
    0, 255, 255,
  };
  gp_texture_data* cd = gp_texture_data_new();
  gp_texture_data_set_2d(cd, colormapData, GP_FORMAT_RGB, GP_DATA_TYPE_UBYTE, 4, 1);
  gp_texture_set_data(colormap, cd);
  gp_texture_set_wrap_x(colormap, GP_WRAP_MIRROR);
  gp_object_unref((gp_object*)cd);
  
  gp_shader_source* source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSource);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  gp_shader_compile(shader, source);
  gp_object_unref((gp_object*)source);
  
  gp_uniform* tex = gp_uniform_texture_new_by_name(shader, "Texture");
  gp_uniform_texture_set(tex, texture);
  
  gp_uniform* cm = gp_uniform_texture_new_by_name(shader, "Colormap");
  gp_uniform_texture_set(cm, colormap);
  
  float o[] = {0.0f, 0.0f};
  data->offset = gp_uniform_vec2_new_by_name(shader, "Offset");
  gp_uniform_vec2_set(data->offset, o);
  
  data->colorShift = gp_uniform_float_new_by_name(shader, "ColorShift");
  gp_uniform_float_set(data->colorShift, 0);
  
  gp_pipeline* pipeline = gp_window_get_pipeline(window);
  
  gp_operation* clear = gp_operation_clear_new();
  gp_pipeline_add_operation(pipeline, clear);
  gp_object_unref((gp_object*)clear);
  
  gp_operation* draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_set_uniform(draw, data->offset);
  gp_operation_draw_set_uniform(draw, data->colorShift);
  gp_operation_draw_set_uniform(draw, tex);
  gp_operation_draw_set_uniform(draw, cm);
  gp_operation_draw_add_array_by_index(draw, array, 0, 3, GP_DATA_TYPE_FLOAT, sizeof(float)*5, 0);
  gp_operation_draw_add_array_by_index(draw, array, 1, 2, GP_DATA_TYPE_FLOAT, sizeof(float)*5, sizeof(float)*3);
  gp_operation_draw_set_verticies(draw, 6);
  gp_operation_draw_set_mode(draw, GP_MODE_TRIANGLES);
  gp_pipeline_add_operation(pipeline, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)tex);
  gp_object_unref((gp_object*)cm);
  
  gp_object_unref((gp_object*)shader);
  gp_object_unref((gp_object*)array);
  gp_object_unref((gp_object*)texture);
  
  gp_window_show(window);
  
  gp_system_run(system);
  
  gp_object_unref((gp_object*)window);
  gp_object_unref((gp_object*)context);
  gp_object_unref((gp_object*)timer);
  gp_object_unref((gp_object*)system);
  
  gp_object_unref((gp_object*)data->offset);
  free(data);
  
  return EXIT_SUCCESS;
}
