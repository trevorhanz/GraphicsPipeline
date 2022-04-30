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
#include "Object.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Pipeline
 * \{
 */

/*!
 * Set the rendering priority for an operation.  Higher priorities render
 * first.  The default pririty is 0.
 * \param operation Operation for which to set the rendering priority.
 * \param priority The new rendering priority.
 */
GP_EXPORT void gp_operation_set_priority(gp_operation* operation, int priority);

/*!
 * Retrieve the current rendering priority for an operation.
 * \param operation Operation for which to set the rendering priority.
 * \return The current rendering priority.
 */
GP_EXPORT int gp_operation_get_priority(gp_operation* operation);

/*!
 * Create a new clear operation.
 * \return Pointer to new operation.
 */
GP_EXPORT gp_operation* gp_operation_clear_new();

/*!
 * Sets the color to be used for the clear operation.
 * \param operation Clear operation for which to set the new color.
 * \param r Red channel.
 * \param g Green channel.
 * \param b Blue channel.
 * \param a Alpha channel.
 */
GP_EXPORT void gp_operation_clear_set_color(gp_operation* operation, float r, float g, float b, float a);

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
 * \param type Data type for each element.
 * \param stride Byte offset between consecutive elements.
 * \param offset Byte offset of first element.
 */
GP_EXPORT void gp_operation_draw_add_array_by_index(gp_operation* operation,
                                                    gp_array* array,
                                                    int index,
                                                    int components,
                                                    GP_DATA_TYPE type,
                                                    int stride,
                                                    int offset);

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
GP_EXPORT void gp_operation_draw_set_mode(gp_operation* operation, GP_DRAW_MODE mode);

/*!
 * Create a new viewport operation.
 * \return Newly created viewport operation.
 */
GP_EXPORT gp_operation* gp_operation_viewport_new();

/*!
 * Get the underline pipeline of the viewport.
 * \param operation Viewport operation for which to get the pipeline.
 * \return The pipeline of the viewport operation.
 */
GP_EXPORT gp_pipeline* gp_operation_viewport_get_pipeline(gp_operation* operation);

/*!
 * Set the dimensions of the viewport operation.
 * \param operation Viewport operation for witch to set the dimensions.
 * \param x Distance of Viewport from the left edge of the window in pixels.
 * \param y Distance of Viewport from the bottom edge of the window in pixels.
 * \param width Width of the Viewport in pixels.
 * \param height Height of the Viewport in pixels.
 */
GP_EXPORT void gp_operation_viewport_set_dimesions(gp_operation* operation, int x, int y, int width, int height);

/*!
 * Create a new group operation.
 * \return Newly created group operation.
 */
GP_EXPORT gp_operation* gp_operation_group_new();

/*!
 * Get the underline pipeline of the group.
 * \param operation Group operation for which to get the pipeline.
 * \return The pipeline of the group operation.
 */
GP_EXPORT gp_pipeline* gp_operation_group_get_pipeline(gp_operation* operation);

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

/*!
 * Remove all operations from the pipeline.
 * \param pipeline Pipeline from which to remove all operations.
 */
GP_EXPORT void gp_pipeline_clear(gp_pipeline* pipeline);

//! \} // Pipeline

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_operation
   */
  class Operation : public Object
  {
  public:
    //! Constructor
    inline Operation(gp_operation* operation);
    
    /*!
     * Set the rendering priority for an operation.  Higher priorities render
     * first.
     * \param priority The new rendering priority.
     */
    inline void SetPriority(int priority);
    
    /*!
     * Retrieve the current rendering priority for an operation.
     * \return The current rendering priority.
     */
    inline int GetPriority();
  };
  
  /*!
   * \brief Extended class for accessing clear operation functions.
   */
  class ClearOperation : public Operation
  {
  public:
    //! Constructor
    inline ClearOperation();
    
    /*!
     * Sets the color to be used for the clear operation.
     * \param r Red channel.
     * \param g Green channel.
     * \param b Blue channel.
     * \param a Alpha channel.
     */
    inline void SetColor(float r, float g, float b, float a);
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
    inline void SetShader(const Shader& shader);
    
    /*!
     * Sets a uniform object to be used.
     * \param uniform New uniform object to be used.
     */
    inline void SetUniform(const Uniform& uniform);
    
    /*!
     * Add an array object to be used in this draw operation.
     * Array will be attached to shader program by layout index value.
     * \param array New array object to be added.
     * \param index Layout index used to attach the array object.
     * \param components Number of components for each element.
     * \param type Data type for each emement.
     * \param stride Byte offset between consecutive elements.
     * \param offset Byte offset of first element.
     */
    inline void AddArrayByIndex(const Array& array, int index, int components, GP_DATA_TYPE type = GP_DATA_TYPE_FLOAT, int stride = 0, int offset = 0);
    
    /*!
     * Set the number of verticies in the draw operation.
     * \param count The number of verticies in the draw operation.
     */
    inline void SetVerticies(int count);
    
    /*!
     * Set the draw mode for a draw operation.
     * \param mode The drawing mode to use.
     */
    inline void SetMode(GP_DRAW_MODE mode);
  };
  
  /*!
   * \brief Extended class for accessing viewport operation functions.
   */
  class ViewportOperation : public Operation
  {
  public:
    //! Constructor
    inline ViewportOperation();
    
    /*!
     * Get the underline Pipeline of the viewport.
     * \return The Pipeline of the viewport operation.
     */
    inline Pipeline GetPipeline();
    
    /*!
     * Set the dimensions of the viewport operation.
     * \param x Distance of Viewport from the left edge of the window in pixels.
     * \param y Distance of Viewport from the bottom edge of the window in pixels.
     * \param width Width of the Viewport in pixels.
     * \param height Height of the Viewport in pixels.
     */
    inline void SetDimensions(int x, int y, int width, int height);
  };
  
  /*!
   * \brief Extended class for accessing group operation functions.
   */
  class GroupOperation : public Operation
  {
  public:
    //! Constructor
    inline GroupOperation();
    
    /*!
     * Get the underline Pipeline of the viewport.
     * \return The Pipeline of the viewport operation.
     */
    inline Pipeline GetPipeline();
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
     * Add an operation to the end of this pipeline object.
     * \param operation Operation to be added.
     */
    inline void AddOperation(const Operation& operation);
    
    /*!
     * Remove an operation from the pipeline.
     * \param operation operation to be removed.
     */
    inline void RemoveOperation(const Operation& operation);
    
    /*!
     * Remove all operations from the pipeline.
     */
    inline void Clear();
    
  private:
    gp_pipeline*          mPipeline;
  };
  
  //
  // Implementation
  //
  Operation::Operation(gp_operation* operation) : Object((gp_object*)operation) {}
  void Operation::SetPriority(int priority) {gp_operation_set_priority((gp_operation*)GetObject(*this), priority);}
  int Operation::GetPriority() {return gp_operation_get_priority((gp_operation*)GetObject(*this));}
  
  ClearOperation::ClearOperation() : Operation(gp_operation_clear_new()) {}
  void ClearOperation::SetColor(float r, float g, float b, float a) {
    gp_operation_clear_set_color((gp_operation*)GetObject(*this), r, g, b,  a);
  }
  DrawOperation::DrawOperation() : Operation(gp_operation_draw_new()) {}
  void DrawOperation::SetShader(const Shader& shader)
  {
    gp_operation_draw_set_shader((gp_operation*)GetObject(*this), (gp_shader*)GetObject(shader));
  }
  void DrawOperation::SetUniform(const Uniform& uniform)
  {
    gp_operation_draw_set_uniform((gp_operation*)GetObject(*this), (gp_uniform*)GetObject(uniform));
  }
  void DrawOperation::AddArrayByIndex(const Array& array, int index, int components, GP_DATA_TYPE type, int stride, int offset)
  {
    gp_operation_draw_add_array_by_index((gp_operation*)GetObject(*this), (gp_array*)GetObject(array), index, components, type, stride, offset);
  }
  void DrawOperation::SetVerticies(int count) {gp_operation_draw_set_verticies((gp_operation*)GetObject(*this), count);}
  void DrawOperation::SetMode(GP_DRAW_MODE mode) {gp_operation_draw_set_mode((gp_operation*)GetObject(*this), mode);}
  
  ViewportOperation::ViewportOperation()
    : Operation(gp_operation_viewport_new())
    {}
  Pipeline ViewportOperation::GetPipeline() {return Pipeline(gp_operation_viewport_get_pipeline((gp_operation*)GetObject(*this)));}
  void ViewportOperation::SetDimensions(int x, int y, int width, int height)
  {
    gp_operation_viewport_set_dimesions((gp_operation*)GetObject(*this), x, y, width, height);
  }
  
  GroupOperation::GroupOperation()
    : Operation(gp_operation_group_new())
    {}
  Pipeline GroupOperation::GetPipeline() {return Pipeline(gp_operation_group_get_pipeline((gp_operation*)GetObject(*this)));}
  
  Pipeline::Pipeline(gp_pipeline* pipeline) : mPipeline(pipeline) {}
  Pipeline::~Pipeline() {}
  void Pipeline::AddOperation(const Operation& operation)
  {
    gp_object* op = operation.GetObject();
    gp_pipeline_add_operation(mPipeline, (gp_operation*)op);
    gp_object_unref(op);
  }
  void Pipeline::RemoveOperation(const Operation& operation)
  {
    gp_object* op = operation.GetObject();
    gp_pipeline_remove_operation(mPipeline, (gp_operation*)op);
    gp_object_unref(op);
  }
  void Pipeline::Clear()
  {
    gp_pipeline_clear(mPipeline);
  }
}
#endif // __cplusplus

#endif // __GP_PIPELINE_H__
