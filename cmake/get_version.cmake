# Version Management via Git Tags
#
# This module provides functions to extract project version from git tags.
# Version format: vX.Y.Z (e.g., v1.0.0)
#
# Usage:
#   include(cmake/get_version.cmake)
#   xc_get_project_version(PROJECT_VERSION)
#
# Public API:
#   xc_get_last_git_tag(TAG_VAR DEFAULT_VALUE) - Get the most recent git tag
#   xc_get_project_version(VERSION_VAR)        - Get version from latest tag

find_program(GII NAMES git)

# xc_get_last_git_tag(TAG_VAR DEFAULT_VALUE)
# Get the most recent git tag
#
# Args:
#   TAG_VAR     - Variable to store the tag
#   DEFAULT_VALUE - Default value if no tag found
#
# Examples:
#   xc_get_last_git_tag(TAG "v0.0.0")
function(xc_get_last_git_tag TAG_VAR DEFAULT_VALUE)
    if(NOT GII)
        message(FATAL_ERROR "git not found")
    endif()

    set(${TAG_VAR} ${DEFAULT_VALUE} PARENT_SCOPE)

    execute_process(
        COMMAND ${GII} describe --abbrev=0 --tags
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(GIT_TAG)
        message(STATUS "GIT_TAG: ${GIT_TAG}")
        set(${TAG_VAR} ${GIT_TAG} PARENT_SCOPE)
    endif()
endfunction(xc_get_last_git_tag)

# xc_get_project_version(VERSION_VAR)
# Get project version from latest git tag
#
# Extracts version in format X.Y.Z from tag (e.g., v1.2.3 -> 1.2.3)
# Defaults to 0.0.0 if no valid tag found
#
# Args:
#   VERSION_VAR - Variable to store the version
#
# Examples:
#   xc_get_project_version(PROJECT_VERSION)
function(xc_get_project_version VERSION_VAR)
    xc_get_last_git_tag(GIT_TAG "v0.0.0")
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" VERSION ${GIT_TAG})
    if(NOT VERSION)
        message(FATAL_ERROR "git tag ${GIT_TAG} not match version format")
    endif()
    set(${VERSION_VAR} ${VERSION} PARENT_SCOPE)
    message(STATUS "VERSION: ${VERSION}")
endfunction(xc_get_project_version)