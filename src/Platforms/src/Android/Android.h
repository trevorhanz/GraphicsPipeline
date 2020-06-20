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

#ifndef __GP_ANDROID_H__
#define __GP_ANDROID_H__

#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES2/gl2.h>

#include <android/native_window.h>
#include <android/log.h>

#include <System.h>
#include <API/GLES/Pipeline.h>

#define ANDROID_LOG(...) __android_log_print(ANDROID_LOG_INFO, "GP", __VA_ARGS__);

namespace GP
{
  class WindowUserData : public UserData
  {
  public:
    ANativeWindow*        mWindow;
    TargetPtr             mTarget;
  };
  typedef std::shared_ptr<WindowUserData> WindowUserDataPtr;
  
  class TargetUserData : public GLES::TargetUserData
  {
  public:
    bool                  mBound;  // TODO - Replace with bitflag indicating state
    ANativeWindow*        mWindow;
    EGLDisplay            mDisplay;
    EGLSurface            mSurface;
    EGLContext            mContext;
    
    void MakeCurrent() override {eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);}
    void Present() override {eglSwapBuffers(mDisplay, mSurface);}
  };
  typedef std::shared_ptr<TargetUserData> TargetUserDataPtr;
}


#endif
