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
  System* system = new System();
  
  Context* context = system->CreateContext();
  
  Target* target = context->CreateTarget();
  Array* array = context->CreateArray();
  Shader* shader = context->CreateShader();
  
  array->SetData(vertexData, 6);
  
  shader->Compile(vertexSource, fragmentSource);
  
  Pipeline* pipeline = target->GetPipeline();
  
  DrawOperation* operation = new DrawOperation();
  operation->SetShader(shader);
  operation->AddArrayByIndex(array, 0);
  pipeline->AddOperation(operation);
  
  system->Run();
}

