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

#include <string>
#include <stdarg.h>
#include <unistd.h>

using namespace GP;

#define LOG(color, prefix)\
  std::string fmt = prefix;\
  fmt += format;\
  if((isatty(fileno(stdout)) != 0))\
  {\
    fmt = color + fmt + "\u001b[0m";\
  }\
  fmt += '\n';\
  va_list args;\
  va_start(args, format);\
  vprintf(fmt.c_str(), args);\
  va_end(args);

void GP::Log(const char* format, ...)
{
  LOG("", "");
}

void GP::LogI(const char* format, ...)
{
  LOG("\u001b[36m", "INFO: ");
}

void GP::LogD(const char* format, ...)
{
  LOG("\u001b[34m", "DEBUG: ");
}

void GP::LogW(const char* format, ...)
{
  LOG("\u001b[33m", "WARNING: ");
}

void GP::LogE(const char* format, ...)
{
  LOG("\u001b[31m", "ERROR: ");
}
