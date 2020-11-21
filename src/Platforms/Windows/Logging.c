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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#define LOG(color, prefix)\
  char* fmt;\
  size_t size = strlen(format)+strlen(prefix)+2;\
  fmt = malloc(sizeof(char)*size);\
  snprintf(fmt, size, "%s%s\n", prefix, format);\
  va_list args;\
  va_start(args, format);\
  vprintf(fmt, args);\
  va_end(args);\
  free(fmt);

void Log(const char* format, ...)
{
  LOG("", "");
}

void LogI(const char* format, ...)
{
  LOG("\033[36m", "INFO: ");
}

void LogD(const char* format, ...)
{
  LOG("\033[34m", "DEBUG: ");
}

void LogW(const char* format, ...)
{
  LOG("\033[33m", "WARNING: ");
}

void LogE(const char* format, ...)
{
  LOG("\033[31m", "ERROR: ");
}
