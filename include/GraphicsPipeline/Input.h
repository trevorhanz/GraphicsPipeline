/************************************************************************
* Copyright (C) 2021 Trevor Hanz
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

//! \file Input.h

#ifndef __GP_INPUT_H__
#define __GP_INPUT_H__

/*!
 * \defgroup Input
 * \{
 */

typedef unsigned char gp_state_t;
typedef unsigned char gp_button_t;
typedef unsigned char gp_key_t;

/*!
 * Structure for mouse click input.
 */
typedef struct 
{
  int             x;
  int             y;
  gp_button_t     button;
  gp_state_t      state;
} gp_event_click_t;

/*!
 * Structure for scroll input.
 */
typedef struct
{
  float           scroll;     //!< Amount scrolled.
} gp_event_scroll_t;

/*!
 * Structure for mouse movement input.
 */
typedef struct 
{
  int             x;
  int             y;
} gp_event_track_t;

/*!
 * Structure for mouse enter/leave input.
 */
typedef struct
{
  int             enter;      //!< 1 if entering, 0 if leaving
} gp_event_enter_t;

/*!
 * Structure for keyboard input.
 */
typedef struct
{
  gp_key_t        key;
  unsigned        code;
  gp_state_t      state;
} gp_event_key_t;

/*!
 * \name States
 * \{
 */
#define GP_PRESSED                  0x01
#define GP_RELEASED                 0x00
//! \}

/*!
 * \name Buttons
 * \{
 */
#define GP_BUTTON_LEFT              0x01
#define GP_BUTTON_MIDDLE            0x02
#define GP_BUTTON_RIGHT             0x03
//! \}

/*!
 * \name Modifiers
 * \{
 */
#define GP_MOD_CTRL                 0x01
#define GP_MOD_ALT                  0x02
#define GP_MOD_SHFT                 0x04
//! \}

/*!
 * \name Keys
 * \{
 */
#define GP_KEY_UNKNOWN              0

#define GP_KEY_A                    1
#define GP_KEY_B                    2
#define GP_KEY_C                    3
#define GP_KEY_D                    4
#define GP_KEY_E                    5
#define GP_KEY_F                    6
#define GP_KEY_G                    7
#define GP_KEY_H                    8
#define GP_KEY_I                    9
#define GP_KEY_J                    10
#define GP_KEY_K                    11
#define GP_KEY_L                    12
#define GP_KEY_M                    13
#define GP_KEY_N                    14
#define GP_KEY_O                    15
#define GP_KEY_P                    16
#define GP_KEY_Q                    17
#define GP_KEY_R                    18
#define GP_KEY_S                    19
#define GP_KEY_T                    20
#define GP_KEY_U                    21
#define GP_KEY_V                    22
#define GP_KEY_W                    23
#define GP_KEY_X                    24
#define GP_KEY_Y                    25
#define GP_KEY_Z                    26

#define GP_KEY_0                    50
#define GP_KEY_1                    51
#define GP_KEY_2                    52
#define GP_KEY_3                    53
#define GP_KEY_4                    54
#define GP_KEY_5                    55
#define GP_KEY_6                    56
#define GP_KEY_7                    57
#define GP_KEY_8                    58
#define GP_KEY_9                    59

#define GP_KEY_F1                   60
#define GP_KEY_F2                   61
#define GP_KEY_F3                   62
#define GP_KEY_F4                   63
#define GP_KEY_F5                   64
#define GP_KEY_F6                   65
#define GP_KEY_F7                   66
#define GP_KEY_F8                   67
#define GP_KEY_F9                   68
#define GP_KEY_F10                  69
#define GP_KEY_F11                  70
#define GP_KEY_F12                  71
#define GP_KEY_F13                  72
#define GP_KEY_F14                  73
#define GP_KEY_F15                  74
#define GP_KEY_F16                  75
#define GP_KEY_F17                  76
#define GP_KEY_F18                  77
#define GP_KEY_F19                  78
#define GP_KEY_F20                  79
#define GP_KEY_F21                  80
#define GP_KEY_F22                  81
#define GP_KEY_F23                  82
#define GP_KEY_F24                  83
#define GP_KEY_F25                  84

#define GP_KEY_UP                   100
#define GP_KEY_DOWN                 101
#define GP_KEY_LEFT                 102
#define GP_KEY_RIGHT                103

#define GP_KEY_TILDE                120
#define GP_KEY_MINUS                121
#define GP_KEY_EQUAL                122
#define GP_KEY_LEFT_BRACKET         123
#define GP_KEY_RIGHT_BRACKET        124
#define GP_KEY_SEMICOLON            125
#define GP_KEY_APOSTROPHE           126
#define GP_KEY_COMMA                127
#define GP_KEY_PERIOD               128
#define GP_KEY_SLASH                129
#define GP_KEY_BACKSLASH            130
#define GP_KEY_WORLD_1              131
#define GP_KEY_SPACE                132
#define GP_KEY_ESCAPE               133
#define GP_KEY_ENTER                134
#define GP_KEY_TAB                  135
#define GP_KEY_BACKSPACE            136
#define GP_KEY_INSERT               137
#define GP_KEY_DELETE               138
#define GP_KEY_PAGE_UP              139
#define GP_KEY_PAGE_DOWN            140
#define GP_KEY_HOME                 141
#define GP_KEY_END                  142
#define GP_KEY_CAPS_LOCK            143
#define GP_KEY_SCROLL_LOCK          144
#define GP_KEY_NUM_LOCK             145
#define GP_KEY_PRINT_SCREEN         146
#define GP_KEY_PAUSE                147

#define GP_KEY_KP_0                 150
#define GP_KEY_KP_1                 151
#define GP_KEY_KP_2                 152
#define GP_KEY_KP_3                 153
#define GP_KEY_KP_4                 154
#define GP_KEY_KP_5                 155
#define GP_KEY_KP_6                 156
#define GP_KEY_KP_7                 157
#define GP_KEY_KP_8                 158
#define GP_KEY_KP_9                 159
#define GP_KEY_KP_DECIMAL           160
#define GP_KEY_KP_DIVIDE            161
#define GP_KEY_KP_MULTIPLY          162
#define GP_KEY_KP_SUBTRACT          163
#define GP_KEY_KP_ADD               164
#define GP_KEY_KP_ENTER             165
#define GP_KEY_KP_EQUAL             166

#define GP_KEY_LEFT_SHIFT           170
#define GP_KEY_LEFT_CONTROL         171
#define GP_KEY_LEFT_ALT             172
#define GP_KEY_LEFT_SUPER           173
#define GP_KEY_RIGHT_SHIFT          174
#define GP_KEY_RIGHT_CONTROL        175
#define GP_KEY_RIGHT_ALT            176
#define GP_KEY_RIGHT_SUPER          177
#define GP_KEY_MENU                 178

//! \} // Keys

//! \} // input

#endif // __GP_INPUT_H__
