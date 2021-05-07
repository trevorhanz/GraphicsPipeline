

find_package(GraphicsPipeline COMPONENTS Interface OPTIONAL_COMPONENTS Native Qt5)

if(TARGET GP::Native)
  add_executable(Loading++ main.cpp)
  target_link_libraries(Loading++ GP::Interface GP::Native)
endif()

if(TARGET GP::Qt5)
  add_executable(Loading++_qt5 main.cpp)
  target_link_libraries(Loading++_qt5 GP::Interface GP::Qt5)
endif()

if(EMSCRIPTEN)
  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
  set_target_properties(Loading++ PROPERTIES LINK_FLAGS "--preload-file data/triangle.txt --preload-file data/square.txt")
endif(EMSCRIPTEN)

configure_file(data/triangle.txt ${CMAKE_CURRENT_BINARY_DIR}/data/triangle.txt COPYONLY)
configure_file(data/square.txt ${CMAKE_CURRENT_BINARY_DIR}/data/square.txt COPYONLY)
