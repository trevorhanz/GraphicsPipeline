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

//! \file Operations.h

#ifndef __GP_OPERATIONS_H__
#define __GP_OPERATIONS_H__

#include <vector>
#include <string>

#include "Pipeline.h"

namespace GP
{
  /*!
   * Base class describing a Pipeline operation
   */
  class Operation
  {
  public:
    //! Constructor
    Operation();
    //! Descructor
    virtual ~Operation();
    
  public:
    /*!
     * Executes the operation implemented in the derived classes.
     * This function should only be called by a Pipeline.
     * \param context The Pipeline::Context on which the operation will be performed.
     */
    virtual void Execute(Pipeline::Context* context) {};
    
  private:
    UserDataPtr                   mUserData;
    
    friend class Pipeline;
  };
  //! Shared pointer to Operation object.
  typedef std::shared_ptr<Operation> OperationPtr;
  
  /*!
   * Operation to load numerical data into an Array.
   * When executed, data stored in an ArrayData obect will be
   * loaded into an Array object.  This operation may be
   * preformed asynchronously, so the ArrayData should not
   * be continued to be used.
   */
  class LoadArray : public Operation
  {
  public:
    //! Constructor
    LoadArray();
    
    /*!
     * Sets the Array for which data will be loaded.
     * \param array Array to be used.
     */
    void SetArray(ArrayPtr array);
    
    /*!
     * Sets the ArrayData for which will be loaded into the Array
     * \param data ArrayData to be used
     */
    void SetData(ArrayDataPtr data);
    
  public:
    void Execute(Pipeline::Context* context) override;
    
  private:
    ArrayPtr                mArray;
    ArrayDataPtr            mData;
  };
  //! Shared pointer to LoadArray object.
  typedef std::shared_ptr<LoadArray> LoadArrayPtr;
  
  /*!
   * Operation to load a Shader.
   */
  class LoadShader : public Operation
  {
  public:
    //! Constructor
    LoadShader();
    
    /*!
     * Sets the Shader for which is to be loaded.
     * \param shader Shader for which is to be loaded.
     */
    void SetShader(ShaderPtr shader);
    
    /*!
     * Sets the coded used in the vertex shader.
     * \param code String containing vertex code.
     */
    void SetVertexCode(const std::string& code);
    
    /*!
     * Sets the coded used in the fragment shader.
     * \param code String containing fragment code.
     */
    void SetFragmentCode(const std::string& code);
    
  public:
    void Execute(Pipeline::Context* context) override;
    
  private:
    ShaderPtr                 mShader;
    std::string               mVertex;
    std::string               mFragment;
  };
  //! Shared pointer to LoadShader object.
  typedef std::shared_ptr<LoadShader> LoadShaderPtr;
  
  /*!
   * Operation to draw.
   */
  class Draw : public Operation
  {
  public:
    //! Constructor
    Draw();
    
    /*!
     * Sets the Shader for which will be used to render.
     * \param shader Shader to be used.
     */
    void SetShader(ShaderPtr shader);
    
    /*!
     * Sets the Array for which will be used to render.
     * \param array Array to be used.
     */
    void SetArray(ArrayPtr array);
    
  public:
    void Execute(Pipeline::Context* context) override;
    
  private:
    ShaderPtr                 mShader;
    ArrayPtr                  mArray;
  };
  //! Shared pointer to Draw object.
  typedef std::shared_ptr<Draw> DrawPtr;
  
  /*!
   * Operation to set Target for future operations.
   */
  class TargetFocus : public Operation
  {
  public:
    /*!
     * Sets the Target to be focused.
     * \param target Target to be focused.
     */
    void SetTarget(TargetPtr target);
    
  public:
    void Execute(Pipeline::Context* context) override;
    
  private:
    TargetPtr                 mTarget;
  };
  //! Shared pointer to TargetFocus object.
  typedef std::shared_ptr<TargetFocus> TargetFocusPtr;
  
  /*!
   * Operation to clear data off of the Target.
   * Clears both color and depth buffer.
   */
  class Clear : public Operation
  {
  public:
    void Execute(Pipeline::Context* context) override;
  };
  //! Shared pointer to Clear object.
  typedef std::shared_ptr<Clear> ClearPtr;
}

#endif // __GP_OPERATIONS_H__
