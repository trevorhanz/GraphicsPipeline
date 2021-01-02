
find_package(GTest REQUIRED)

include(GoogleTest)
add_executable(TestC TestC.cpp)
target_link_libraries(TestC GTest::GTest Native)
gtest_discover_tests(TestC XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_executable(TestCPP TestCPP.cpp)
target_link_libraries(TestCPP GTest::GTest Native)
gtest_discover_tests(TestCPP XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
