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
    "attribute vec4 position;       \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.xyz, 1.0);     \n"
    "}                                            \n";
const char* fragmentSource =
    "uniform vec4 Color;                          \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = Color;                      \n"
    "}                                            \n";
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int main(int argc, char* argv[])
{
  System* system = new System();
  
  Context* context = system->CreateContext();
  
  Target* target = context->CreateTarget();
  Array* array = context->CreateArray();
  Shader* shader = context->CreateShader();
  
  array->SetData(vertexData, 6);
  
  shader->Compile(vertexSource, fragmentSource);
  
  float c[] = {1.0f, 0.0f, 0.0f, 1.0f};
  Uniform* color = shader->CreateUniform("Color");
  color->SetVec4(c);
  
  Pipeline* pipeline = target->GetPipeline();
  
  pipeline->AddOperation(new ClearOperation());
  
  DrawOperation* operation = new DrawOperation();
  operation->SetShader(shader);
  operation->SetUniform(color);
  operation->AddArrayByIndex(array, 0, 2);
  operation->SetVerticies(3);
  pipeline->AddOperation(operation);
  
  system->Run();
  
  return EXIT_SUCCESS;
}
