cmake_minimum_required(VERSION 3.10..4.1)
include(cmake/get_version.cmake)
xc_get_project_version(PROJECT_VERSION)
project(xcmixin LANGUAGES CXX VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(GNUInstallDirs)
enable_testing()
option(XCMIXIN_BUILD_EXAMPLES "Build xcmixin examples" OFF)
option(XCMIXIN_BUILD_TESTS "Build xcmixin tests" ON)
add_library(xcmixin INTERFACE)
target_include_directories( xcmixin INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
add_library(xcmixin::xcmixin ALIAS xcmixin)
if(XCMIXIN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(XCMIXIN_BUILD_TESTS)
    add_subdirectory(tests)
endif()

include(CMakePackageConfigHelpers)

install(
    TARGETS xcmixin
    EXPORT xcmixin
)
install(
    EXPORT xcmixin
    NAMESPACE xcmixin::
    FILE xcmixinConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xcmixin/
)

install(
    DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/xcmixin/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xcmixin/
    FILES_MATCHING
    PATTERN "*.hpp"
)

