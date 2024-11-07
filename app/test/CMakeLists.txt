add_executable(hello_world hello_world.cpp)
# target_link_libraries(hello_world ${PROJECT_NAME})


add_executable(TestNdiSampleApp TestNdiSampleApp.cpp)


target_link_libraries(TestNdiSampleApp PRIVATE libgtest.a gtest_main pthread NdiSampleApp)
