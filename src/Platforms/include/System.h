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

#include <string>

#include "GraphicsPipeline.h"
#include "Types.h"

namespace GP
{
  /*!
   * Top-Level system object.
   * Abstraction for OS specific window system and event loop.
   */
  class System
  {
  public:
    //! Constructor
    System();
    //! Destructor
    ~System();
    
    /*!
     * Create a new Window object for this system.
     * \param title Window title.
     * \param width Window width in pixels.
     * \param height Window height in pixels.
     * \return Pointer to Window object.
     */
    WindowPtr CreateWindow(const std::string& title, int width, int height);
    
    /*!
     * Creates a new Context bound to this System.
     * \return Pointer to new Context.
     */
    ContextPtr CreateContext();
    
    /*!
     * Process pending events.
     */
    void Poll();
    
    /*!
     * Process all events in a loop.
     */
    void Run();
    
  private:
    class Data;
    Data*           mData;
  };
};


#endif // __GP_SYSTEM_H__
