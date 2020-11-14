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

//! \file Context.h

#ifndef __GP_CONTEXT_H__
#define __GP_CONTEXT_H__

// #include <memory>

#include "Pipeline.h"
#include "Types.h"
#include "Target.h"
#include "Array.h"
#include "Shader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _gp_context gp_context;

void gp_context_free(gp_context* context);
gp_target* gp_context_target_new(gp_context* context);
gp_array* gp_context_array_new(gp_context* context);
gp_shader* gp_context_shader_new(gp_context* context);

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  class Context
  {
  private:
    inline Context(gp_context* context);
  public:
    inline ~Context();
    
    inline Target* CreateTarget();
    
    inline Array* CreateArray();
    
    inline Shader* CreateShader();
    
  private:
    gp_context*           mContext;
    
    friend class System;
  };
  
  /*
   * Implementation
   */
  Context::Context(gp_context* context) : mContext(context) {}
  Context::~Context() {gp_context_free(mContext);}
  Target* Context::CreateTarget() {return new Target(gp_context_target_new(mContext));}
  Array* Context::CreateArray() {return new Array(gp_context_array_new(mContext));}
  Shader* Context::CreateShader() {return new Shader(gp_context_shader_new(mContext));}
}
#endif

#endif // __GP_CONTEXT_H__
