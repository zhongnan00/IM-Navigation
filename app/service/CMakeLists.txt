include_directories(../CombinedAPI/library/include 
                    ../CombinedAPI/library/src/include 
                    ./)


add_executable(NdiSampleService NdiSampleService.cpp NdiSampleApp.cpp)     

target_link_libraries(NdiSampleService PRIVATE CombinedAPI)