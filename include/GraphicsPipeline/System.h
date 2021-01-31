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
 * Free system object.
 * \param system Pointer to system object to be freed.
 */
GP_EXPORT void gp_system_free(gp_system* system);

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
GP_EXPORT gp_timer* gp_system_timer_new(gp_system* system);

/*!
 * Free a gp_timer object.
 * \param timer Pointer to timer object to be freed.
 */
GP_EXPORT void gp_timer_free(gp_timer* timer);

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
GP_EXPORT void gp_timer_set_userdata(gp_timer* timer, void* userdata);

/*!
 * Get user defined data to be associated with a gp_timer object.
 * \param timer Pointer to timer object.
 * \return Pointer to user defined data.
 */
GP_EXPORT void* gp_timer_get_userdata(gp_timer* timer);

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
GP_EXPORT gp_io* gp_system_io_read_new(gp_system* system, int fd);

/*!
 * Create a new gp_io object tied to the event loop
 * of the gp_system object for listening to write events.
 * \param system Pointer to system object to be used.
 * \param fd File descriptor to watch.
 * \return Pointer to Newly created gp_io object.
 */
GP_EXPORT gp_io* gp_system_io_write_new(gp_system* system, int fd);

/*!
 * Free a gp_io object.
 * \param io Pointer to io object to be freed.
 */
GP_EXPORT void gp_io_free(gp_io* io);

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
GP_EXPORT void gp_io_set_userdata(gp_io* io, void* userdata);

/*!
 * Get user defined data to be associated with a gp_timer object.
 * \param io Pointer to io object.
 * \return Pointer to user defined data.
 */
GP_EXPORT void* gp_io_get_userdata(gp_io* io);

//! \} // System

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  class Timer;
  class IO;
  
  /*!
   * \brief Wrapper class for ::gp_system.
   */
  class System
  {
  public:
    //! Constructor
    inline System();
    
    //! Destructor
    inline ~System();
    
    /*!
     * Creates a new GP::Timer for this system.
     * \return Pointer to newly created GP::Timer.
     */
    inline Timer* CreateTimer();
    
    /*!
     * Creates a new GP::IO for this system that listens
     * for a file descriptor to become readable.
     * \param fd File descriptor for which to listen.
     * \return Pointer to newly created GP::IO.
     */
    inline IO* CreateReadIO(int fd);
    
    /*!
     * Creates a new GP::IO for this system that listens
     * for a file descriptor to become writable.
     * \param fd File descriptor for which to listen.
     * \return Pointer to newly created GP::IO.
     */
    inline IO* CreateWriteIO(int fd);
    
    /*!
     * Start the main loop.
     */
    inline void Run();
    
  private:
    gp_system*            mSystem;
    
    friend class Context;
  };
  
  /*!
   * \brief Wrapper class for ::gp_timer.
   */
  class Timer
  {
  private:
    //! Constructor
    inline Timer(gp_timer* timer);
    
  public:
    //! Destructor
    inline ~Timer();
    
    /*!
     * Sets callback function to be called at timeout.
     * \param callback Callback function to be called.
     */
    inline void SetCallback(std::function<void(Timer*)> callback);
    
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
    
    gp_timer*                           mTimer;
    std::function<void(Timer*)>         mCallback;
    
    friend class System;
  };
  
  /*!
   * \brief Wapper class for ::gp_io.
   */
  class IO
  {
  private:
    //! Constructor
    inline IO(gp_io* io);
    
  public:
    //! Destructor
    inline ~IO();
    
    /*!
     * Sets callback function to be called at timeout.
     * \param callback Callback function to be called.
     */
    inline void SetCallback(std::function<void(IO*)> callback);
    
  private:
    inline static void HandleUpdate(gp_io* io);
    
    gp_io*                              mIO;
    std::function<void(IO*)>            mCallback;
    
    friend class System;
  };
  
  //
  // Implementation
  //
  System::System() {mSystem = gp_system_new();}
  System::~System() {gp_system_free(mSystem);}
  Timer* System::CreateTimer() {return new Timer(gp_system_timer_new(mSystem));}
  IO* System::CreateReadIO(int fd) {return new IO(gp_system_io_read_new(mSystem, fd));}
  IO* System::CreateWriteIO(int fd) {return new IO(gp_system_io_write_new(mSystem, fd));}
  void System::Run() {gp_system_run(mSystem);}
  
  Timer::Timer(gp_timer* timer) : mTimer(timer) {}
  Timer::~Timer() {gp_timer_free(mTimer);}
  void Timer::SetCallback(std::function<void(Timer*)> callback)
  {
    gp_timer_set_callback(mTimer, HandleTimeout);
    gp_timer_set_userdata(mTimer, (void*)this);
    mCallback = callback;
  }
  void Timer::HandleTimeout(gp_timer* timer)
  {
    Timer* THIS = (Timer*)gp_timer_get_userdata(timer);
    THIS->mCallback(THIS);
  }
  void Timer::Arm(double seconds) {gp_timer_arm(mTimer, seconds);}
  void Timer::Disarm() {gp_timer_disarm(mTimer);}
  
  IO::IO(gp_io* io) : mIO(io) {}
  IO::~IO() {gp_io_free(mIO);}
  void IO::SetCallback(std::function<void(IO*)> callback)
  {
    gp_io_set_callback(mIO, HandleUpdate);
    gp_io_set_userdata(mIO, (void*)this);
    mCallback = callback;
  }
  void IO::HandleUpdate(gp_io* io)
  {
    IO* THIS = (IO*)gp_io_get_userdata(io);
    THIS->mCallback(THIS);
  }
}
#endif // __cplusplus

#endif // __GP_SYSTEM_H__
