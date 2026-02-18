


add_executable(oop_example oop.cc)
target_link_libraries(oop_example PRIVATE xcmixin)
add_executable(overload_example overload-msvc-bug.cc)
target_link_libraries(overload_example PRIVATE xcmixin)
