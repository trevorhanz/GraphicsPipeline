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

#include "Context.h"
#include <API/GLES/Pipeline.h>

#include <assert.h>
#include <emscripten/bind.h>

#include <string>

using namespace GP::Web;

class TargetUserData : public GP::GLES::TargetUserData
{
public:
  void MakeCurrent() override {emscripten_webgl_make_context_current(mContext);}
  void Present() override {}
  
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE       mContext;
};

Target::Target(std::string selector)
{
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;
  
  auto context = emscripten_webgl_create_context(selector.c_str(), &attrs);
  
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(context);
  assert(res == EMSCRIPTEN_RESULT_SUCCESS);
  assert(emscripten_webgl_get_current_context() == context);
  
  auto userData = std::make_shared<TargetUserData>();
  userData->mContext = context;
  mUserData = userData;
}

Context::Context()
{
}

GP::PipelinePtr Context::CreatePipeline()
{
  return std::make_shared<GP::GLES::Pipeline>(this);
}

void Context::Bind(GP::TargetPtr target)
{
}
