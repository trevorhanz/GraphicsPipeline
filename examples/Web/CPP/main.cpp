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
#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Web.h>

using namespace GP;

// Shader sources
const char* vertexSource =
    "attribute vec4 position;                     \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.xyz, 1.0);     \n"
    "}                                            \n";
const char* fragmentSource =
    "precision mediump float;\n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);   \n"
    "}                                            \n";
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int main(int argv, char* argc[])
{
  System system;
  
  Web::Context context(system, "#WebContext");
  
  Array array(context);
  Shader shader(context);
  
  Web::Window window1(context, "#WebWindow1");
  Web::Window window2(context, "#WebWindow2");
  
  ArrayData ad;
  ad.Set(vertexData, 6);
  array.SetData(ad);
  
  ShaderSource source;
  source.AddString(GP_SHADER_SOURCE_VERTEX, vertexSource);
  source.AddString(GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  shader.Compile(source);
  
  Pipeline pipeline = window1.GetPipeline();
  
  ClearOperation clear1;
  clear1.SetColor(1, 0, 0, 1);
  pipeline.AddOperation(clear1);
  
  DrawOperation operation;
  operation.SetShader(shader);
  operation.AddArrayByIndex(array, 0, 2);
  operation.SetVerticies(3);
  pipeline.AddOperation(operation);
  
  pipeline = window2.GetPipeline();
  
  ClearOperation clear2;
  clear2.SetColor(0, 0, 1, 1);
  pipeline.AddOperation(clear2);
  
  pipeline.AddOperation(operation);
  
  system.Run();
}

