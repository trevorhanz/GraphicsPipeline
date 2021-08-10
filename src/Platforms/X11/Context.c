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

#include "X11.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "../../API/GL/GL.h"
#include "Platforms/Defaults.h"

gp_context* sContext = NULL;

#define GP_STATE_RUNNING 0x01

typedef struct
{
  gp_list_node      mNode;
  void(*mWork)(void*);
  void(*mJoin)(void*);
  void*             mData;
} _gp_work_node;

void* _gp_work_thread(void* data)
{
  gp_context* context = (gp_context*)data;
  
  glXMakeCurrent(context->mDisplay, context->mWindow, context->mWorkCtx);
  
  _gp_api_init_context();
  
  pthread_mutex_lock(&context->mWorkMutex);
  
  // Signal main thread that initialization is complete
  pthread_cond_signal(&context->mWorkCV);
  
  while(context->mState&GP_STATE_RUNNING)
  {
    while(gp_list_front(&context->mWork) != NULL)
    {
      _gp_work_node* node = (_gp_work_node*)gp_list_front(&context->mWork);
      gp_list_remove(&context->mWork, (gp_list_node*)node);
      pthread_mutex_unlock(&context->mWorkMutex);
      
      node->mWork(node->mData);
      
      pthread_mutex_lock(&context->mWorkMutex);
      gp_list_push_back(&context->mFinished, (gp_list_node*)node);
      size_t r = write(context->mWorkPipe[1], "x", 1);
    }
    
    pthread_cond_wait(&context->mWorkCV, &context->mWorkMutex);
  }
  
  return 0;
}

void _gp_work_done(gp_io* io)
{
  gp_context* context = (gp_context*)gp_io_get_userdata(io);
  
  for(;;)
  {
    char ch;
    if(read(context->mWorkPipe[0], &ch, 1) == -1)
      break;
  }
  
  pthread_mutex_lock(&context->mWorkMutex);
  
  while(gp_list_front(&context->mFinished) != NULL)
  {
    _gp_work_node* node = (_gp_work_node*)gp_list_front(&context->mFinished);
    gp_list_remove(&context->mFinished, (gp_list_node*)node);
    
    pthread_mutex_unlock(&context->mWorkMutex);
    node->mJoin(node->mData);
    pthread_mutex_lock(&context->mWorkMutex);
  }
  
  pthread_mutex_unlock(&context->mWorkMutex);
}

gp_context* gp_context_new(gp_system* system)
{
  assert(system != NULL);
  
  gp_context* context = (gp_context*)malloc(sizeof(struct _gp_context));
  context->mParent = system;
  context->mDisplay = system->mDisplay;
  context->mWindow = 0;
  context->mState = GP_STATE_RUNNING;
  gp_ref_init(&context->mRef);
  
  gp_list_init(&context->mWork);
  gp_list_init(&context->mFinished);
  
  // Get a matching FB config
  static int attrList[] =
  {
    GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
    GLX_RENDER_TYPE     , GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
    GLX_RED_SIZE        , 8,
    GLX_GREEN_SIZE      , 8,
    GLX_BLUE_SIZE       , 8,
    GLX_ALPHA_SIZE      , 8,
    GLX_DEPTH_SIZE      , 24,
    GLX_STENCIL_SIZE    , 8,
    GLX_DOUBLEBUFFER    , True,
    None
  };
  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig(context->mDisplay, DefaultScreen(context->mDisplay), attrList, &fbcount );
  if ( !fbc )
  {
//     cerr << "Failed to retrieve a framebuffer config" << endl;
//     return;
  }
  int samp_buf, samples, best_samples, best_fbc;
  best_samples = -1;
  best_fbc = 0;
  for(int i=0; i<fbcount; i++)
  {
    XVisualInfo *vi;
    vi = glXGetVisualFromFBConfig(context->mDisplay, fbc[i] );
    if (vi)
    {
      glXGetFBConfigAttrib(context->mDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
      glXGetFBConfigAttrib(context->mDisplay, fbc[i], GLX_SAMPLES       , &samples  );
      if(samples > best_samples)
      {
        best_fbc = i;
        best_samples = samples;
        context->mVisualInfo = vi;
      }
    }
  }
  context->mConfig = fbc[best_fbc];
  XFree(fbc);
  
  context->mColorMap = XCreateColormap(context->mDisplay, RootWindow(context->mDisplay, context->mVisualInfo->screen), context->mVisualInfo->visual, AllocNone);
  
  //
  // Create dummy context to read OpenGL version.
  //
  GLXContext dummy = glXCreateNewContext(context->mDisplay, context->mConfig, GLX_RGBA_TYPE, NULL, True);
  glXMakeCurrent(context->mDisplay, context->mWindow, dummy);
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  
  _gp_api_init();
  
  glXDestroyContext(context->mDisplay, dummy);
  
  //
  // Create the real OpenGL context.
  //
  int context_attribs[] =
  {
    GLX_CONTEXT_MAJOR_VERSION_ARB, major,
    GLX_CONTEXT_MINOR_VERSION_ARB, minor,
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
    None
  };
  
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");
  GLXContext ctx = glXCreateContextAttribsARB(context->mDisplay, context->mConfig, 0, True, context_attribs);
  context->mShare = ctx;
  
  //
  // Create window
  //
  XSetWindowAttributes attr;
  attr.colormap = context->mColorMap;
  attr.border_pixel = 0;
  attr.event_mask = 0;
  
  context->mWindow = XCreateWindow(context->mDisplay, RootWindow(context->mDisplay, context->mVisualInfo->screen),
                                   0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT,
                                   0, context->mVisualInfo->depth, InputOutput, context->mVisualInfo->visual,
                                   CWBorderPixel | CWColormap | CWEventMask, &attr);
  
  // NOTE: Context needs window first time it is made current.
  glXMakeCurrent(context->mDisplay, context->mWindow, context->mShare);
  
  // NOTE: GL functions need a context to be bound to get information from
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  gp_log_info("OpenGL Version: %d.%d", major, minor);
  
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  gp_log_info("GLSL Version: %s", glslVersion);
  
  gp_log_info("Direct Rendering: %s", ((glXIsDirect(context->mDisplay, context->mShare)) ? "YES" : "NO"));
  
  _gp_api_init_context();
  
  _gp_event_pipe_new(system->mEvent, context->mWorkPipe);
  
  context->mWorkCtx = glXCreateContextAttribsARB(context->mDisplay, context->mConfig, context->mShare, True, context_attribs);
  glXMakeCurrent(context->mDisplay, context->mWindow, context->mShare);
  
  gp_pointer* pointer = gp_pointer_new(context, 0);
  context->mWorkIO = gp_io_read_new(system, context->mWorkPipe[0]);
  gp_io_set_callback(context->mWorkIO, _gp_work_done);
  gp_io_set_userdata(context->mWorkIO, pointer);
  gp_object_unref((gp_object*)pointer);
  
  pthread_mutex_init(&context->mWorkMutex, NULL);
  pthread_cond_init(&context->mWorkCV, NULL);
  
  pthread_mutex_lock(&context->mWorkMutex);
  pthread_create(&context->mWorkThread, NULL, _gp_work_thread, context);
  
  // Wait for work thread to finish initializing before continuing.
  // X11 has trouble using the display connection in multiple threads.
  pthread_cond_wait(&context->mWorkCV, &context->mWorkMutex);
  pthread_mutex_unlock(&context->mWorkMutex);
  
  sContext = context;
  
  return context;
}

void gp_context_ref(gp_context* context)
{
  gp_ref_inc(&context->mRef);
}

void gp_context_unref(gp_context* context)
{
  if(gp_ref_dec(&context->mRef))
  {
    glXDestroyContext(context->mDisplay, context->mShare);
    XFreeColormap(context->mDisplay, context->mColorMap);
    XFree(context->mVisualInfo);
    gp_object_unref((gp_object*)context->mWorkIO);
    
    close(context->mWorkPipe[0]);
    close(context->mWorkPipe[1]);
    
    pthread_mutex_lock(&context->mWorkMutex);
    context->mState &= ~GP_STATE_RUNNING;
    pthread_cond_signal(&sContext->mWorkCV);
    pthread_mutex_unlock(&context->mWorkMutex);
    pthread_join(context->mWorkThread, NULL);
    free(context);
  }
}

void _gp_api_work(void(*work)(void*), void(*join)(void*), void* data)
{
  _gp_work_node* node = malloc(sizeof(_gp_work_node));
  node->mWork = work;
  node->mJoin = join;
  node->mData = data;
  
  pthread_mutex_lock(&sContext->mWorkMutex);
  gp_list_push_back(&sContext->mWork, (gp_list_node*)node);
  pthread_cond_signal(&sContext->mWorkCV);
  pthread_mutex_unlock(&sContext->mWorkMutex);
}

void _gp_api_context_make_current(gp_context* context)
{
  glXMakeCurrent(context->mDisplay, 0, context->mShare);
}
