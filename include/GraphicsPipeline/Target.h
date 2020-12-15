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

//! \file Target.h

#ifndef __GP_TARGET_H__
#define __GP_TARGET_H__

#include "Common.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Target
 * \{
 */

/*!
 * Get the gp_pipeline tied to a gp_target.
 * \param target Pointer to target object.
 * \return Pointer to pipeline object.
 */
  GP_EXPORT gp_pipeline* gp_target_get_pipeline(gp_target* target);

//! \} // Target

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_target.
   */
  class Target
  {
  public:
    //! Constructor
    inline Target(gp_target* target);
    
    //! Destructor
    inline ~Target();
    
    /*!
     * Retrieve GP::Pipeline object.
     * \return Pointer to GP::Pipeline.
     */
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
