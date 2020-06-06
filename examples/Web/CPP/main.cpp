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

#include <GraphicsPipeline.h>
#include <WebAssembly.h>

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
GLfloat vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int main(int argv, char* argc[])
{
  Context* context = new Web::Context();
  
  TargetPtr target = std::make_shared<Web::Target>("#canvas1");
  
  ArrayPtr array = std::make_shared<Array>();
  ShaderPtr shader = std::make_shared<Shader>();
  
  ArrayDataPtr data = std::make_shared<ArrayData>();
  data->SetData(vertexData, 6);
  
  PipelinePtr loadPipe = context->CreatePipeline();
  LoadArrayPtr lap = std::make_shared<LoadArray>();
  lap->SetArray(array);
  lap->SetData(data);
  loadPipe->AddOperation(lap);
  LoadShaderPtr lsp = std::make_shared<LoadShader>();
  lsp->SetShader(shader);
  lsp->SetVertexCode(vertexSource);
  lsp->SetFragmentCode(fragmentSource);
  loadPipe->AddOperation(lsp);
  loadPipe->Execute();
  
  PipelinePtr pipeline = context->CreatePipeline();
  ClearPtr clear = std::make_shared<Clear>();
  pipeline->AddOperation(clear);
  DrawPtr draw = std::make_shared<Draw>();
  draw->SetShader(shader);
  draw->SetArray(array);
  pipeline->AddOperation(draw);
  
  pipeline->Execute();
}

