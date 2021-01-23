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

#include "Common.h"
#include "Types.h"
#include "Shader.h"
#include "Array.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Pipeline
 * \{
 */

typedef unsigned int gp_draw_mode;
extern const gp_draw_mode GP_MODE_TRIANGLES;
extern const gp_draw_mode GP_MODE_TRIANGLE_STRIP;

/*!
 * Create a new clear operation.
 * \return Pointer to new operation.
 */
GP_EXPORT gp_operation* gp_operation_clear_new();

/*!
 * Creates a new draw operation.
 * \return Pointer to new operation.
 */
GP_EXPORT gp_operation* gp_operation_draw_new();

/*!
 * Sets the shader program to be used for this draw operation.
 * \param operation Draw operation to set the shader to.
 * \param shader New shader program to be used.
 */
GP_EXPORT void gp_operation_draw_set_shader(gp_operation* operation, gp_shader* shader);

/*!
 * Sets a uniform object to be used.
 * \param operation Draw operation for which to apply the uniform object.
 * \param uniform New uniform object to be used.
 */
GP_EXPORT void gp_operation_draw_set_uniform(gp_operation* operation, gp_uniform* uniform);

/*!
 * Add an array object to be used in this draw operation.
 * Array will be attached to shader program by layout index value.
 * \param operation Draw operation to add the array object to.
 * \param array New array object to be added.
 * \param index Layout index used to attach the array object.
 * \param components Number of components for each element.
 * \param stride Byte offset between consecutive elements.
 * \param offset Byte offset of first element.
 */
GP_EXPORT void gp_operation_draw_add_array_by_index(gp_operation* operation, gp_array* array, int index, int components, int stride, int offset);

/*!
 * Set the number of verticies in the draw operation.
 * \param operation Draw operation for which to set the vertex count.
 * \param count The number of verticies in the draw operation.
 */
GP_EXPORT void gp_operation_draw_set_verticies(gp_operation* operation, int count);

/*!
 * Set the draw mode for a draw operation.
 * \param operation Draw operation for which to change the draw mode.
 * \param mode The drawing mode to use.
 */
GP_EXPORT void gp_operation_draw_set_mode(gp_operation* operation, gp_draw_mode mode);

/*!
  * Add an operation to the end of the pipeline.
  * \param pipeline Pipeline to add operation to.
  * \param operation Operation to be added.
  */
GP_EXPORT void gp_pipeline_add_operation(gp_pipeline* pipeline, gp_operation* operation);

/*!
 * Remove an operation from the pipeline.
 * \param pipeline Pipeline from which to remove the operation.
 * \param operation operation to be removed.
 */
GP_EXPORT void gp_pipeline_remove_operation(gp_pipeline* pipeline, gp_operation* operation);

//! \} // Pipeline

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_operation
   */
  class Operation
  {
  protected:
    //! Constructor
    inline Operation(gp_operation* operation);
    
    gp_operation*         mOperation;
    
    friend class Pipeline;
  };
  
  /*!
   * \brief Extended class for accessing clear operation functions.
   */
  class ClearOperation : public Operation
  {
  public:
    //! Constructor
    inline ClearOperation();
  };
  
  /*!
   * \brief Extended class for accessing draw operation functions.
   */
  class DrawOperation : public Operation
  {
  public:
    //! Constructor
    inline DrawOperation();
    
    /*!
     * Sets the shader program to be used for this draw operation.
     * \param shader Shader object to be used.
     */
    inline void SetShader(Shader* shader);
    
    /*!
     * Sets a uniform object to be used.
     * \param uniform New uniform object to be used.
     */
    inline void SetUniform(Uniform* uniform);
    
    /*!
     * Add an array object to be used in this draw operation.
     * Array will be attached to shader program by layout index value.
     * \param array New array object to be added.
     * \param index Layout index used to attach the array object.
     * \param components Number of components for each element.
     * \param stride Byte offset between consecutive elements.
     * \param offset Byte offset of first element.
     */
    inline void AddArrayByIndex(Array* array, int index, int components, int stride = 0, int offset = 0);
    
    /*!
     * Set the number of verticies in the draw operation.
     * \param count The number of verticies in the draw operation.
     */
    inline void SetVerticies(int count);
    
    /*!
     * Set the draw mode for a draw operation.
     * \param mode The drawing mode to use.
     */
    inline void SetMode(gp_draw_mode mode);
  };
  
  /*!
   * \brief Wrapper class for ::gp_pipeline 
   */
  class Pipeline
  {
  public:
    //! Constructor
    inline Pipeline(gp_pipeline* pipeline);
    //! Destructor
    inline ~Pipeline();
    
    /*!
     * Add draw operation to the end of this pipeline object.
     * \param shader Shader object to be used in operation.
     * \param array Array object to be used in operation.
     */
    inline void AddOperation(Operation* operation);
    
  private:
    gp_pipeline*          mPipeline;
  };
  
  //
  // Implementation
  //
  Operation::Operation(gp_operation* operation) : mOperation(operation) {}
  ClearOperation::ClearOperation() : Operation(gp_operation_clear_new()) {}
  DrawOperation::DrawOperation() : Operation(gp_operation_draw_new()) {}
  void DrawOperation::SetShader(Shader* shader)
  {
    gp_operation_draw_set_shader(mOperation, shader->mShader);
  }
  void DrawOperation::SetUniform(Uniform* uniform)
  {
    gp_operation_draw_set_uniform(mOperation, uniform->mUniform);
  }
  void DrawOperation::AddArrayByIndex(Array* array, int index, int components, int stride, int offset)
  {
    gp_operation_draw_add_array_by_index(mOperation, array->mArray, index, components, stride, offset);
  }
  void DrawOperation::SetVerticies(int count) {gp_operation_draw_set_verticies(mOperation, count);}
  void DrawOperation::SetMode(gp_draw_mode mode) {gp_operation_draw_set_mode(mOperation, mode);}
  
  Pipeline::Pipeline(gp_pipeline* pipeline) : mPipeline(pipeline) {}
  Pipeline::~Pipeline() {}
  void Pipeline::AddOperation(Operation* operation)
  {
    gp_pipeline_add_operation(mPipeline, operation->mOperation);
  }
}
#endif // __cplusplus

#endif // __GP_PIPELINE_H__
