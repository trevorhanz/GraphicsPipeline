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

// Shader sources
const char* vertexSource = GLSL(
  in highp vec4 position;
  void main()
  {
    gl_Position = vec4(position.xyz, 1.0);
  });
const char* fragmentSource = GLSL(
  out highp vec4 fragColor;
  void main()
  {
    fragColor = vec4(0, 1, 1, 1);
  });
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

const char* vertexSourceQuad = GLSL(
  in highp vec4 position;
  out vec2 UV;
  void main()
  {
    UV = position.xy;
    gl_Position = vec4(position.xyz*2.0-1.0, 1.0);
  });
const char* fragmentSourceQuad = GLSL(
  precision highp float;
  out highp vec4 fragColor;
  in vec2 UV;
  uniform sampler2D Texture;
  void main()
  {
    fragColor = texture(Texture, UV);
  });

const char* vertexSourceBlur = GLSL(
  in highp vec4 position;
  out vec2 UV;
  void main()
  {
    UV = position.xy;
    gl_Position = vec4(position.xyz*2.0-1.0, 1.0);
  });
const char* fragmentSourceBlur = GLSL(
  precision highp float;
  out highp vec4 fragColor;
  in vec2 UV;
  uniform sampler2D Texture;
  const float offsetX = 1.0 / 1024.0;
  const float offsetY = 1.0 / 768.0;
  void main()
  {
    vec2 offsets[9] = vec2[](
      vec2(-offsetX,  offsetY),   // top-left
      vec2( 0.0f,    offsetY),    // top-center
      vec2( offsetX,  offsetY),   // top-right
      vec2(-offsetX,  0.0f),      // center-left
      vec2( 0.0f,    0.0f),       // center-center
      vec2( offsetX,  0.0f),      // center-right
      vec2(-offsetX, -offsetY),   // bottom-left
      vec2( 0.0f,   -offsetY),    // bottom-center
      vec2( offsetX, -offsetY)    // bottom-right    
    );
    float kernel[9] = float[](
      1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
      2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
      1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
      );
    vec4 color;
    for(int i=0; i<9; ++i)
      color += texture(Texture, UV+offsets[i])*kernel[i];
    fragColor = color;
  });
const char* vertexSourceWave = GLSL(
  in highp vec4 position;
  out vec2 UV;
  void main()
  {
    UV = position.xy;
    gl_Position = vec4(position.xyz*2.0-1.0, 1.0);
  });
const char* fragmentSourceWave = GLSL(
  precision highp float;
  out vec4 fragColor;
  in vec2 UV;
  uniform sampler2D Texture;
  uniform float Offset;
  const float offsetX = 1.0 / 1024.0;
  const float offsetY = 1.0 / 768.0;
  void main()
  {
    vec2 uv = UV;
    uv.x = uv.x+sin((uv.y+Offset)*20.0)*.1;
    fragColor = texture(Texture, uv);
  });

const char* vertexSourceGaussianVertical = GLSL(
  in highp vec4 position;
  out vec2 UV;
  void main()
  {
    UV = position.xy;
    gl_Position = vec4(position.xyz*2.0-1.0, 1.0);
  });
const char* fragmentSourceGaussianVertical = GLSL(
  precision highp float;
  out highp vec4 fragColor;
  in vec2 UV;
  uniform sampler2D Texture;
  float offset[3] = float[]( 1.411764705882353, 3.2941176470588234, 5.176470588235294 );
  float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
  
  void main() 
  { 
    vec2 uv = UV.xy;
    vec3 tc = texture(Texture, uv).rgb * weight[0];
    for (int i=1; i<3; i++) 
    {
      tc += texture(Texture, uv + vec2(0.0, offset[i])/768.0).rgb \
              * weight[i];
      tc += texture(Texture, uv - vec2(0.0, offset[i])/768.0).rgb \
            * weight[i];
    }
    fragColor = vec4(tc, 1.0);
  });
const char* vertexSourceGaussianHorizontal = GLSL(
  in highp vec4 position;
  out vec2 UV;
  void main()
  {
    UV = position.xy;
    gl_Position = vec4(position.xyz*2.0-1.0, 1.0);
  });
const char* fragmentSourceGaussianHorizontal = GLSL(
  precision highp float;
  out highp vec4 fragColor;
  in vec2 UV;
  uniform sampler2D Texture;
  float offset[3] = float[]( 1.411764705882353, 3.2941176470588234, 5.176470588235294 );
  float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
  
  void main() 
  { 
    vec2 uv = UV.xy;
    vec3 tc = texture(Texture, uv).rgb * weight[0];
    for (int i=1; i<3; i++) 
    {
      tc += texture(Texture, uv + vec2(offset[i], 0.0)/1024.0).rgb \
              * weight[i];
      tc += texture(Texture, uv - vec2(offset[i], 0.0)/1024.0).rgb \
            * weight[i];
    }
    fragColor = vec4(tc, 1.0);
  });

float vertexDataQuad[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};

typedef struct
{
  gp_uniform*           offsetWave;
  gp_window*            window;
} TimeoutData;

void TimeoutFunc(gp_timer* timer, gp_pointer* userdata)
{
  TimeoutData* data = (TimeoutData*)gp_pointer_get_pointer(userdata);
  
  gp_uniform_float_set(data->offsetWave, gp_uniform_float_get(data->offsetWave)+.01);
  
  gp_window_redraw(data->window);
  
  gp_timer_arm(timer, .01);
}

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_context* context = gp_context_new(system);
  
  gp_window* window = gp_window_new(context);
  gp_texture* texture = gp_texture_new(context);
  gp_frame_buffer* fb = gp_frame_buffer_new(context);
  gp_texture* textureG = gp_texture_new(context);
  gp_frame_buffer* fbG = gp_frame_buffer_new(context);
  gp_array* array = gp_array_new(context);
  gp_shader* shader = gp_shader_new(context);
  gp_array* arrayQuad = gp_array_new(context);
  gp_shader* shaderQuad = gp_shader_new(context);
  gp_shader* shaderBlur = gp_shader_new(context);
  gp_shader* shaderWave = gp_shader_new(context);
  gp_shader* shaderGV = gp_shader_new(context);
  gp_shader* shaderGH = gp_shader_new(context);
  
  gp_array_data* ad = gp_array_data_new();
  gp_array_data_set(ad, vertexData, 6*sizeof(float));
  gp_array_set_data(array, ad);
  gp_object_unref((gp_object*)ad);
  
  ad = gp_array_data_new();
  gp_array_data_set(ad, vertexDataQuad, 8*sizeof(float));
  gp_array_set_data(arrayQuad, ad);
  gp_object_unref((gp_object*)ad);
  
  gp_shader_source* source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSource);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  gp_shader_compile(shader, source);
  gp_object_unref((gp_object*)source);
  
  source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSourceQuad);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSourceQuad);
  gp_shader_compile(shaderQuad, source);
  gp_object_unref((gp_object*)source);
  
  source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSourceBlur);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSourceBlur);
  gp_shader_compile(shaderBlur, source);
  gp_object_unref((gp_object*)source);
  
  source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSourceWave);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSourceWave);
  gp_shader_compile(shaderWave, source);
  gp_object_unref((gp_object*)source);
  
  source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSourceGaussianVertical);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSourceGaussianVertical);
  gp_shader_compile(shaderGV, source);
  gp_object_unref((gp_object*)source);
  
  source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSourceGaussianHorizontal);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSourceGaussianHorizontal);
  gp_shader_compile(shaderGH, source);
  gp_object_unref((gp_object*)source);
  
  gp_frame_buffer_attach(fb, texture);
  gp_pipeline* pipeline = gp_frame_buffer_get_pipeline(fb);
  
  gp_operation* clear = gp_operation_clear_new();
  gp_pipeline_add_operation(pipeline, clear);
  gp_object_unref((gp_object*)clear);
  
  gp_operation* draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_pipeline_add_operation(pipeline, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)shader);
  gp_object_unref((gp_object*)array);
  
  pipeline = gp_window_get_pipeline(window);
  
  //
  // Upper Left
  //
  gp_operation* view = gp_operation_viewport_new();
  gp_operation_viewport_set_dimesions(view, 0, 385, 512, 385);
  gp_pipeline_add_operation(pipeline, view);
  gp_pipeline* view_pipeline = gp_operation_viewport_get_pipeline(view);
  
  gp_uniform* texQuad = gp_uniform_texture_new_by_name(shaderQuad, "Texture");
  gp_uniform_texture_set(texQuad, texture);
  
  draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shaderQuad);
  gp_operation_draw_add_array_by_index(draw, arrayQuad, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_uniform(draw, texQuad);
  gp_operation_draw_set_verticies(draw, 4);
  gp_operation_draw_set_mode(draw, GP_MODE_TRIANGLE_STRIP);
  gp_pipeline_add_operation(view_pipeline, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)texQuad);
  
  //
  // Upper Right
  //
  view = gp_operation_viewport_new();
  gp_operation_viewport_set_dimesions(view, 512, 385, 512, 385);
  gp_pipeline_add_operation(pipeline, view);
  view_pipeline = gp_operation_viewport_get_pipeline(view);
  

  gp_frame_buffer_attach(fbG, textureG);
  gp_pipeline* pipelineG = gp_frame_buffer_get_pipeline(fbG);
  
  clear = gp_operation_clear_new();
  gp_pipeline_add_operation(pipelineG, clear);
  gp_operation_clear_set_color(clear, 1, 0, 0, 1);
  gp_object_unref((gp_object*)clear);
  
  view = gp_operation_viewport_new();
  gp_operation_viewport_set_dimesions(view, 0, 0, 1024, 768);
  gp_pipeline_add_operation(pipelineG, view);
  pipelineG = gp_operation_viewport_get_pipeline(view);
  
  gp_uniform* texGV = gp_uniform_texture_new_by_name(shaderGV, "Texture");
  gp_uniform_texture_set(texGV, texture);
  
  draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shaderGV);
  gp_operation_draw_add_array_by_index(draw, arrayQuad, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_uniform(draw, texGV);
  gp_operation_draw_set_verticies(draw, 4);
  gp_operation_draw_set_mode(draw, GP_MODE_TRIANGLE_STRIP);
  gp_pipeline_add_operation(pipelineG, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)texGV);
  
  gp_uniform* texGH = gp_uniform_texture_new_by_name(shaderGH, "Texture");
  gp_uniform_texture_set(texGH, textureG);
  
  draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shaderGH);
  gp_operation_draw_add_array_by_index(draw, arrayQuad, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_uniform(draw, texGH);
  gp_operation_draw_set_verticies(draw, 4);
  gp_operation_draw_set_mode(draw, GP_MODE_TRIANGLE_STRIP);
  gp_pipeline_add_operation(view_pipeline, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)texGH);
  
  //
  // Lower Left
  //
  gp_uniform* texWave = gp_uniform_texture_new_by_name(shaderWave, "Texture");
  gp_uniform_texture_set(texWave, texture);
  
  gp_uniform* offsetWave = gp_uniform_float_new_by_name(shaderWave, "Offset");
  gp_uniform_float_set(offsetWave, 0);
  
  view = gp_operation_viewport_new();
  gp_operation_viewport_set_dimesions(view, 0, 0, 512, 385);
  gp_pipeline_add_operation(pipeline, view);
  view_pipeline = gp_operation_viewport_get_pipeline(view);
  
  draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shaderWave);
  gp_operation_draw_add_array_by_index(draw, arrayQuad, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_uniform(draw, texWave);
  gp_operation_draw_set_uniform(draw, offsetWave);
  gp_operation_draw_set_verticies(draw, 4);
  gp_operation_draw_set_mode(draw, GP_MODE_TRIANGLE_STRIP);
  gp_pipeline_add_operation(view_pipeline, draw);
  gp_object_unref((gp_object*)draw);
  gp_object_unref((gp_object*)texWave);
  gp_object_unref((gp_object*)offsetWave);
  
  //
  // Lower Right
  //
  view = gp_operation_viewport_new();
  gp_operation_viewport_set_dimesions(view, 512, 0, 512, 385);
  gp_pipeline_add_operation(pipeline, view);
  view_pipeline = gp_operation_viewport_get_pipeline(view);
  
  draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shaderBlur);
  gp_operation_draw_add_array_by_index(draw, arrayQuad, 0, 2, GP_DATA_TYPE_FLOAT, 0, 0);
  gp_operation_draw_set_verticies(draw, 4);
  gp_operation_draw_set_mode(draw, GP_MODE_TRIANGLE_STRIP);
  gp_pipeline_add_operation(view_pipeline, draw);
  gp_object_unref((gp_object*)draw);
  
  gp_object_unref((gp_object*)shaderQuad);
  gp_object_unref((gp_object*)shaderBlur);
  gp_object_unref((gp_object*)shaderGH);
  gp_object_unref((gp_object*)shaderGV);
  gp_object_unref((gp_object*)shaderWave);
  gp_object_unref((gp_object*)arrayQuad);
  gp_frame_buffer_redraw(fbG);
  gp_frame_buffer_redraw(fb);
  gp_frame_buffer_redraw(fbG);
  
  TimeoutData* data = malloc(sizeof(TimeoutData));
  data->offsetWave = offsetWave;
  data->window = window;
  gp_pointer* pointer = gp_pointer_new(data, free);
  
  gp_timer* timer = gp_timer_new(system);
  gp_timer_set_callback(timer, &TimeoutFunc, pointer);
  gp_timer_arm(timer, 0.01);
  gp_object_unref((gp_object*)pointer);
  
  gp_window_show(window);
  
  gp_system_run(system);
  
  gp_object_unref((gp_object*)fb);
  gp_object_unref((gp_object*)fbG);
  gp_object_unref((gp_object*)window);
  gp_object_unref((gp_object*)context);
  gp_object_unref((gp_object*)timer);
  gp_object_unref((gp_object*)system);
  free(data);
  
  return EXIT_SUCCESS;
}

