
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(GraphicsPipeline REQUIRED COMPONENTS Interface Qt5)

add_executable(Qt Qt.cpp)
target_link_libraries(Qt GP::Interface GP::Qt5 Qt5::Widgets)

add_executable(Qt++ Qt++.cpp)
target_link_libraries(Qt++ GP::Interface GP::Qt5 Qt5::Widgets)
