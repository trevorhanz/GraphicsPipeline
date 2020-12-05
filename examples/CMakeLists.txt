
add_subdirectory(Simple)

if(EMSCRIPTEN)
  add_subdirectory(Web)
endif(EMSCRIPTEN)

if(GP_BUILD_QT5)
  add_subdirectory(Qt5)
endif(GP_BUILD_QT5)
