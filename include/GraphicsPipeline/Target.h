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
#include "Context.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Target
 * \{
 */

/*!
 * Create a new gp_target object tied to a context.
 * \param context Context object used to create target.
 * \return Newly created target.
 */
GP_EXPORT gp_target* gp_target_new(gp_context* context);

/*!
 * Increase target object reference count.
 * \param target Target object for which to increase the ref count.
 */
GP_EXPORT void gp_target_ref(gp_target* target);

/*!
 * Decrease target object reference count.
 * \param target Target object for which to decrease the ref count.
 */
GP_EXPORT void gp_target_unref(gp_target* target);

/*!
 * Get the gp_pipeline tied to a gp_target.
 * \param target Pointer to target object.
 * \return Pointer to pipeline object.
 */
GP_EXPORT gp_pipeline* gp_target_get_pipeline(gp_target* target);

/*!
 * Schedule the target to be redrawn.
 * \param target Pointer to target object.
 */
GP_EXPORT void gp_target_redraw(gp_target* target);

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
  protected:
    //! Constructor
    inline Target(gp_target* target);
    
  public:
    //! Constructor
    inline Target(const Context& context);
    
    //! Copy Constructor
    inline Target(const Target& other);
    
    //! Destructor
    inline ~Target();
    
    /*!
     * Retrieve GP::Pipeline object.
     * \return GP::Pipeline used by this target.
     */
    inline Pipeline GetPipeline();
    
    /*!
     * Schedules the target to be redrawn.
     */
    inline void Redraw();
    
    //! Equal operator
    inline const Target& operator = (const Target& other);
    
  protected:
    inline static gp_context* GetContext(const Context& context);
    
  private:
    gp_target*        mTarget;
  };
  
  //
  // Implementation
  //
  Target::Target(gp_target* target) : mTarget(target) {}
  Target::Target(const Context& context) : mTarget(gp_target_new(context.mContext)) {}
  Target::Target(const Target& other)
  {
    mTarget = other.mTarget;
    gp_target_ref(mTarget);
  }
  Target::~Target() {gp_target_unref(mTarget);}
  Pipeline Target::GetPipeline() {return Pipeline(gp_target_get_pipeline(mTarget));}
  void Target::Redraw() {gp_target_redraw(mTarget);}
  const Target& Target::operator = (const Target& other)
  {
    gp_target_unref(mTarget);
    mTarget = other.mTarget;
    gp_target_ref(mTarget);
    return *this;
  }
  gp_context* Target::GetContext(const Context& context) {return context.mContext;}
}

#endif // __cplusplus

#endif // __GP_TARGET_H__
