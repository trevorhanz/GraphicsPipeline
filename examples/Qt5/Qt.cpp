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
    "#version 300 es                              \n"
    "in highp vec4 position;                      \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.xyz, 1.0);     \n"
    "}                                            \n";
const char* fragmentSource =
    "#version 300 es                              \n"
    "out highp vec4 fragColor;                    \n"
    "void main()                                  \n"
    "{                                            \n"
    "  fragColor = vec4(0.0f, 1.0f, 0.0f, 0.0f);  \n"
    "}                                            \n";
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  
  QWidget window;
  
  window.resize(250, 150);
  window.setWindowTitle("Simple example");
  
  gp_context* context = gp_qt_context_new();
  
  gp_target* target = gp_target_new(context);
  gp_array* array = gp_array_new(context);
  gp_shader* shader = gp_shader_new(context);
  
  gp_array_data* ad = gp_array_data_new();
  gp_array_data_set(ad, vertexData, 6);
  gp_array_set_data(array, ad);
  gp_array_data_unref(ad);
  
  gp_shader_source* source = gp_shader_source_new();
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_VERTEX, vertexSource);
  gp_shader_source_add_from_string(source, GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  gp_shader_compile(shader, source);
  gp_shader_source_unref(source);
  
  gp_pipeline* pipeline = gp_target_get_pipeline(target);
  
  gp_operation* draw = gp_operation_draw_new();
  gp_operation_draw_set_shader(draw, shader);
  gp_operation_draw_add_array_by_index(draw, array, 0, 2, 0, 0);
  gp_operation_draw_set_verticies(draw, 3);
  gp_pipeline_add_operation(pipeline, draw);
  
  gp_target_get_qwidget(target)->show();
  QHBoxLayout* layout = new QHBoxLayout();
  layout->addWidget(gp_target_get_qwidget(target));
  
  window.setLayout(layout);
  window.show();
  
  return app.exec();
}
