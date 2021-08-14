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

#include <windows.h>
#include <winuser.h>

#include <GL/glew.h>
#include <GL/wglew.h>

#include <GraphicsPipeline/Window.h>
#include "API/GL/GL.h"
#include "Windows.h"
#include "Platforms/Defaults.h"

void _gp_window_free(gp_object* object)
{
  gp_window* window = (gp_window*)object;
  
  _gp_pipeline_free(window->mPipeline);
  
  if(window->mClickData) gp_object_unref((gp_object*)window->mClickData);
  if(window->mTrackData) gp_object_unref((gp_object*)window->mTrackData);
  if(window->mEnterData) gp_object_unref((gp_object*)window->mEnterData);
  if(window->mKeyData) gp_object_unref((gp_object*)window->mKeyData);
  if(window->mResizeData) gp_object_unref((gp_object*)window->mResizeData);
  if(window->mMoveData) gp_object_unref((gp_object*)window->mMoveData);
  
  free(window);
}

gp_window* gp_window_new(gp_context* context)
{
  gp_window* window = malloc(sizeof(struct _gp_window));
  _gp_object_init(&window->mObject, _gp_window_free);
  window->mParent = context;
  window->mPipeline = _gp_pipeline_new();
  window->mMinWidth = 0;
  window->mMinHeight = 0;
  window->mMaxWidth = 1000000;
  window->mMaxHeight = 1000000;
  window->mClickCB = NULL;
  window->mClickData = NULL;
  window->mTrackCB = NULL;
  window->mTrackData = NULL;
  window->mEnterCB = NULL;
  window->mEnterData = NULL;
  window->mKeyCB = NULL;
  window->mKeyData = NULL;
  window->mResizeCB = NULL;
  window->mResizeData = NULL;
  window->mMoveCB = NULL;
  window->mMoveData = NULL;
  window->mMouseEntered = 0;

  DWORD                 dwExStyle;                                  // Window Extended Style
  DWORD                 dwStyle;                                    // Window Style
  RECT                  WindowRect;                                 // Grabs Rectangle Upper Left / Lower Right Values
  WindowRect.left = (long)0;                                        // Set Left Value To 0
  WindowRect.right = (long)GP_DEFAULT_WINDOW_WIDTH;                 // Set Right Value To Requested Width
  WindowRect.top = (long)0;                                         // Set Top Value To 0
  WindowRect.bottom = (long)GP_DEFAULT_WINDOW_HEIGHT;               // Set Bottom Value To Requested Height
  HWND                  hWnd;

  static PIXELFORMATDESCRIPTOR pfd =                                // pfd Tells Windows How We Want Things To Be
  {
    sizeof(PIXELFORMATDESCRIPTOR),                                  // Size Of This Pixel Format Descriptor
    1,                                                              // Version Number
    PFD_DRAW_TO_WINDOW |                                            // Format Must Support Window
    PFD_SUPPORT_OPENGL |                                            // Format Must Support OpenGL
    PFD_DOUBLEBUFFER,                                               // Must Support Double Buffering
    PFD_TYPE_RGBA,                                                  // Request An RGBA Format
    (BYTE)0,                                                        // Select Our Color Depth
    0, 0, 0, 0, 0, 0,                                               // Color Bits Ignored
    0,                                                              // No Alpha Buffer
    0,                                                              // Shift Bit Ignored
    0,                                                              // No Accumulation Buffer
    0, 0, 0, 0,                                                     // Accumulation Bits Ignored
    16,                                                             // 16Bit Z-Buffer (Depth Buffer)
    0,                                                              // No Stencil Buffer
    0,                                                              // No Auxiliary Buffer
    PFD_MAIN_PLANE,                                                 // Main Drawing Layer
    0,                                                              // Reserved
    0, 0, 0                                                         // Layer Masks Ignored
  };

  if (0)//fullscreen                                                // Are We Still In Fullscreen Mode?
  {
    dwExStyle = WS_EX_APPWINDOW;                                    // Window Extended Style
    dwStyle = WS_POPUP;                                             // Windows Style
  }
  else
  {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;                 // Window Extended Style
    dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
  }

  AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);       // Adjust Window To True Requested Size

  // Create The Window
  if (!(hWnd = CreateWindowEx(dwExStyle,                            // Extended Style For The Window
    "GraphicsPipeline",                                           // Class Name
    "WindowTitle",                                                // Window Title
    dwStyle |                                                     // Defined Window Style
    WS_CLIPSIBLINGS |                                             // Required Window Style
    WS_CLIPCHILDREN,                                              // Required Window Style
    0, 0,                                                         // Window Position
    WindowRect.right - WindowRect.left,                           // Calculate Window Width
    WindowRect.bottom - WindowRect.top,                           // Calculate Window Height
    NULL,                                                         // No Parent Window
    NULL,                                                         // No Menu
    GetModuleHandle(NULL),                                        // Instance
    window)))                                                     // Pass a pointer to this window
  {
    MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return NULL;
  }

  HDC hDC = GetDC(hWnd);

  if (context->mPixelFormat > 0)
    if (!SetPixelFormat(hDC, context->mPixelFormat, &pfd))            // Are We Able To Set The Pixel Format?
    {
      MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
      return NULL;
    }

  HGLRC hRC;
  if (!(hRC = wglCreateContext(hDC)))                               // Are We Able To Get A Rendering Context?
  {
    MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return NULL;
  }
  if (!wglMakeCurrent(hDC, hRC))                                    // Try To Activate The Rendering Context
  {
    MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return NULL;
  }

  _gp_api_init_context();

  wglShareLists(context->mShare, hRC);

  window->mWindow = hWnd;
  window->mContext = hRC;

  ShowWindow(window->mWindow, SW_SHOW);

  return window;
}

gp_pipeline* gp_window_get_pipeline(gp_window* window)
{
  return window->mPipeline;
}

void gp_window_redraw(gp_window* window)
{
  RedrawWindow(window->mWindow, 0, 0, RDW_INVALIDATE);
}

void gp_window_set_min_size(gp_window* window, int width, int height)
{
  window->mMinWidth = (width < 0) ? 0 : width;
  window->mMinHeight = (height < 0) ? 0 : height;
}

void gp_window_set_max_size(gp_window* window, int width, int height)
{
  window->mMaxWidth = (width < 0) ? 1000000 : width;
  window->mMaxHeight = (height < 0) ? 1000000 : height;
}

void gp_window_set_size(gp_window* window, unsigned int width, unsigned int height)
{
  SetWindowPos(window->mWindow, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  RECT rect;
  if (GetClientRect(window->mWindow, &rect))
  {
    if (width) *width = rect.right - rect.left;
    if (height) *height = rect.bottom - rect.top;
  }
}

void gp_window_set_position(gp_window* window, unsigned int x, unsigned int y)
{
  // TODO: Implement
}

void gp_window_get_position(gp_window* window, unsigned int* x, unsigned int* y)
{
  // TODO: Implement
}

#define _GP_SET_WINDOW_CALLBACK(name, cb, data)\
  void gp_window_set_ ## name ## _callback(gp_window* window, gp_event_ ## name ## _callback_t callback, gp_pointer* userData)\
  {\
    if(window->data) gp_object_unref((gp_object*)window->data);\
    \
    window->cb = callback;\
    window->data = userData;\
    \
    if(window->data) gp_object_ref((gp_object*)window->data);\
  }

_GP_SET_WINDOW_CALLBACK(click, mClickCB, mClickData)
_GP_SET_WINDOW_CALLBACK(track, mTrackCB, mTrackData)
_GP_SET_WINDOW_CALLBACK(enter, mEnterCB, mEnterData)
_GP_SET_WINDOW_CALLBACK(key, mKeyCB, mKeyData)
_GP_SET_WINDOW_CALLBACK(resize, mResizeCB, mResizeData)
_GP_SET_WINDOW_CALLBACK(move, mMoveCB, mMoveData)
