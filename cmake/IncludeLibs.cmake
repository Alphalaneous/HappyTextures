cmake_minimum_required(VERSION 3.21)
add_library(third_party INTERFACE)

CPMAddPackage("gh:EclipseMenu/rift#b8b31d6")

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    target_compile_definitions(rift PRIVATE _HAS_ITERATOR_DEBUGGING=0)
endif()

target_link_libraries(third_party INTERFACE
    rift
)