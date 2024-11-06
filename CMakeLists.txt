cmake_minimum_required(VERSION 3.10)
project(IM-Navigation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_INCLUDE_CURRENT_DIR ON)   
set(CMAKE_POSITION_INDEPENDENT_CODE ON)    
set(CMAKE_BUILD_TYPE debug)


include_directories(.)

find_package(VTK REQUIRED)
if (NOT VTK_FOUND)
  message(FATAL_ERROR "Unable to find the VTK build folder.")
endif()

add_subdirectory(app/test)
add_subdirectory(app/demos)
add_subdirectory(app/CombinedAPI/library)