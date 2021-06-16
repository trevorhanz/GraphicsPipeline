if(EMSCRIPTEN)
  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

find_package(GraphicsPipeline REQUIRED COMPONENTS Interface OPTIONAL_COMPONENTS Native Qt5)

if(TARGET GP::Native)
  add_executable(Input main.c)
  target_link_libraries(Input GP::Interface GP::Native)

  add_executable(Input++ main.cpp)
  target_link_libraries(Input++ GP::Interface GP::Native)
endif()

if(TARGET GP::Qt5)
  add_executable(Input_qt5 main.c)
  target_link_libraries(Input_qt5 GP::Interface GP::Qt5)
  
  add_executable(Input++_qt5 main.cpp)
  target_link_libraries(Input++_qt5 GP::Interface GP::Qt5)
endif(TARGET GP::Qt5)


