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

#include <GraphicsPipeline/GP.h>
#include <GraphicsPipeline/Qt5.h>
#include <GraphicsPipeline/System.h>

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>

const char* vertexSource =
    "attribute vec4 position;                     \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.xyz, 1.0);     \n"
    "}                                            \n";
const char* fragmentSource =
    "precision mediump float;\n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);   \n"
    "}                                            \n";
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  
  QWidget window;
  
  window.resize(250, 150);
  window.setWindowTitle("Simple example");
  
  gp_context* context = gp_qt_context_new();
  
  gp_target* target = gp_context_target_new(context);
  gp_array* array = gp_context_array_new(context);
  gp_shader* shader = gp_context_shader_new(context);
  
  gp_array_set_data(array, vertexData, 6);
  
  gp_shader_compile(shader, vertexSource, fragmentSource);
  
  gp_pipeline* pipeline = gp_target_get_pipeline(target);
  
  gp_pipeline_add_draw(pipeline, shader, array);
  
  gp_target_get_qwidget(target)->show();
  QHBoxLayout* layout = new QHBoxLayout();
  layout->addWidget(gp_target_get_qwidget(target));
  
  window.setLayout(layout);
  window.show();
  
  return app.exec();
}
