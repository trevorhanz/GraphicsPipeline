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
#include <GraphicsPipeline/Web.h>
#include <API/GLES/GLES.h>
#include "Web.h"

#include <stdlib.h>

gp_system* gp_system_new()
{
  gp_system* system = (gp_system*)malloc(sizeof(struct _gp_system));
  
  return system;
}

void gp_system_free(gp_system* system)
{
  free(system);
}

gp_context* gp_system_context_new(gp_system* system)
{
  gp_context* context = malloc(sizeof(gp_context));
  context->mParent = system;
  
  return context;
}

void gp_system_run(gp_system* system)
{
  gp_pipeline* pipeline = gp_target_get_pipeline(system->mTarget);
  _gp_pipeline_execute(pipeline);
}

