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

#include "MacOS.h"

#include <unistd.h>

#include <OpenGL/gl3.h>

#include "View.h"
#include "Platforms/Defaults.h"

gp_context* sContext = 0;

typedef struct
{
  gp_list_node          mNode;
  void(*mWork)(void*);
  void(*mJoin)(void*);
  void*                 mData;
} _gp_work_node;

void* _gp_work_thread(void* data)
{
  gp_context* context = (gp_context*)data;
  
  [context->mWorkContext makeCurrentContext];
  
  _gp_api_init_context();
  
  pthread_mutex_lock(&context->mWorkMutex);
  
  while(1)
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
  
  return NULL;
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
  gp_context* context = malloc(sizeof(gp_context));
  gp_ref_init(&context->mRef);
  
  NSOpenGLPixelFormatAttribute pixelAttribs[ 16 ];
  int pixNum = 0;

  pixelAttribs[ pixNum++ ] = NSOpenGLPFAOpenGLProfile;
  pixelAttribs[ pixNum++ ] = NSOpenGLProfileVersion3_2Core;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFADoubleBuffer;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFAAccelerated;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFAColorSize;
  pixelAttribs[ pixNum++ ] = 32;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFADepthSize;
  pixelAttribs[ pixNum++ ] = 16;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFAMultisample,
  pixelAttribs[ pixNum++ ] = NSOpenGLPFASampleBuffers;
  pixelAttribs[ pixNum++ ] = 1;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFASamples;
  pixelAttribs[ pixNum++ ] = 4;
  pixelAttribs[ pixNum ] = 0;
  context->mPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelAttribs];

  context->mShare = [[NSOpenGLContext alloc] initWithFormat:context->mPixelFormat shareContext:NULL];
  [context->mShare makeCurrentContext];
  
  _gp_api_init_context();
  
  // NOTE: GL functions need a context to be bound to get information from
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  gp_log_info("OpenGL Version: %d.%d", major, minor);

  GLboolean shaderSupport;
  glGetBooleanv(GL_SHADER_COMPILER, &shaderSupport);
  gp_log_info("ShaderSupport: %s", (shaderSupport)?"TRUE":"FALSE");
  
  sContext = context;
  
  pipe(context->mWorkPipe);
  int flags = fcntl(context->mWorkPipe[0], F_GETFL);
  flags += O_NONBLOCK;
  fcntl(context->mWorkPipe[0], F_SETFL, flags);
  
  flags = fcntl(context->mWorkPipe[1], F_GETFL);
  flags += O_NONBLOCK;
  fcntl(context->mWorkPipe[1], F_SETFL, flags);
  
  context->mWorkIO = gp_io_read_new(system, context->mWorkPipe[0]);
  gp_io_set_callback(context->mWorkIO, _gp_work_done);
  gp_io_set_userdata(context->mWorkIO, context);
  gp_list_init(&context->mWork);
  gp_list_init(&context->mFinished);
  context->mWorkContext = [[NSOpenGLContext alloc] initWithFormat:context->mPixelFormat shareContext:context->mShare];
  pthread_mutex_init(&context->mWorkMutex, NULL);
  pthread_cond_init(&context->mWorkCV, NULL);
  pthread_create(&context->mWorkThread, NULL, _gp_work_thread, context);
  
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
    free(context);
  }
}

gp_target* gp_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(gp_target));
  target->mParent = context;
  target->mPipeline = _gp_pipeline_new();
  gp_ref_init(&target->mRef);
  
  NSUInteger windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable |
                           NSWindowStyleMaskMiniaturizable;
  target->mWindow = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT)
  styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO]
  autorelease];
  [target->mWindow cascadeTopLeftFromPoint:NSMakePoint(20,20)];
  [target->mWindow setTitle:@"GP Window"];
  [target->mWindow makeKeyAndOrderFront:nil];

  // Create OpenGL view
  target->mView = [[View alloc] initWithFrame:NSMakeRect( 0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT ) pixelFormat:context->mPixelFormat];
  NSOpenGLContext* newContext = [ [ NSOpenGLContext alloc ] initWithFormat:context->mPixelFormat shareContext:context->mShare ];
  [target->mView setOpenGLContext: newContext];
  [target->mWindow setContentView:target->mView];
  [target->mView setTarget:target];
  [target->mView display];

  [[target->mView openGLContext] makeCurrentContext];
  
  _gp_api_init_context();
  
  return target;
}

void gp_target_ref(gp_target* target)
{
  gp_ref_inc(&target->mRef);
}

void gp_target_unref(gp_target* target)
{
  if(gp_ref_dec(&target->mRef))
  {
    _gp_pipeline_free(target->mPipeline);
    free(target);
  }
}

gp_pipeline* gp_target_get_pipeline(gp_target* target)
{
  return target->mPipeline;
}

void gp_target_redraw(gp_target* target)
{
  [target->mView setNeedsDisplay:YES];
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

