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

#include <GraphicsPipeline/Logging.h>

#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// using namespace GP;

#define LOG(color, prefix)\
  char* fmt;\
  if((isatty(fileno(stdout)) != 0))\
  {\
    int size = strlen(format)+strlen(prefix)+strlen(color)+10;\
    fmt = malloc(sizeof(char)*size);\
    snprintf(fmt, size, "%s%s%s\033[0m\n", color, prefix, format);\
  }\
  else\
  {\
    int size = strlen(format)+strlen(prefix)+2;\
    fmt = malloc(sizeof(char)*size);\
    snprintf(fmt, size, "%s%s\n", prefix, format);\
  }\
  va_list args;\
  va_start(args, format);\
  vprintf(fmt, args);\
  va_end(args);\
  free(fmt);

void gp_log(const char* format, ...)
{
  LOG("", "");
}

void gp_log_info(const char* format, ...)
{
  LOG("\033[36m", "INFO: ");
}

void gp_log_debug(const char* format, ...)
{
  LOG("\033[34m", "DEBUG: ");
}

void gp_log_warn(const char* format, ...)
{
  LOG("\033[33m", "WARNING: ");
}

void gp_log_error(const char* format, ...)
{
  LOG("\033[31m", "ERROR: ");
}
