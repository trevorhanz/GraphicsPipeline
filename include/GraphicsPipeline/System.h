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

//! \file System.h

#ifndef __GP_SYSTEM_H__
#define __GP_SYSTEM_H__

#include "Common.h"
#include "Context.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Create a new system object.
 * \return Pointer to new system object.
 */
GP_EXPORT gp_system* gp_system_new();

/*!
 * Free system object.
 * \param system Pointer to system object to be freed.
 */
GP_EXPORT void gp_system_free(gp_system* system);

/*!
 * Create a new context object tied to an existing system object.
 * \param system Pointer to existing system object.
 * \return Pointer to new context object.
 */
GP_EXPORT gp_context* gp_system_context_new(gp_system* system);

/*!
 * Runs the main event loop for a system obect.
 * The function will take control of the current thread.
 * \param system Pointer to system object to be run.
 */
GP_EXPORT void gp_system_run(gp_system* system);

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  /*!
   * Wrapper class for gp_system.
   */
  class System
  {
  public:
    //! Constructor
    inline System();
    
    //! Destructor
    inline ~System();
    
    /*!
     * Creates a new GP::Context for this system.
     * \return Pointer to newly created GP::Context.
     */
    inline Context* CreateContext();
    
    /*!
     * Start the main loop.
     */
    inline void Run();
    
  private:
    gp_system*            mSystem;
  };
  
  //
  // Implementation
  //
  System::System() {mSystem = gp_system_new();}
  System::~System() {gp_system_free(mSystem);}
  Context* System::CreateContext() {return new Context(gp_system_context_new(mSystem));}
  void System::Run() {gp_system_run(mSystem);}
}
#endif // __cplusplus

#endif // __GP_SYSTEM_H__
