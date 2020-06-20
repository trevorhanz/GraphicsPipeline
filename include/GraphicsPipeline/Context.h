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

#include <memory>

#include "Pipeline.h"
#include "Types.h"

namespace GP
{
  /*!
   * Central class for providing rendering support.
   * Must be overridden to provide functionality.
   */
  class Context
  {
  public:
    /*!
     * Default contructor.
     */
    Context() {}
    
    /*!
     *  Creates a Pipeline tied to this context.
     *  \return A pointer to the newly create Pipeline
     */
    virtual PipelinePtr CreatePipeline() = 0;
    
    /*!
     * Binds a target to this context.
     * Targets can only be bound to a single Context.  Targets must
     * be bound to a Context before they can be used in a Pipeline.
     * If the Target hasn't been bound to a context before it is used
     * in a Pipeline, the Pipeline to bind it to its context.
     * \param target The Target to be bound
     */
    virtual void Bind(TargetPtr target) = 0;
  };
  typedef std::shared_ptr<Context> ContextPtr;
};

#endif // __GP_CONTEXT_H__
