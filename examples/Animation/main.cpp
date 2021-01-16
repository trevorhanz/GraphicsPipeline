
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

using namespace GP;

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

int main(int argc, char* argv[])
{
  System* system = new System();
  
  Context* context = system->CreateContext();
  
  Target* target = context->CreateTarget();
  Array* array = context->CreateArray();
  Shader* shader = context->CreateShader();
  Uniform* uniform;
  
  int frame = 0;
  Timer* timer = system->CreateTimer();
  timer->SetCallback([&](Timer* timer)
  {
    timer->Arm(TIMEOUT);
    
    float offset = frame%200/200.0;
    if(offset > .5) offset = 1.0-offset;
    offset -= .25;
    
    float o[] = {offset, 0.0f};
    uniform->SetVec2(o);
    
    target->Redraw();
    
    ++frame;
  });
  timer->Arm(TIMEOUT);
  
  array->SetData(vertexData, 6);
  
  shader->Compile(vertexSource, fragmentSource);
  
  float o[] = {0.0f, 0.0f};
  uniform = shader->CreateUniform("Offset");
  uniform->SetVec2(o);
  
  Pipeline* pipeline = target->GetPipeline();
  
  pipeline->AddOperation(new ClearOperation());
  
  DrawOperation* operation = new DrawOperation();
  operation->SetShader(shader);
  operation->SetUniform(uniform);
  operation->AddArrayByIndex(array, 0, 2);
  pipeline->AddOperation(operation);
  
  system->Run();
  
  return EXIT_SUCCESS;
}
