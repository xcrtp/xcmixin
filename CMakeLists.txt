cmake_minimum_required(VERSION 3.10..4.1)

project(xcmixin LANGUAGES CXX VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(GNUInstallDirs)

add_library(xcmixin INTERFACE)
target_include_directories( xcmixin INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_subdirectory(examples)

include(CMakePackageConfigHelpers)

install(
    TARGETS xcmixin
    EXPORT xcmixin
)
install(
    EXPORT xcmixin
    NAMESPACE xcmixin::
    FILE xcmixinConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/xcmixin/
)

install(
    DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/xcmixin/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xcmixin/
    FILES_MATCHING
    PATTERN "*.hpp"
)

