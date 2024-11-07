include_directories(../CombinedAPI/library/include 
                    ../CombinedAPI/library/src/include 
                    ./)

            
add_library(NdiSampleApp STATIC NdiSampleApp.cpp)
target_link_libraries(NdiSampleApp PRIVATE CombinedAPI)

add_executable(NdiSampleService NdiSampleService.cpp NdiSampleApp.cpp)     
target_link_libraries(NdiSampleService PRIVATE CombinedAPI)