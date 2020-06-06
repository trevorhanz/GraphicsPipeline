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

#ifndef __GP_SYSTEM_H__
#define __GP_SYSTEM_H__

#include <string>

#include "GraphicsPipeline.h"
#include "Types.h"

namespace GP
{
  /*
   * Object for managing a system window
   */
  class Window
  {
  public:
    class Init;
    Window(Init* init);
    virtual ~Window();
    
    TargetPtr GetTarget();
    
  private:
    class Data;
    Data*           mData;
  };
  typedef std::shared_ptr<Window> WindowPtr;
  
  /*
   * Top-Level system object
   */
  class System
  {
  public:
    System();
    ~System();
    
    WindowPtr CreateWindow(const std::string& title, int width, int height);
    
    ContextPtr CreateContext();
    
    void Poll();
    
    void Run();
    
  private:
    class Data;
    Data*           mData;
  };
};


#endif // __GP_SYSTEM_H__
