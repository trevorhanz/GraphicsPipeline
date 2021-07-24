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

//! \file Qt5.h

#ifndef __GP_QT5_H__
#define __GP_QT5_H__

#include <GraphicsPipeline/GP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Qt5
 * \{
 */

/*!
 * Create a new system object from cli arguments.
 * \param argc Pointer to number of arguments
 * \param argv Array of c strings containing arguments
 * \return Newly created gp_system.
 */
gp_system* gp_qt_system_new(int* argc, char** argv);

/*!
 * Create a new gp_context attached to existing QAplication.
 * \return Newly created gp_context.
 */
gp_context* gp_qt_context_new();

//! \} // Qt5

#ifdef __cplusplus
}

#include <QWidget>

/*!
 * Retrieve QWidget associated with a gp_window.
 * \param window Pointer to gp_window.
 * \return Pointer to QWidget used by window.
 */
QWidget* gp_window_get_qwidget(gp_window* window);

namespace GP
{
namespace Qt
{
  /*!
   * \brief Wrapper class for ::gp_system extended for Qt features.
   */
  class System : public GP::System
  {
  public:
    inline System(int* argc, char** argv);
  };
  
  /*!
   * \brief Wrapper class for ::gp_context extended for Qt features.
   */
  class Context : public GP::Context
  {
  public:
    inline Context();
  };
  
  /*!
   * \brief Wrapper class for ::gp_window extended for Qt features.
   */
  class Window : public GP::Window
  {
  public:
    inline Window(const GP::Context& context);
    
    /*!
     * Retrieve QWidget associated with a gp_window.
     * \return Pointer to QWidget used by the window.
     */
    inline QWidget* GetQWidget();
  };
  
  //
  // Implementation
  //
  System::System(int* argc, char** argv) : GP::System(gp_qt_system_new(argc, argv)) {}
  
  Context::Context() : GP::Context(gp_qt_context_new()) {}
  
  Window::Window(const GP::Context& context) : GP::Window(context) {}
  QWidget* Window::GetQWidget() {return gp_window_get_qwidget(GetWindow());}
}
}

#endif // __cplusplus

#endif // __GP_QT5_H__
