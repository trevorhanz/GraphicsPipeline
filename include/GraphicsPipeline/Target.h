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

#ifndef __GP_TARGET_H__
#define __GP_TARGET_H__

#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif
  
  gp_pipeline* gp_target_get_pipeline(gp_target* target);
  
#ifdef __cplusplus
}

namespace GP
{
  class Target
  {
  public:
    inline Target(gp_target* target);
    inline ~Target();
    
    inline Pipeline* GetPipeline();
    
  private:
    gp_target*        mTarget;
    Pipeline*         mPipeline;
    
    friend class Context;
  };
  
  //
  // Implementation
  //
  Target::Target(gp_target* target) : mTarget(target), mPipeline(new Pipeline(gp_target_get_pipeline(target))) {}
  Target::~Target() {}
  Pipeline* Target::GetPipeline() {return mPipeline;}
}

#endif // __cplusplus

#endif // __GP_TARGET_H__
