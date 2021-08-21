
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

#include "../Common.h"

using namespace GP;

const char* vertexSource = GLSL(
  in vec4 position;
  uniform vec2 Offset; 
  void main()
  {
    gl_Position = vec4(position.x+Offset.x, position.y+Offset.y, position.z, 1.0);
  });
const char* fragmentSource = GLSL(
  out vec4 fragColor;
  void main()
  { 
    fragColor = vec4(0.0, 1.0, 0.0, 1.0);
  });
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

#define TIMEOUT .01

int main(int argc, char* argv[])
{
  System system;
  
  Context context(system);
  
  Window window(context);
  Array array(context);
  Shader shader(context);
  
  ArrayData ad;
  ad.Set(vertexData, 6*sizeof(float));
  array.SetData(ad);
  
  ShaderSource source;
  source.AddString(GP_SHADER_SOURCE_VERTEX, vertexSource);
  source.AddString(GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  shader.Compile(source);
  
  float o[] = {0.0f, 0.0f};
  UniformVec2 uniform(shader, "Offset");
  uniform.Set(o);
  
  int frame = 0;
  Timer timer(system);
  timer.SetCallback([&](Timer& timer)
  {
    timer.Arm(TIMEOUT);
    
    float offset = frame%200/200.0;
    if(offset > .5) offset = 1.0-offset;
    offset -= .25;
    
    float o[] = {offset, 0.0f};
    uniform.Set(o);
    
    window.Redraw();
    
    ++frame;
  });
  timer.Arm(TIMEOUT);
  
  Pipeline pipeline = window.GetPipeline();
  
  ClearOperation clear;
  pipeline.AddOperation(clear);
  
  DrawOperation operation;
  operation.SetShader(shader);
  operation.SetUniform(uniform);
  operation.AddArrayByIndex(array, 0, 2);
  operation.SetVerticies(3);
  pipeline.AddOperation(operation);
  
  system.Run();
  
  return EXIT_SUCCESS;
}
