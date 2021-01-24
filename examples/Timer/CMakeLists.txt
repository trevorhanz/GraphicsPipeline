if(EMSCRIPTEN)
  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

find_package(GraphicsPipeline REQUIRED COMPONENTS Interface OPTIONAL_COMPONENTS Native Qt5)

if(TARGET GP::Native)
  add_executable(Timer main.c)
  target_link_libraries(Timer GP::Interface GP::Native)

  add_executable(Timer++ main.cpp)
  target_link_libraries(Timer++ GP::Interface GP::Native)
endif()

if(TARGET GP::Qt5)
  add_executable(Timer_qt5 main.c)
  target_link_libraries(Timer_qt5 GP::Interface GP::Qt5)

  add_executable(Timer++_qt5 main.cpp)
  target_link_libraries(Timer++_qt5 GP::Interface GP::Qt5)
endif()
