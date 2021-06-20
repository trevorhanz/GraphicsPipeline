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

#ifndef __TIMER_H__
#define __TIMER_H__

#include <GraphicsPipeline/Types.h>

@interface Timer : NSObject
{
  NSTimer*                  mTimer;
  gp_timer*                 mGPTimer;
  gp_timer_callback         mCallback;
  gp_pointer*               mUserData;
}

- (void) dealloc;
- (Timer*) init:(gp_timer*)timer;
- (void) Arm:(double)seconds;
- (void) Disarm;
- (void) HandleTimeout:(NSTimer*)timer;
- (void) SetCallback:(gp_timer_callback) callback;
- (void) SetUserData:(gp_pointer*)userdata;
- (gp_pointer*) GetUserData;

@end

#endif // __TIMER_H__
