
if(EMSCRIPTEN)
  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

find_package(GraphicsPipeline COMPONENTS Interface OPTIONAL_COMPONENTS Native Qt5)

if(TARGET GP::Native)
  add_executable(Simple main.c)
  target_link_libraries(Simple GP::Interface GP::Native)

  add_executable(Simple++ main.cpp)
  target_link_libraries(Simple++ GP::Interface GP::Native)
endif()

if(TARGET GP::Qt5)
  add_executable(Simple_qt5 main.c)
  target_link_libraries(Simple_qt5 GP::Interface GP::Qt5)

  add_executable(Simple++_qt5 main.cpp)
  target_link_libraries(Simple++_qt5 GP::Interface GP::Qt5)
endif()
