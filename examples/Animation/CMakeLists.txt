if(EMSCRIPTEN)
  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

find_package(GraphicsPipeline REQUIRED COMPONENTS Interface OPTIONAL_COMPONENTS Native Qt5)

if(TARGET GP::Native)
  add_executable(Animation main.c)
  target_link_libraries(Animation GP::Interface GP::Native)

  add_executable(Animation++ main.cpp)
  target_link_libraries(Animation++ GP::Interface GP::Native)
endif()

if(TARGET GP::Qt5)
  add_executable(Animation_qt5 main.c)
  target_link_libraries(Animation_qt5 GP::Interface GP::Qt5)

  add_executable(Animation++_qt5 main.cpp)
  target_link_libraries(Animation++_qt5 GP::Interface GP::Qt5)
endif()

