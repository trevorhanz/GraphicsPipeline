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

#ifndef __GP_PIPELINE_H__
#define __GP_PIPELINE_H__

#include <memory>

#include "Types.h"

namespace GP
{
  class Operation;
  typedef std::shared_ptr<Operation> OperationPtr;
  
  /*
   * Structure for storing and executing operations
   */
  class Pipeline
  {
  public:
    class Context;
    
    Pipeline() {}
    virtual ~Pipeline() {}
    
    virtual void AddOperation(OperationPtr operation) {};
    virtual void RemoveOperation(OperationPtr operation) {};
    virtual void ClearPipeline() {};
    
    virtual void Execute() {};
  };
  typedef std::shared_ptr<Pipeline> PipelinePtr;
  
  class Pipeline::Context
  {
  public:
    virtual ~Context() {}
    virtual void SetTarget(TargetPtr target) = 0;
    virtual void ClearColor() = 0;
    virtual void ClearDepth() = 0;
    virtual void LoadArray(ArrayPtr array, ArrayDataPtr data) = 0;
    virtual void LoadShader(ShaderPtr shader, const char* vertex, const char* fragment) = 0;
    virtual void SetShader(ShaderPtr shader) = 0;
    virtual void AttachArray(ArrayPtr array, const std::string& name) = 0;
    virtual void Draw() = 0;
  };
}



#endif // __GP_PIPELINE_H__
