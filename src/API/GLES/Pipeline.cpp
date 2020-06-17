
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

#include "Pipeline.h"
#include <Operations.h>

#include <iostream>

#include <GLES2/gl2.h>

using namespace GP::GLES;
using namespace std;

class ArrayUserData : public GP::UserData
{
public:
  ArrayUserData() : mVBO(0) {}
  virtual ~ArrayUserData() {}
  
  GLuint                mVBO;
};
typedef std::shared_ptr<ArrayUserData> ArrayUserDataPtr;

class ShaderUserData : public GP::UserData
{
public:
  ShaderUserData() {}
  virtual ~ShaderUserData() {}
  
  GLuint                mProgram;
  GLint                 mAttribute;
};
typedef std::shared_ptr<ShaderUserData> ShaderUserDataPtr;

Pipeline::Pipeline(GP::Context* context)
  : mContext(context)
{
}

Pipeline::~Pipeline()
{
}

void Pipeline::AddOperation(OperationPtr operation)
{
  mOperations.push_back(operation);
}

void Pipeline::RemoveOperation(OperationPtr operation)
{
  mOperations.remove(operation);
}

void Pipeline::ClearPipeline()
{
  mOperations.clear();
}

void Pipeline::Execute()
{
  auto ctx = CreateContext();
  for(auto op : mOperations)
  {
    op->Execute(ctx);
  }
  delete ctx;
}

Pipeline::Context* Pipeline::CreateContext()
{
  return new Context(this);
}

Pipeline::Context::Context(Pipeline* pipeline)
  : mPipeline(pipeline)
{
}

Pipeline::Context::~Context()
{
}

void Pipeline::Context::SetTarget(GP::TargetPtr target)
{
  mPipeline->mContext->Bind(target);
  
  TargetUserDataPtr ptr = std::dynamic_pointer_cast<TargetUserData>(target->mUserData);
  
  ptr->MakeCurrent();
  
  mCurrentTarget = target;
}

void Pipeline::Context::ClearColor()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Pipeline::Context::ClearDepth()
{
  glClear(GL_DEPTH_BUFFER_BIT);
}

void Pipeline::Context::LoadArray(ArrayPtr array, ArrayDataPtr data)
{
  ArrayUserDataPtr ptr = std::dynamic_pointer_cast<ArrayUserData>(array->mUserData);
  if(!ptr)
  {
    ptr = std::make_shared<ArrayUserData>();
    array->mUserData = ptr;
    glGenBuffers(1, &ptr->mVBO);
  }
  
  glBindBuffer(GL_ARRAY_BUFFER, ptr->mVBO);
  glBufferData(GL_ARRAY_BUFFER, data->GetSize()*sizeof(float), data->GetData(), GL_STATIC_DRAW);
}

void Pipeline::Context::LoadShader(ShaderPtr shader, const char* vertex, const char* fragment)
{
  ShaderUserDataPtr ptr;
  if(!shader->mUserData)
  {
    ptr = std::make_shared<ShaderUserData>();
    shader->mUserData = ptr;
  }
  else ptr = std::static_pointer_cast<ShaderUserData>(shader->mUserData);
  
  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex, nullptr);
  glCompileShader(vertexShader);

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragment, nullptr);
  glCompileShader(fragmentShader);

  // Link the vertex and fragment shader into a shader program
  ptr->mProgram = glCreateProgram();
  glAttachShader(ptr->mProgram, vertexShader);
  glAttachShader(ptr->mProgram, fragmentShader);
  glLinkProgram(ptr->mProgram);
  glUseProgram(ptr->mProgram);
  
  int linkStatus = 0;
  glGetProgramiv(ptr->mProgram, GL_LINK_STATUS, &linkStatus);
  if(!linkStatus)
  {
    int charsWritten  = 0;
    int length;
    glGetProgramiv(ptr->mProgram, GL_INFO_LOG_LENGTH, &length);
    if(length > 0)
    {
      char infoLog[length];
      glGetProgramInfoLog(ptr->mProgram, length, &length, infoLog);
      cerr << "ShaderError: " << infoLog << endl;
    }
  }
  
  ptr->mAttribute = glGetAttribLocation(ptr->mProgram, "position");
}

void Pipeline::Context::SetShader(ShaderPtr shader)
{
  ShaderUserDataPtr ptr = std::dynamic_pointer_cast<ShaderUserData>(shader->mUserData);
  glUseProgram(ptr->mProgram);
  
  ptr->mAttribute = glGetAttribLocation(ptr->mProgram, "position");
  mCurrentShader = shader;
}

void Pipeline::Context::AttachArray(ArrayPtr array, const std::string& name)
{
  ArrayUserDataPtr ptr = std::dynamic_pointer_cast<ArrayUserData>(array->mUserData);
  glBindBuffer(GL_ARRAY_BUFFER, ptr->mVBO);
  
  ShaderUserDataPtr ptr2 = std::dynamic_pointer_cast<ShaderUserData>(mCurrentShader->mUserData);
  // Specify the layout of the vertex data
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Pipeline::Context::Draw()
{
  glDrawArrays(GL_TRIANGLES, 0, 3);
  
  // TODO - Create finalize function
  auto ptr = std::dynamic_pointer_cast<TargetUserData>(mCurrentTarget->mUserData);
  ptr->Present();
}
