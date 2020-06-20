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
#include "Web.h"

#include <emscripten/bind.h>

using namespace GP::Web;

int main(int argc, char* argv[])
{
}

EMSCRIPTEN_BINDINGS(GP) {
  emscripten::class_<GP::Target>("_Target")
    .smart_ptr_constructor("_Target", &std::make_shared<GP::Target>);
  
  emscripten::class_<Target, emscripten::base<GP::Target>>("Target")
    .smart_ptr_constructor("Target", &std::make_shared<Target, std::string>);
  
  emscripten::class_<GP::Array>("Array")
    .smart_ptr_constructor("Array", &std::make_shared<GP::Array>);
  
  emscripten::class_<GP::ArrayData>("ArrayData")
    .smart_ptr_constructor("ArrayData", &std::make_shared<GP::ArrayData>);
  
  emscripten::class_<GP::Shader>("Shader")
    .smart_ptr_constructor("Shader", &std::make_shared<GP::Shader>);
  
  emscripten::class_<GP::Pipeline>("Pipeline")
    .smart_ptr_constructor("Pipeline", &std::make_shared<GP::Pipeline>)
    .function("AddOperation", &GP::Pipeline::AddOperation)
    .function("Execute", &GP::Pipeline::Execute);
  
  emscripten::class_<Context>("Context")
    .smart_ptr_constructor("Context", &std::make_shared<Context>)
    .function("CreatePipeline", &Context::CreatePipeline);
  
  emscripten::class_<GP::Operation>("Operation")
    .smart_ptr<std::shared_ptr<GP::Operation>>("Operation");
  
  emscripten::class_<GP::LoadArray, emscripten::base<GP::Operation>>("LoadArray")
    .smart_ptr_constructor("LoadArray", &std::make_shared<GP::LoadArray>)
    .function("SetArray", &GP::LoadArray::SetArray)
    .function("SetData", &GP::LoadArray::SetData);
  
  emscripten::class_<GP::LoadShader, emscripten::base<GP::Operation>>("LoadShader")
    .smart_ptr_constructor("LoadShader", &std::make_shared<GP::LoadShader>)
    .function("SetShader", &GP::LoadShader::SetShader)
    .function("SetVertexCode", &GP::LoadShader::SetVertexCode)
    .function("SetFragmentCode", &GP::LoadShader::SetFragmentCode);
  
  emscripten::class_<GP::Draw, emscripten::base<GP::Operation>>("Draw")
    .smart_ptr_constructor("Draw", &std::make_shared<GP::Draw>)
    .function("SetShader", &GP::Draw::SetShader)
    .function("SetArray", &GP::Draw::SetArray);
  
  emscripten::class_<GP::TargetFocus, emscripten::base<GP::Operation>>("TargetFocus")
    .smart_ptr_constructor("TargetFocus", &std::make_shared<GP::TargetFocus>)
    .function("SetTarget", &GP::TargetFocus::SetTarget);
}
