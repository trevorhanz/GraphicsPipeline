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

#include <emscripten.h>

#include <stdarg.h>

#define LOG(level)\
  va_list args;\
  va_start(args, format);\
  emscripten_log(level, format, args);\
  va_end(args);

void Log(const char* format, ...)
{
  LOG(EM_LOG_INFO);
}

void LogI(const char* format, ...)
{
  LOG(EM_LOG_INFO);
}

void LogD(const char* format, ...)
{
  LOG(EM_LOG_DEBUG);
}

void LogW(const char* format, ...)
{
  LOG(EM_LOG_WARN);
}

void LogE(const char* format, ...)
{
  LOG(EM_LOG_ERROR);
}
