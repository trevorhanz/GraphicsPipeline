
if(EMSCRIPTEN)
  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

add_executable(Simple main.c)
target_link_libraries(Simple Native)

add_executable(Simple++ main.cpp)
target_link_libraries(Simple++ Native)

if(GP_BUILD_QT5)
  add_executable(Simple_qt5 main.c)
  target_link_libraries(Simple_qt5 GP::Qt5)

  add_executable(Simple++_qt5 main.cpp)
  target_link_libraries(Simple++_qt5 GP_Qt5)
endif(GP_BUILD_QT5)
