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

//! \file Web.h

#ifndef __GP_WEB_H__
#define __GP_WEB_H__

namespace GP
{
  namespace Web
  {
    /*!
     * Window with web specific features.
     */
    class Window : public GP::Window
    {
    public:
      /*!
       * Constructor.
       * \param id HTML id of canvas element.
       */
      Window(const std::string& id);
    };
    //! Shared pointer to Window object.
    typedef std::shared_ptr<Window> WindowPtr;
  }
}

#endif // __GP_WEB_H__