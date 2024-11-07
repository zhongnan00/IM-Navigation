cmake_minimum_required(VERSION 3.10)
project(IM-Navigation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_INCLUDE_CURRENT_DIR ON)   
set(CMAKE_POSITION_INDEPENDENT_CODE ON)    
set(CMAKE_BUILD_TYPE debug)


include_directories(.)


# if(BUILD_UI) 
#     message("Building ui")
#     set(CMAKE_AUTOUIC ON)
#     set(CMAKE_AUTOMOC ON)
#     set(CMAKE_AUTORCC ON)

    if(BUILD_SYSTEM_NAME STREQUAL "aarch64")
      message("Build system = aarch64")
      # list(APPEND CMAKE_PREFIX_PATH "/opt/qt-5.15.7-aarch64/lib/cmake")
      include_directories(/opt/Qt5.15.17-aarch64/include)
      link_directories(/opt/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/lib)
      add_subdirectory(app/test)
      # add_subdirectory(app/demos)
      add_subdirectory(app/CombinedAPI/library)
      add_subdirectory(app/service)

    endif()
    if(BUILD_SYSTEM_NAME STREQUAL "ubuntu")
    message("Build system = ubuntu")
    find_package(VTK REQUIRED)
    if (NOT VTK_FOUND)
      message(FATAL_ERROR "Unable to find the VTK build folder.")
    endif()
        # list(APPEND CMAKE_PREFIX_PATH "/opt/qt-5.15.7/lib/cmake")
        # include_directories(/opt/qt-5.15.7/include)
        # find_package(QT NAMES  Qt5 COMPONENTS Widgets Charts Sql PrintSupport REQUIRED)
      add_subdirectory(app/test)
      add_subdirectory(app/demos)
      add_subdirectory(app/CombinedAPI/library)
      add_subdirectory(app/service)
    endif()

#     find_package(Qt5 REQUIRED COMPONENTS Widgets Charts Sql PrintSupport  REQUIRED)
#     message("find qt")
#     add_subdirectory(third_party/qcustomplot)


# endif()

