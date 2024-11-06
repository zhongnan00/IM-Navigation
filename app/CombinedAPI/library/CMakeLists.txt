# build the CombinedAPI code of the library
include_directories(include src/include)
file (GLOB SOURCES "src/*.cpp")

add_library(CombinedAPI STATIC ${SOURCES})

