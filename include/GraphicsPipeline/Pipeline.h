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

//! \file Pipeline.h

#ifndef __GP_PIPELINE_H__
#define __GP_PIPELINE_H__

#include <memory>

#include "Types.h"

namespace GP
{
  class Operation;
  typedef std::shared_ptr<Operation> OperationPtr;
  
  /*!
   * Structure for storing and executing operations.
   * Each Pipeline is bound to a single Context and may share
   * any resources bound to the same Context.  Any resources 
   * not yet bound to a Context will be bound during execution.
   */
  class Pipeline
  {
  public:
    class Context;
    
    //! Constructor
    Pipeline() {}
    //! Destructor
    virtual ~Pipeline() {}
    
    /*!
     * Adds Operation to be executed.
     * \param operation Operation to be added.
     */
    virtual void AddOperation(OperationPtr operation) {};
    
    /*!
     * Removes Operation from execution list.
     * \param operation Operation to be removed.
     */
    virtual void RemoveOperation(OperationPtr operation) {};
    
    /*!
     * Clears all stored operations.
     */
    virtual void ClearPipeline() {};
    
    /*!
     * Starts execution of stored operations.
     */
    virtual void Execute() {};
  };
  //! Shared pointer to Pipeline object.
  typedef std::shared_ptr<Pipeline> PipelinePtr;
  
  /*!
   * Execution context of a Pipeline.
   * This contains all necessary functions to execute any Operation.
   */
  class Pipeline::Context
  {
  public:
    //! Destructor
    virtual ~Context() {}
    
    /*!
     * Sets the Target to be used for future operations.
     * \param target Target to be set.
     */
    virtual void SetTarget(TargetPtr target) = 0;
    
    /*!
     * Clears the color buffer from the current Target.
     */
    virtual void ClearColor() = 0;
    
    /*!
     * Clears the depth buffer from the current Target.
     */
    virtual void ClearDepth() = 0;
    
    /*!
     * Loads data from ArrayData into an Array.
     * \param array Array to be loaded.
     * \param data ArrayData for which to load.
     */
    virtual void LoadArray(ArrayPtr array, ArrayDataPtr data) = 0;
    
    /*!
     * Loads vertex and fragment coded into a Shader.
     * \param shader Shader to be loaded.
     * \param vertex String containing vertex code.
     * \param fragment String containing fragment code.
     */
    virtual void LoadShader(ShaderPtr shader, const char* vertex, const char* fragment) = 0;
    
    /*!
     * Sets the Shader to be used for furture operations.
     * \param shader Shader to be set.
     */
    virtual void SetShader(ShaderPtr shader) = 0;
    
    /*!
     * Prepares an Array to be used for a draw operation by binding it to a Shader attribute.
     * \param array Array to be bound.
     * \param name String name of the attribute to which will be bound.
     */
    virtual void AttachArray(ArrayPtr array, const std::string& name) = 0;
    
    /*!
     * Performs the draw operation using the current Shader and Array attachments.
     */
    virtual void Draw() = 0;
  };
}



#endif // __GP_PIPELINE_H__
