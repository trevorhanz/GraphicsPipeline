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
   * Add a draw operation to the end of the pipeline.
   * \param pipeline Pipeline to add operation to.
   * \param shader Shader object used in draw operation.
   * \param array Array object used in draw operation.
   */
  GP_EXPORT void gp_pipeline_add_draw(gp_pipeline* pipeline, gp_shader* shader, gp_array* array);
  
#ifdef __cplusplus
}

namespace GP
{
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
    inline void AddDraw(Shader* shader, Array* array);
    
  private:
    gp_pipeline*          mPipeline;
  };
  
  //
  // Implementation
  //
  Pipeline::Pipeline(gp_pipeline* pipeline) : mPipeline(pipeline) {}
  Pipeline::~Pipeline() {}
  void Pipeline::AddDraw(Shader* shader, Array* array)
  {
    gp_pipeline_add_draw(mPipeline, shader->mShader, array->mArray);
  }
}
#endif // __cplusplus

#endif // __GP_PIPELINE_H__
