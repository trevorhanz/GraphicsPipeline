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

//! \file Window.h

#ifndef __GP_WINDOW_H__
#define __GP_WINDOW_H__

#include "Types.h"

namespace GP
{
  /*!
   * Base class for a top-level window.
   */
  class Window
  {
  protected:
    //! Constructor \param windowData Pointer to internal data provided by derived class
    Window(UserDataPtr windowData);
    
  public:
    //! Destructor
    virtual ~Window();
    
    /*!
     * Get unerlying render target.
     * \return Target.
     */
    TargetPtr GetTarget();
    
    /*!
     * Get Window width in pixels.
     * \return Width in pixels.
     */
    unsigned int GetWidth();
    
    /*!
     * Get Window height in pixels.
     * \return Height in pixels.
     */
    unsigned int GetHeight();
    
    UserDataPtr           mUserData;
  };
  //! Shared pointer to Window object.
  typedef std::shared_ptr<Window> WindowPtr;
}

#endif // __GP_WINDOW_H__
