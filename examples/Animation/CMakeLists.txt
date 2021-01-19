if(EMSCRIPTEN)
  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

add_executable(Animation main.c)
target_link_libraries(Animation Native)

add_executable(Animation++ main.cpp)
target_link_libraries(Animation++ Native)

if(GP_BUILD_QT5)
  add_executable(Animation_qt5 main.c)
  target_link_libraries(Animation_qt5 GP::Qt5)

  add_executable(Animation++_qt5 main.cpp)
  target_link_libraries(Animation++_qt5 GP_Qt5)
endif(GP_BUILD_QT5)

