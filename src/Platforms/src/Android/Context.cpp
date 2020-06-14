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

#include "Context.h"
#include "Android.h"

#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES2/gl2.h>

using namespace GP::GLES;

Context::Context()
{
  const EGLint attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE
  };
  EGLint numConfigs;
  
  if ((mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
    ANDROID_LOG("eglGetDisplay() returned error %d", eglGetError());
    return;
  }
  if (!eglInitialize(mDisplay, 0, 0)) {
    ANDROID_LOG("eglInitialize() returned error %d", eglGetError());
    return;
  }

  if (!eglChooseConfig(mDisplay, attribs, &mConfig, 1, &numConfigs)) {
    ANDROID_LOG("eglChooseConfig() returned error %d", eglGetError());

    return;
  }

  if (!eglGetConfigAttrib(mDisplay, mConfig, EGL_NATIVE_VISUAL_ID, &mFormat)) {
    ANDROID_LOG("eglGetConfigAttrib() returned error %d", eglGetError());
    return;
  }
  
  const EGLint contextAttribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 2,
    EGL_NONE
  };
  
  if (!(mShare = eglCreateContext(mDisplay, mConfig, 0, contextAttribs))) {
    ANDROID_LOG("eglCreateContext() returned error %d", eglGetError());
    return;
  }
  
  const EGLint surfaceAttribs[] {
    EGL_NONE
  };
  
  if (!(mShareSurface = eglCreatePbufferSurface(mDisplay, mConfig, surfaceAttribs))) {
    ANDROID_LOG("eglCreatePbufferSurface() returned error %d", eglGetError());
    return;
  }
  
  if (!eglMakeCurrent(mDisplay, mShareSurface, mShareSurface, mShare)) {
      ANDROID_LOG("eglMakeCurrent() returned error %d", eglGetError());
      return;
    }
  
  ANDROID_LOG("GL Version: %s", glGetString(GL_VERSION));
}

GP::PipelinePtr Context::CreatePipeline()
{
  return std::make_shared<GLES::PipelineBase>(this);
}

void Context::Bind(GP::TargetPtr target)
{
  GP::TargetUserDataPtr ptr;
  if(!target->mUserData)
  {
    ptr = std::make_shared<GP::TargetUserData>();
    target->mUserData = ptr;
    ptr->mBound = false;
  }
  else
    ptr = std::dynamic_pointer_cast<GP::TargetUserData>(target->mUserData);
  
  if(!ptr->mBound)
  {
    ANativeWindow_setBuffersGeometry(ptr->mWindow, 0, 0, mFormat);

    if (!(ptr->mSurface = eglCreateWindowSurface(mDisplay, mConfig, ptr->mWindow, 0))) {
      ANDROID_LOG("eglCreateWindowSurface() returned error %d", eglGetError());
      return;
    }
    
    const EGLint contextAttribs[] = {
      EGL_CONTEXT_MAJOR_VERSION, 2,
      EGL_NONE
    };
    
    if (!(ptr->mContext = eglCreateContext(mDisplay, mConfig, mShare, contextAttribs))) {
      ANDROID_LOG("eglCreateContext() returned error %d", eglGetError());
      return;
    }
    
    if (!eglMakeCurrent(mDisplay, ptr->mSurface, ptr->mSurface, ptr->mContext)) {
      ANDROID_LOG("eglMakeCurrent() returned error %d", eglGetError());
      return;
    }
    
    ptr->mDisplay = mDisplay;
    ptr->mBound = true;
  }
}
