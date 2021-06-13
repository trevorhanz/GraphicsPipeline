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

using namespace GP;

// Shader sources
const char* vertexSource =
    "#version 300 es                              \n"
    "in highp vec4 position;                      \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.xyz, 1.0);     \n"
    "}                                            \n";
const char* fragmentSource =
    "#version 300 es                              \n"
    "uniform highp vec4 Color;                    \n"
    "out highp vec4 fragColor;                    \n"
    "void main()                                  \n"
    "{                                            \n"
    "  fragColor = Color;                         \n"
    "}                                            \n";
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int main(int argc, char* argv[])
{
  System system;
  
  Context context(system);
  
  Window window(context);
  Array array(context);
  Shader shader(context);
  
  ArrayData ad;
  ad.Set(vertexData, 6);
  array.SetData(ad);
  
  ShaderSource source;
  source.AddString(GP_SHADER_SOURCE_VERTEX, vertexSource);
  source.AddString(GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  shader.Compile(source);
  
  float c[] = {1.0f, 0.0f, 0.0f, 1.0f};
  UniformVec4 color(shader, "Color");
  color.Set(c);
  
  Pipeline pipeline = window.GetPipeline();
  
  ClearOperation clear;
  pipeline.AddOperation(clear);
  
  DrawOperation operation;
  operation.SetShader(shader);
  operation.SetUniform(color);
  operation.AddArrayByIndex(array, 0, 2);
  operation.SetVerticies(3);
  pipeline.AddOperation(operation);
  
  system.Run();
  
  return EXIT_SUCCESS;
}
