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

#include <WebAssembly.h>

#include <assert.h>
#include <emscripten/bind.h>

#include <string>

using namespace GP::Web;

Target::Target(std::string selector)
{
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;
  
  mContext = emscripten_webgl_create_context(selector.c_str(), &attrs);
  
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(mContext);
  assert(res == EMSCRIPTEN_RESULT_SUCCESS);
  assert(emscripten_webgl_get_current_context() == mContext);
}

class ArrayUserData : public GP::UserData
{
public:
  ArrayUserData() : mVBO(0) {}
  virtual ~ArrayUserData() {emscripten_log(EM_LOG_CONSOLE, "Free ArrayUserData");}
  
  GLuint                mVBO;
};
typedef std::shared_ptr<ArrayUserData> ArrayUserDataPtr;

class ShaderUserData : public GP::UserData
{
public:
  ShaderUserData() {}
  virtual ~ShaderUserData() {emscripten_log(EM_LOG_CONSOLE, "Free ShaderUserData");}
  
  GLuint                mProgram;
  GLint                 mAttribute;
};
typedef std::shared_ptr<ShaderUserData> ShaderUserDataPtr;

void Pipeline::AddOperation(OperationPtr operation)
{
//   mTargets.push_back(std::dynamic_pointer_cast<Web::Target>(target));
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

static uint32_t start = 0;
void Pipeline::Execute()
{
  auto ctx = new Context();
  for(auto op : mOperations)
  {
    op->Execute(ctx);
  }
  delete ctx;
}

void Pipeline::Context::SetTarget(GP::TargetPtr target)
{
  if(mCurrentTarget == target) return;
  mCurrentTarget = target;
  
  TargetPtr t = std::dynamic_pointer_cast<Web::Target>(target);
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(t->mContext);
  assert(res == EMSCRIPTEN_RESULT_SUCCESS);
  assert(emscripten_webgl_get_current_context() == t->mContext);
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
      emscripten_log(EM_LOG_CONSOLE, "ShaderError: %s\n", infoLog);
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
}

Context::Context()
{
}

GP::PipelinePtr Context::CreatePipeline()
{
  return std::make_shared<Pipeline>();
}

// EMSCRIPTEN_BINDINGS(GP) {
//   emscripten::class_<GP::Target>("_Target")
//     .smart_ptr_constructor("_Target", &std::make_shared<GP::Target>);
//   
//   emscripten::class_<Target, emscripten::base<GP::Target>>("Target")
//     .smart_ptr_constructor("Target", &std::make_shared<Target, std::string>);
//   
//   emscripten::class_<GP::Array>("Array")
//     .smart_ptr_constructor("Array", &std::make_shared<GP::Array>);
//   
//   emscripten::class_<GP::ArrayData>("ArrayData")
//     .smart_ptr_constructor("ArrayData", &std::make_shared<GP::ArrayData>);
//   
//   emscripten::class_<GP::Shader>("Shader")
//     .smart_ptr_constructor("Shader", &std::make_shared<GP::Shader>);
//   
//   emscripten::class_<GP::Pipeline>("Pipeline")
//     .smart_ptr_constructor("Pipeline", &std::make_shared<GP::Pipeline>)
//     .function("AddOperation", &GP::Pipeline::AddOperation)
//     .function("Execute", &GP::Pipeline::Execute);
//   
//   emscripten::class_<Context>("Context")
//     .smart_ptr_constructor("Context", &std::make_shared<Context>)
//     .function("CreatePipeline", &Context::CreatePipeline);
//   
//   emscripten::class_<GP::Operation>("Operation")
//     .smart_ptr<std::shared_ptr<GP::Operation>>("Operation");
//   
//   emscripten::class_<GP::LoadArray, emscripten::base<GP::Operation>>("LoadArray")
//     .smart_ptr_constructor("LoadArray", &std::make_shared<GP::LoadArray>)
//     .function("SetArray", &GP::LoadArray::SetArray)
//     .function("SetData", &GP::LoadArray::SetData);
//   
//   emscripten::class_<GP::LoadShader, emscripten::base<GP::Operation>>("LoadShader")
//     .smart_ptr_constructor("LoadShader", &std::make_shared<GP::LoadShader>)
//     .function("SetShader", &GP::LoadShader::SetShader)
//     .function("SetVertexCode", &GP::LoadShader::SetVertexCode)
//     .function("SetFragmentCode", &GP::LoadShader::SetFragmentCode);
//   
//   emscripten::class_<GP::Draw, emscripten::base<GP::Operation>>("Draw")
//     .smart_ptr_constructor("Draw", &std::make_shared<GP::Draw>)
//     .function("SetShader", &GP::Draw::SetShader)
//     .function("SetArray", &GP::Draw::SetArray);
//   
//   emscripten::class_<GP::TargetFocus, emscripten::base<GP::Operation>>("TargetFocus")
//     .smart_ptr_constructor("TargetFocus", &std::make_shared<GP::TargetFocus>)
//     .function("SetTarget", &GP::TargetFocus::SetTarget);
// }
