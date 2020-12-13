if(EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")

  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

add_executable(Timer main.c)
target_link_libraries(Timer Native)

# add_executable(Timer++ main.cpp)
# target_link_libraries(Simple++ Native)

if(GP_BUILD_QT5)
  add_executable(Timer_qt5 main.c)
  target_link_libraries(Timer_qt5 GP::Qt5)

#   add_executable(Simple++_qt5 main.cpp)
#   target_link_libraries(Simple++_qt5 GP_Qt5)
endif(GP_BUILD_QT5)
