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

#ifndef __GP_OPERATIONS_H__
#define __GP_OPERATIONS_H__

#include <vector>
#include <string>

#include "Pipeline.h"

namespace GP
{
  /*
   * Base class describing a pipeline operation
   */
  class Operation
  {
  public:
    Operation();
    virtual ~Operation();
    
  public:
    virtual void Execute(Pipeline::Context* context) {};
    
  private:
    UserDataPtr                   mUserData;
    
    friend class Pipeline;
  };
  typedef std::shared_ptr<Operation> OperationPtr;
  
  /*
   * Operation to load numerical data into an Array
   */
  class LoadArray : public Operation
  {
  public:
    LoadArray();
    
    void SetArray(ArrayPtr array);
    void SetData(ArrayDataPtr data);
    
  public:
    void Execute(Pipeline::Context* context) override;
    
  private:
    ArrayPtr                mArray;
    ArrayDataPtr            mData;
  };
  typedef std::shared_ptr<LoadArray> LoadArrayPtr;
  
  /*
   * Operation to load a Shader
   */
  class LoadShader : public Operation
  {
  public:
    LoadShader();
    
    void SetShader(ShaderPtr shader);
    void SetVertexCode(const std::string& code);
    void SetFragmentCode(const std::string& code);
    
  public:
    void Execute(Pipeline::Context* context) override;
    
  private:
    ShaderPtr                 mShader;
    std::string               mVertex;
    std::string               mFragment;
  };
  typedef std::shared_ptr<LoadShader> LoadShaderPtr;
  
  /*
   * Operation to draw
   */
  class Draw : public Operation
  {
  public:
    Draw();
    
    void SetShader(ShaderPtr shader);
    void SetArray(ArrayPtr array);
    
  public:
    void Execute(Pipeline::Context* context) override;
    
  private:
    ShaderPtr                 mShader;
    ArrayPtr                  mArray;
  };
  typedef std::shared_ptr<Draw> DrawPtr;
  
  /*
   * Operation to set Target for future operations
   */
  class TargetFocus : public Operation
  {
  public:
    void SetTarget(TargetPtr target);
    
  public:
    void Execute(Pipeline::Context* context) override;
    
  private:
    TargetPtr                 mTarget;
  };
  typedef std::shared_ptr<TargetFocus> TargetFocusPtr;
  
  /*
   * Operation to clear data off of the Target
   */
  class Clear : public Operation
  {
  public:
    void Execute(Pipeline::Context* context) override;
  };
  typedef std::shared_ptr<Clear> ClearPtr;
}

#endif // __GP_OPERATIONS_H__
