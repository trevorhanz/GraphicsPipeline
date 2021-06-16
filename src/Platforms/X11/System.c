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

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Window.h>
#include <GraphicsPipeline/X11.h>
#include "X11.h"
#include "API/GL/GL.h"

#include <X11/XKBlib.h>
#include <GL/glew.h>
#include <GL/glx.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/select.h>

void _gp_system_free(gp_object* object)
{
  gp_system* system = (gp_system*)object;
  
  XCloseDisplay(system->mDisplay);
  
  _gp_event_free(system->mEvent);
  free(system);
}

gp_system* gp_system_new()
{
  gp_system* system = (gp_system*)malloc(sizeof(struct _gp_system));
  _gp_object_init(&system->mObject, _gp_system_free);
  gp_list_init(&system->mTargets);
  system->mEvent = _gp_event_new();
  
  char* display = getenv("DISPLAY");
  system->mDisplay = XOpenDisplay(display);
  if(!system->mDisplay)
  {
    gp_log_error("Unable to connect to X11 server (%s).", (display)?display:":0");
    free(system);
    return NULL;
  }
  
  system->mDeleteMessage = XInternAtom(system->mDisplay, "WM_DELETE_WINDOW", False);
  
  XkbDescPtr desc = XkbGetMap(system->mDisplay, 0, XkbUseCoreKbd);
  XkbGetNames(system->mDisplay, XkbKeyNamesMask | XkbKeyAliasesMask, desc);
  
  system->mKeyMap.mMaxCode = desc->max_key_code;
  system->mKeyMap.mMinCode = desc->min_key_code;
  size_t size = sizeof(_gp_key_map)*(system->mKeyMap.mMaxCode-system->mKeyMap.mMinCode);
  system->mKeyMap.mKeys = malloc(size);
  memset(system->mKeyMap.mKeys, 0, size);
  
  const struct
  {
    int key;
    char* name;
  } keymap[] =
  {
    { GP_KEY_TILDE, "TLDE" },
    { GP_KEY_1, "AE01" },
    { GP_KEY_2, "AE02" },
    { GP_KEY_3, "AE03" },
    { GP_KEY_4, "AE04" },
    { GP_KEY_5, "AE05" },
    { GP_KEY_6, "AE06" },
    { GP_KEY_7, "AE07" },
    { GP_KEY_8, "AE08" },
    { GP_KEY_9, "AE09" },
    { GP_KEY_0, "AE10" },
    { GP_KEY_MINUS, "AE11" },
    { GP_KEY_EQUAL, "AE12" },
    { GP_KEY_Q, "AD01" },
    { GP_KEY_W, "AD02" },
    { GP_KEY_E, "AD03" },
    { GP_KEY_R, "AD04" },
    { GP_KEY_T, "AD05" },
    { GP_KEY_Y, "AD06" },
    { GP_KEY_U, "AD07" },
    { GP_KEY_I, "AD08" },
    { GP_KEY_O, "AD09" },
    { GP_KEY_P, "AD10" },
    { GP_KEY_LEFT_BRACKET, "AD11" },
    { GP_KEY_RIGHT_BRACKET, "AD12" },
    { GP_KEY_A, "AC01" },
    { GP_KEY_S, "AC02" },
    { GP_KEY_D, "AC03" },
    { GP_KEY_F, "AC04" },
    { GP_KEY_G, "AC05" },
    { GP_KEY_H, "AC06" },
    { GP_KEY_J, "AC07" },
    { GP_KEY_K, "AC08" },
    { GP_KEY_L, "AC09" },
    { GP_KEY_SEMICOLON, "AC10" },
    { GP_KEY_APOSTROPHE, "AC11" },
    { GP_KEY_Z, "AB01" },
    { GP_KEY_X, "AB02" },
    { GP_KEY_C, "AB03" },
    { GP_KEY_V, "AB04" },
    { GP_KEY_B, "AB05" },
    { GP_KEY_N, "AB06" },
    { GP_KEY_M, "AB07" },
    { GP_KEY_COMMA, "AB08" },
    { GP_KEY_PERIOD, "AB09" },
    { GP_KEY_SLASH, "AB10" },
    { GP_KEY_BACKSLASH, "BKSL" },
    { GP_KEY_WORLD_1, "LSGT" },
    { GP_KEY_SPACE, "SPCE" },
    { GP_KEY_ESCAPE, "ESC" },
    { GP_KEY_ENTER, "RTRN" },
    { GP_KEY_TAB, "TAB" },
    { GP_KEY_BACKSPACE, "BKSP" },
    { GP_KEY_INSERT, "INS" },
    { GP_KEY_DELETE, "DELE" },
    { GP_KEY_RIGHT, "RGHT" },
    { GP_KEY_LEFT, "LEFT" },
    { GP_KEY_DOWN, "DOWN" },
    { GP_KEY_UP, "UP" },
    { GP_KEY_PAGE_UP, "PGUP" },
    { GP_KEY_PAGE_DOWN, "PGDN" },
    { GP_KEY_HOME, "HOME" },
    { GP_KEY_END, "END" },
    { GP_KEY_CAPS_LOCK, "CAPS" },
    { GP_KEY_SCROLL_LOCK, "SCLK" },
    { GP_KEY_NUM_LOCK, "NMLK" },
    { GP_KEY_PRINT_SCREEN, "PRSC" },
    { GP_KEY_PAUSE, "PAUS" },
    { GP_KEY_F1, "FK01" },
    { GP_KEY_F2, "FK02" },
    { GP_KEY_F3, "FK03" },
    { GP_KEY_F4, "FK04" },
    { GP_KEY_F5, "FK05" },
    { GP_KEY_F6, "FK06" },
    { GP_KEY_F7, "FK07" },
    { GP_KEY_F8, "FK08" },
    { GP_KEY_F9, "FK09" },
    { GP_KEY_F10, "FK10" },
    { GP_KEY_F11, "FK11" },
    { GP_KEY_F12, "FK12" },
    { GP_KEY_F13, "FK13" },
    { GP_KEY_F14, "FK14" },
    { GP_KEY_F15, "FK15" },
    { GP_KEY_F16, "FK16" },
    { GP_KEY_F17, "FK17" },
    { GP_KEY_F18, "FK18" },
    { GP_KEY_F19, "FK19" },
    { GP_KEY_F20, "FK20" },
    { GP_KEY_F21, "FK21" },
    { GP_KEY_F22, "FK22" },
    { GP_KEY_F23, "FK23" },
    { GP_KEY_F24, "FK24" },
    { GP_KEY_F25, "FK25" },
    { GP_KEY_KP_0, "KP0" },
    { GP_KEY_KP_1, "KP1" },
    { GP_KEY_KP_2, "KP2" },
    { GP_KEY_KP_3, "KP3" },
    { GP_KEY_KP_4, "KP4" },
    { GP_KEY_KP_5, "KP5" },
    { GP_KEY_KP_6, "KP6" },
    { GP_KEY_KP_7, "KP7" },
    { GP_KEY_KP_8, "KP8" },
    { GP_KEY_KP_9, "KP9" },
    { GP_KEY_KP_DECIMAL, "KPDL" },
    { GP_KEY_KP_DIVIDE, "KPDV" },
    { GP_KEY_KP_MULTIPLY, "KPMU" },
    { GP_KEY_KP_SUBTRACT, "KPSU" },
    { GP_KEY_KP_ADD, "KPAD" },
    { GP_KEY_KP_ENTER, "KPEN" },
    { GP_KEY_KP_EQUAL, "KPEQ" },
    { GP_KEY_LEFT_SHIFT, "LFSH" },
    { GP_KEY_LEFT_CONTROL, "LCTL" },
    { GP_KEY_LEFT_ALT, "LALT" },
    { GP_KEY_LEFT_SUPER, "LWIN" },
    { GP_KEY_RIGHT_SHIFT, "RTSH" },
    { GP_KEY_RIGHT_CONTROL, "RCTL" },
    { GP_KEY_RIGHT_ALT, "RALT" },
    { GP_KEY_RIGHT_ALT, "LVL3" },
    { GP_KEY_RIGHT_ALT, "MDSW" },
    { GP_KEY_RIGHT_SUPER, "RWIN" },
    { GP_KEY_MENU, "MENU" }
  };
  
  for(int scancode = system->mKeyMap.mMinCode;  scancode <= system->mKeyMap.mMaxCode;  ++scancode)
  {
    for (int i = 0;  i < sizeof(keymap) / sizeof(keymap[0]);  i++)
    {
      if (strncmp(desc->names->keys[scancode].name,
                  keymap[i].name,
                  XkbKeyNameLength) == 0)
      {
        system->mKeyMap.mKeys[scancode-system->mKeyMap.mMinCode].key = keymap[i].key;
        break;
      }
    }
  }
  
  XkbFreeNames(desc, XkbKeyNamesMask, True);
  XkbFreeKeyboard(desc, 0, True);
  
  return system;
}

gp_window* _gp_system_find_window(gp_system* system, Window window)
{
  gp_list_node* node = gp_list_front(&system->mTargets);
  while(node != NULL)
  {
    gp_window* w = (gp_window*)node;
    if(w->mWindow == window)
    {
      return w;
    }
    node = gp_list_node_next(node);
  }
  return NULL;
}

void _gp_system_process_events(gp_io* io)
{
  gp_system* system = (gp_system*)gp_io_get_userdata(io);
  
  XEvent event;
  
  while(XPending(system->mDisplay))
  {
    XNextEvent(system->mDisplay, &event);
    
    switch(event.type)
    {
      case Expose: {
        gp_window* window = _gp_system_find_window(system, event.xexpose.window);
        if(window)
        {
          gp_window_redraw(window);
        }
      } break;
      case ClientMessage:
        if(event.type == ClientMessage)
        {
          if(event.xclient.data.l[0] == system->mDeleteMessage)
          {
            gp_system_stop(system);
          }
        }
        break;
      case ButtonPress:
      case ButtonRelease:
      {
        gp_window* window = _gp_system_find_window(system, event.xbutton.window);
        if(window && window->mClickCB)
        {
          gp_event_click_t input;
          input.x = event.xbutton.x;
          input.y = event.xbutton.y;
          input.state = (event.type == ButtonPress);
          switch(event.xbutton.button)
          {
            case Button1: input.button = GP_BUTTON_LEFT; break;
            case Button2: input.button = GP_BUTTON_MIDDLE; break;
            case Button3: input.button = GP_BUTTON_RIGHT; break;
            default: input.button = 0; break;
          }
          
          if(input.button != 0)
            window->mClickCB(&input, window->mClickData);
        }
      } break;
      case MotionNotify:
      {
        gp_window* window = _gp_system_find_window(system, event.xbutton.window);
        if(window && window->mMoveCB)
        {
          gp_event_move_t input;
          input.x = event.xmotion.x;
          input.y = event.xmotion.y;
          
          window->mMoveCB(&input, window->mMoveData);
        }
      } break;
      case EnterNotify:
      case LeaveNotify:
      {
        gp_window* window = _gp_system_find_window(system, event.xany.window);
        if(window && window->mEnterCB)
        {
          gp_event_enter_t input;
          input.enter = (event.type == EnterNotify)?1:0;
          
          window->mEnterCB(&input, window->mEnterData);
        }
      } break;
      case KeyPress:
      case KeyRelease:
      {
        gp_window* window = _gp_system_find_window(system, event.xany.window);
        if(window && window->mKeyCB)
        {
          KeySym keysym = XLookupKeysym(&event.xkey, 0);
          
          gp_event_key_t input;
          input.state = (event.type == KeyPress)?1:0;
          if(event.xkey.keycode < system->mKeyMap.mMinCode ||
             event.xkey.keycode > system->mKeyMap.mMaxCode)
            input.key = GP_KEY_UNKNOWN;
          else
            input.key = system->mKeyMap.mKeys[event.xkey.keycode-system->mKeyMap.mMinCode].key;
          
          window->mKeyCB(&input, window->mKeyData);
        }
      } break;
      case ConfigureNotify:
      {
        gp_window* window = _gp_system_find_window(system, event.xany.window);
        if(window && window->mResizeCB)
        {
          gp_event_resize_t input;
          input.width = event.xconfigure.width;
          input.height = event.xconfigure.height;
          
          window->mResizeCB(&input, window->mResizeData);
        }
      } break;
    }
  }
}

void gp_system_run(gp_system* system)
{
  assert(system != NULL);
  
  int fd = XConnectionNumber(system->mDisplay);
  
  gp_io* io = gp_io_read_new(system, fd);
  gp_io_set_callback(io, _gp_system_process_events);
  gp_io_set_userdata(io, (void*)system);
  
  _gp_event_run(system->mEvent);
  
  gp_object_unref((gp_object*)io);
}

void gp_system_stop(gp_system* system)
{
  _gp_event_stop(system->mEvent);
}

gp_timer* gp_timer_new(gp_system* system)
{
  return _gp_event_timer_new(system->mEvent);
}

gp_io* gp_io_read_new(gp_system* system, int fd)
{
  return _gp_event_io_read_new(system->mEvent, fd);
}

gp_io* gp_io_write_new(gp_system* system, int fd)
{
  return _gp_event_io_write_new(system->mEvent, fd);
}


