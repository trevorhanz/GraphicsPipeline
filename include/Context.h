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

#ifndef __GP_CONTEXT_H__
#define __GP_CONTEXT_H__

#include <memory>

#include "Pipeline.h"
#include "Types.h"

namespace GP
{
  /*
   * Central class for providing rendering support
   * Must be overridden to provide functionality.
   */
  class Context
  {
  public:
    Context() {}
    virtual PipelinePtr CreatePipeline() = 0;
    
    virtual void Bind(TargetPtr target) = 0;
  };
  typedef std::shared_ptr<Context> ContextPtr;
};

#endif // __GP_CONTEXT_H__
