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
    GP::LogE("eglGetDisplay() returned error %d", eglGetError());
    return;
  }
  if (!eglInitialize(mDisplay, 0, 0)) {
    GP::LogE("eglInitialize() returned error %d", eglGetError());
    return;
  }

  if (!eglChooseConfig(mDisplay, attribs, &mConfig, 1, &numConfigs)) {
    GP::LogE("eglChooseConfig() returned error %d", eglGetError());

    return;
  }

  if (!eglGetConfigAttrib(mDisplay, mConfig, EGL_NATIVE_VISUAL_ID, &mFormat)) {
    GP::LogE("eglGetConfigAttrib() returned error %d", eglGetError());
    return;
  }
  
  const EGLint contextAttribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 2,
    EGL_NONE
  };
  
  if (!(mShare = eglCreateContext(mDisplay, mConfig, 0, contextAttribs))) {
    GP::LogE("eglCreateContext() returned error %d", eglGetError());
    return;
  }
  
  const EGLint surfaceAttribs[] {
    EGL_NONE
  };
  
  if (!(mShareSurface = eglCreatePbufferSurface(mDisplay, mConfig, surfaceAttribs))) {
    GP::LogE("eglCreatePbufferSurface() returned error %d", eglGetError());
    return;
  }
  
  if (!eglMakeCurrent(mDisplay, mShareSurface, mShareSurface, mShare)) {
      GP::LogE("eglMakeCurrent() returned error %d", eglGetError());
      return;
    }
  
  GP::LogE("GL Version: %s", glGetString(GL_VERSION));
}

GP::PipelinePtr Context::CreatePipeline()
{
  return std::make_shared<GLES::Pipeline>(this);
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
      GP::LogE("eglCreateWindowSurface() returned error %d", eglGetError());
      return;
    }
    
    const EGLint contextAttribs[] = {
      EGL_CONTEXT_MAJOR_VERSION, 2,
      EGL_NONE
    };
    
    if (!(ptr->mContext = eglCreateContext(mDisplay, mConfig, mShare, contextAttribs))) {
      GP::LogE("eglCreateContext() returned error %d", eglGetError());
      return;
    }
    
    if (!eglMakeCurrent(mDisplay, ptr->mSurface, ptr->mSurface, ptr->mContext)) {
      GP::LogE("eglMakeCurrent() returned error %d", eglGetError());
      return;
    }
    
    ptr->mDisplay = mDisplay;
    ptr->mBound = true;
  }
}
