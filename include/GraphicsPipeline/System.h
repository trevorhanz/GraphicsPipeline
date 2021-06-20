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
#include "Types.h"
#include "Object.h"

#ifdef __cplusplus
#include <functional>

extern "C" {
#endif

/*!
 * \defgroup System
 * \{
 */

/*!
 * Create a new system object.
 * \return Pointer to new system object.
 */
GP_EXPORT gp_system* gp_system_new();

/*!
 * Runs the main event loop for a system obect.
 * The function will take control of the current thread.
 * \param system Pointer to system object to be run.
 */
GP_EXPORT void gp_system_run(gp_system* system);

/*!
 * Stop the main event loop for a running system object.
 * \param system Pointer to system object to be stopped.
 */
GP_EXPORT void gp_system_stop(gp_system* system);

/*!
 * Create a new gp_timer object tied to the event loop
 * of the gp_system object.
 * \param system Pointer to system object to be used.
 * \return Pointer to Newly created gp_target object.
 */
GP_EXPORT gp_timer* gp_timer_new(gp_system* system);

/*!
 * Set the callback to be used when the gp_timer timesout.
 * \param timer Pointer to timer object.
 * \param callback Callback function to be called at timeout.
 */
GP_EXPORT void gp_timer_set_callback(gp_timer* timer, gp_timer_callback callback);

/*!
 * Set user defined data to be associated with a gp_timer object.
 * \param timer Pointer to timer object.
 * \param userdata Pointer to user defined data.
 */
GP_EXPORT void gp_timer_set_userdata(gp_timer* timer, gp_pointer* userdata);

/*!
 * Get user defined data to be associated with a gp_timer object.
 * \param timer Pointer to timer object.
 * \return Pointer to user defined data.
 */
GP_EXPORT gp_pointer* gp_timer_get_userdata(gp_timer* timer);

/*!
 * Start the timer count down.
 * Note: Timer count downs only progress while the system main loop
 * is running.
 * \param timer Pointer to timer object.
 * \param timeout Time till timeout in seconds.
 */
GP_EXPORT void gp_timer_arm(gp_timer* timer, double timeout);

/*!
 * Stops the timer count down and prevents the callback
 * from being called.
 * \param timer Pointer to timer object.
 */
GP_EXPORT void gp_timer_disarm(gp_timer* timer);

/*!
 * Create a new gp_io object tied to the event loop
 * of the gp_system object for listening to read events.
 * \param system Pointer to system object to be used.
 * \param fd File descriptor to watch.
 * \return Pointer to Newly created gp_io object.
 */
GP_EXPORT gp_io* gp_io_read_new(gp_system* system, int fd);

/*!
 * Create a new gp_io object tied to the event loop
 * of the gp_system object for listening to write events.
 * \param system Pointer to system object to be used.
 * \param fd File descriptor to watch.
 * \return Pointer to Newly created gp_io object.
 */
GP_EXPORT gp_io* gp_io_write_new(gp_system* system, int fd);

/*!
 * Set the callback to be used for gp_io events.
 * \param io Pointer to io object.
 * \param callback Callback function to be called for events.
 * \param userdata Pointer to be passed to callback function.
 */
GP_EXPORT void gp_io_set_callback(gp_io* io, gp_io_callback callback);

/*!
 * Set user defined data to be associated with a gp_timer object.
 * \param io Pointer to io object.
 * \param userdata Pointer to user defined data.
 */
GP_EXPORT void gp_io_set_userdata(gp_io* io, gp_pointer* userdata);

/*!
 * Get user defined data to be associated with a gp_timer object.
 * \param io Pointer to io object.
 * \return Pointer to user defined data.
 */
GP_EXPORT gp_pointer* gp_io_get_userdata(gp_io* io);

//! \} // System

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  class Timer;
  class IO;
  class ReadIO;
  class WriteIO;
  
  /*!
   * \brief Wrapper class for ::gp_system.
   */
  class System : public Object
  {
  public:
    //! Constructor
    inline System();
    
    //! Constructor
    inline System(gp_system* system);
    
    //! Destructor
    inline ~System();
    
    /*!
     * Start the main loop.
     */
    inline void Run();
    
    /*!
     * Stop the main loop.
     */
    inline void Stop();
    
  private:
    
    friend class Context;
    friend class Timer;
    friend class IO;
    friend class ReadIO;
    friend class WriteIO;
  };
  
  /*!
   * \brief Wrapper class for ::gp_timer.
   */
  class Timer : public Object
  {
  public:
    //! Constructor
    inline Timer(gp_timer* timer);
    
    //! Constructor
    inline Timer(const System& system);
    
     //! Copy Constructor
    inline Timer(const Object& other);
    
    //! Destructor
    inline ~Timer();
    
    /*!
     * Sets callback function to be called at timeout.
     * \param callback Callback function to be called.
     */
    inline void SetCallback(std::function<void(Timer&)> callback);
    
    /*!
     * Start timer count down.
     * \param seconds Time till timeout in seconds.
     */
    inline void Arm(double seconds);
    
    /*!
     * Cancel the current count down.
     */
    inline void Disarm();
    
  private:
    inline static void HandleTimeout(gp_timer* timer);
    
    struct CallbackData
    {
      std::function<void(Timer&)>           mCallback;
    };
  };
  
  /*!
   * \brief Wapper class for ::gp_io.
   */
  class IO : public Object
  {
  protected:
    //! Constructor. Only for internal use.
    inline IO(void* io);
    
  public:
    //! Constructor
    inline IO(gp_io* io);
    
    //! Destructor
    inline ~IO();
    
    /*!
     * Sets callback function to be called at timeout.
     * \param callback Callback function to be called.
     */
    inline void SetCallback(std::function<void(IO&)> callback);
    
  private:
    inline static void HandleUpdate(gp_io* io);
    
    struct CallbackData
    {
      std::function<void(IO&)>              mCallback;
    };
    
    friend class System;
  };
  
  /*!
   * \brief IO class extended for read operations
   */
  class ReadIO : public IO
  {
  public:
    inline ReadIO(const System& system, int fd);
  };
  
  /*!
   * \brief IO class extended for write operations
   */
  class WriteIO : public IO
  {
  public:
    inline WriteIO(const System& system, int fd);
  };
  
  //
  // Implementation
  //
  System::System() : Object((void*)gp_system_new()) {}
  System::System(gp_system* system) : Object((gp_object*)system) {}
  System::~System() {}
  void System::Run() {gp_system_run((gp_system*)mObject);}
  void System::Stop() {gp_system_stop((gp_system*)mObject);}
  
  Timer::Timer(gp_timer* timer) : Object((gp_object*)timer) {}
  Timer::Timer(const System& system) : Object((void*)gp_timer_new((gp_system*)system.mObject)) {}
  Timer::Timer(const Object& other) : Object(other) {}
  Timer::~Timer() {}
  void Timer::SetCallback(std::function<void(Timer&)> callback)
  {
    CallbackData* data = new CallbackData();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_timer_set_callback((gp_timer*)mObject, HandleTimeout);
    gp_timer_set_userdata((gp_timer*)mObject, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Timer::HandleTimeout(gp_timer* timer)
  {
    CallbackData* data = (CallbackData*)gp_pointer_get_pointer(gp_timer_get_userdata(timer));
    Timer t = timer;
    data->mCallback(t);
  }
  void Timer::Arm(double seconds) {gp_timer_arm((gp_timer*)mObject, seconds);}
  void Timer::Disarm() {gp_timer_disarm((gp_timer*)mObject);}
  
  IO::IO(void* io) : Object((void*)io) {}
  IO::IO(gp_io* io) : Object((gp_object*)io) {}
  IO::~IO() {}
  void IO::SetCallback(std::function<void(IO&)> callback)
  {
    CallbackData* data = new CallbackData();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_io_set_callback((gp_io*)mObject, HandleUpdate);
    gp_io_set_userdata((gp_io*)mObject, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void IO::HandleUpdate(gp_io* io)
  {
    CallbackData* data = (CallbackData*)gp_pointer_get_pointer(gp_io_get_userdata(io));
    IO i = io;
    data->mCallback(i);
  }
  
  ReadIO::ReadIO(const System& system, int fd) : IO((void*)gp_io_read_new((gp_system*)system.mObject, fd)) {}
  
  WriteIO::WriteIO(const System& system, int fd) : IO(gp_io_write_new((gp_system*)system.mObject, fd)) {}
}
#endif // __cplusplus

#endif // __GP_SYSTEM_H__
