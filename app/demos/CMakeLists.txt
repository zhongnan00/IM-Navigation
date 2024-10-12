add_executable(ReadSTL ReadSTL.cpp)
target_link_libraries(ReadSTL PRIVATE ${VTK_LIBRARIES})

vtk_module_autoinit(
  TARGETS ReadSTL
  MODULES ${VTK_LIBRARIES}
)