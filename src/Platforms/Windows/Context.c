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

#include <GraphicsPipeline/Context.h>
#include "API/GL/GL.h"
#include "Windows.h"

void gp_context_free(gp_context* context)
{
  free(context);
}

gp_target* gp_context_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(struct _gp_target));
  target->mParent = context;
  target->mPipeline = malloc(sizeof(struct _gp_pipeline));
  target->mPipeline->mOperations = NULL;

  DWORD                 dwExStyle;                                  // Window Extended Style
  DWORD                 dwStyle;                                    // Window Style
  RECT                  WindowRect;                                 // Grabs Rectangle Upper Left / Lower Right Values
  WindowRect.left = (long)0;                                        // Set Left Value To 0
  WindowRect.right = (long)720;                                     // Set Right Value To Requested Width
  WindowRect.top = (long)0;                                         // Set Top Value To 0
  WindowRect.bottom = (long)480;                                    // Set Bottom Value To Requested Height
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
    dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
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
      target)))                                                     // Pass a pointer to this window
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

  wglShareLists(context->mShare, hRC);

  target->mWindow = hWnd;
  target->mContext = hRC;
  target->mParent->mParent->mTarget = target;

  ShowWindow(target->mWindow, SW_SHOW);

  return target;
}

gp_array* gp_context_array_new(gp_context* context)
{
  gp_array* array = malloc(sizeof(struct _gp_array));

  _gp_generate_array(array);

  return array;
}

gp_shader* gp_context_shader_new(gp_context* context)
{
  gp_shader* shader = malloc(sizeof(struct _gp_shader));

  return shader;
}

gp_pipeline* gp_target_get_pipeline(gp_target* target)
{
  return target->mPipeline;
}

void gp_target_redraw(gp_target* target)
{
  SendMessage(target->mWindow, WM_REDRAW, target, 0);
}


