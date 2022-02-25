/************************************************************************
* Copyright (C) 2021 Trevor Hanz
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

#ifndef __GP_MONITORS_H__
#define __GP_MONITORS_H__

#include "Common.h"
#include "Types.h"
#include "System.h"
#include "Object.h"

#ifdef __cplusplus
#include <functional>

extern "C" {
#endif

/*!
 * \defgroup Monitor
 * \{
 */

/*!
 * Create a new monitor list object tied to a system object.
 * \param system Pointer to system object to be used.
 * \return Pointer to new monitor list object.
 */
GP_EXPORT gp_monitor_list* gp_monitor_list_new(gp_system* system);

/*!
 * Retrieve the number of monitors known to a monitor list object.
 * \param monitors Pointer to a monitor list object to be used.
 * \return Number of monitors known to the monitor list object.
 */
GP_EXPORT int gp_monitor_list_get_count(gp_monitor_list* monitors);

/*!
 * Retrieve the monitor object at the specified index inside a monitor list object.
 * \param monitors Pointer to a monitor list object to be used.
 * \param index The index of the monitor to be retrieved.
 * \return The monitor object at the specified index or NULL if there are no monitors
 * at the provided index.
 */
GP_EXPORT gp_monitor* gp_monitor_list_get_by_index(gp_monitor_list* monitors, int index);

/*!
 * Retrieve the monitor object at the specified position on the screen.
 * \param monitors Pointer to a monitor list object to be used.
 * \param x The x screen coordinate.
 * \param y The y screen coordinate.
 * \return The monitor object at the specified location or NULL if there are no monitors
 * at the provided location.
 */
GP_EXPORT gp_monitor* gp_monitor_list_get_at_position(gp_monitor_list* monitors, int x, int y);

/*!
 * Retrieve the primary monitor.  All lists that contain 1 or more monitors will
 * have a primary monitor.  On desktop systems, the primary monitor is usually
 * the largest and central most monitor and contains a menu bar.  This is not
 * always the case as users can often manually select the monitor to be used as
 * the primary.
 * \param monitors Pointer to a monitor list object to be used.
 * \return The monitor object referring to the primary monitor of NULL if there
 * are no monitors in the monitor list object to be primary.
 */
GP_EXPORT gp_monitor* gp_monitor_list_get_primary(gp_monitor_list* monitors);

/*!
 * Retrieve the position of the top-left corner of the monitor.  The position is measured in
 * pixels after screen orientation has been applied.
 * \param monitor Pointer to a monitor to be used.
 * \return The position of the monitor in pixels.
 */
GP_EXPORT gp_point gp_monitor_get_position(gp_monitor* monitor);

/*!
 * Retrieve the size of the monitor.  Size is measured in pixels after screen orientation
 * has been applied.
 * \param monitor Pointer to a monitor to be used.
 * \return The size of the monitor in pixels.
 */
GP_EXPORT gp_size gp_monitor_get_size(gp_monitor* monitor);

/*!
 * Convenience function for retreiving both the size and position of a monitor at the same time.
 * \param monitor Pointer to a monitor to be used.
 * \return The rectangular region of the monitor in pixels.
 * \see gp_monitor_get_size()
 * \see gp_monitor_get_position()
 */
GP_EXPORT gp_rect gp_monitor_get_rect(gp_monitor* monitor);

//! \} // Monitor

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_monitor
   */
  class MonitorList : public Object
  {
  public:
    //! Constructor
    inline MonitorList(const System& system);
    
    /*!
     * Retrieve the number of monitors known to a monitor list object.
     * \return Number of monitors known to the monitor list object.
     */
    inline int GetCount();
    
    /*!
     * Retrieve the monitor object at the spacified index inside a monitor list object.
     * \param index The index of the monitor to be retrieved.
     * \return The monitor object at the specified index or NULL if there are no monitors
     * at the provided index.
     */
    inline Monitor GetByIndex(int index);
    
    /*!
     * Retrieve the monitor object at the specified position on the screen.
     * \param x The x screen coordinate.
     * \param y The y screen coordinate.
     * \return The monitor object at the specified location or NULL if there are no monitors
     * at the provided location.
     */
    inline Monitor GetAtPosition(int x, int y);
    
    /*!
     * Retrieve the primary monitor.  All lists that contain 1 or more monitors will
     * have a primary monitor.  On desktop systems, the primary monitor is usually
     * the largest and central most monitor and contains a menu bar.  This is not
     * always the case as users can often manually select the monitor to be used as
     * the primary.
     * \return The monitor object referring to the primary monitor of NULL if there
     * are no monitors in the monitor list object to be primary.
     */
    inline Monitor GetPrimary();
  };
  
  /*!
   * \brief Wrapper class for ::gp_monitor
   */
  class Monitor : public Object
  {
  public:
    inline Monitor();
    
    //! Constructor
    inline Monitor(gp_monitor* monitor);
    
    /*!
     * Retrieve the position of the top-left corner of the monitor.  The position is measured in
     * pixels after screen orientation has been applied.
     * \return The position of the monitor in pixels.
     */
    inline Point GetPosition();
    
    /*!
     * Retrieve the size of the monitor.  Size is measured in pixels after screen orientation
     * has been applied.
     * \return Size of the monitor in pixels.
     */
    inline Size GetSize();
    
    /*!
     * Convenience function for retreiving both the size and position of a monitor at the same time.
     * \return The rectangular region of the monitor in pixels.
     * \see Monitor::GetSize()
     * \see Monitor::GetPosition()
     */
    inline Rect GetRect();
  };
  
  //
  // Implementation
  //
  MonitorList::MonitorList(const System& system) : Object((void*)gp_monitor_list_new((gp_system*)system.GetObject())) {}
  int MonitorList::GetCount() {return gp_monitor_list_get_count((gp_monitor_list*)GetObject());}
  Monitor MonitorList::GetByIndex(int index) {return Monitor(gp_monitor_list_get_by_index((gp_monitor_list*)GetObject(), index));}
  Monitor MonitorList::GetAtPosition(int x, int y) {return Monitor(gp_monitor_list_get_at_position((gp_monitor_list*)GetObject(), x, y));}
  Monitor MonitorList::GetPrimary() {return Monitor(gp_monitor_list_get_primary((gp_monitor_list*)GetObject()));}
  
  Monitor::Monitor() : Object((void*)0) {}
  Monitor::Monitor(gp_monitor* monitor) : Object((gp_object*)monitor) {}
  Point Monitor::GetPosition() {return gp_monitor_get_position((gp_monitor*)GetObject());}
  Size Monitor::GetSize() {return gp_monitor_get_size((gp_monitor*)GetObject());}
  Rect Monitor::GetRect() {return gp_monitor_get_rect((gp_monitor*)GetObject());}
}
#endif // __cplusplus

#endif // __GP_MONITORS_H__
