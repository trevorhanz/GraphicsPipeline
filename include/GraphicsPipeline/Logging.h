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

//! \file Logging.h

#ifndef __GP_LOGGING_H__
#define __GP_LOGGING_H__

namespace GP
{
  /*!
   * Generate formatted log output.
   * \param format Printf sytle format string.
   */
  void Log(const char* format, ...);
  
  /*!
   * Generate informative formatted log output.
   * \param format Printf sytle format string.
   */
  void LogI(const char* format, ...);
  
  /*!
   * Generate debug formatted log output.
   * \param format Printf sytle format string.
   */
  void LogD(const char* format, ...);
  
  /*!
   * Generate warning formatted log output.
   * \param format Printf sytle format string.
   */
  void LogW(const char* format, ...);
  
  /*!
   * Generate error formatted log output.
   * \param format Printf sytle format string.
   */
  void LogE(const char* format, ...);
}

#endif // __GP_LOGGING_H__