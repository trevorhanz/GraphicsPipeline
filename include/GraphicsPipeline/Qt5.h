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

//! \file Qt5.h

#ifndef __GP_QT5_H__
#define __GP_QT5_H__

#include <GraphicsPipeline/GP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Create a new system object from cli arguments.
 * \param argc Pointer to number of arguments
 * \param argv Array of c strings containing arguments
 * \return Newly created gp_system.
 */
gp_system* gp_qt_system_new(int* argc, char** argv);

/*!
 * Create a new gp_context attached to existing QAplication.
 * \return Newly created gp_context.
 */
gp_context* gp_qt_context_new();

#ifdef __cplusplus
}

#include <QWidget>

/*!
 * Retrieve QWidget associated with a gp_target.
 * \param target Pointer to gp_target.
 * \return Pointer to QWidget used by target.
 */
QWidget* gp_target_get_qwidget(gp_target* target);
#endif

#endif // __GP_QT5_H__
