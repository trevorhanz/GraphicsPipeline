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

#include <GraphicsPipeline/GP.h>

#include "gtest/gtest.h"

TEST(C, Init)
{
  gp_system* system = gp_system_new();
  ASSERT_NE(0, (uintptr_t)system);
  
  gp_context* context = gp_context_new(system);
  ASSERT_NE(0, (uintptr_t)context);
  
  gp_object_ref((gp_object*)context);
  gp_object_unref((gp_object*)context);
  
  gp_object_unref((gp_object*)system);
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
