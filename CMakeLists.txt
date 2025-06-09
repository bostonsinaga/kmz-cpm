cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(geoton VERSION 1.1.4)

message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Major version: ${PROJECT_VERSION_MAJOR}")
message(STATUS "Minor version: ${PROJECT_VERSION_MINOR}")
message(STATUS "Patch version: ${PROJECT_VERSION_PATCH}")

find_package( GeographicLib REQUIRED )
include_directories( ${GeographicLib_INCLUDE_DIRS} )

if (NOT MSVC)
  set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
endif()

add_subdirectory(wrappers)
add_subdirectory(tools)

add_executable( ${PROJECT_NAME}
  wrappers/call-briefer.cpp
  main.cpp
)

target_link_libraries( ${PROJECT_NAME}
  tools wrappers
  ${GeographicLib_LIBRARIES}
)

if (MSVC)
  get_target_property( _LIBTYPE ${GeographicLib_LIBRARIES} TYPE )

  if (_LIBTYPE STREQUAL "SHARED_LIBRARY")

    # On Windows systems, copy the shared library to build directory
    add_custom_command (TARGET ${PROJECT_NAME} POST_BUILD

      COMMAND ${CMAKE_COMMAND} -E
      copy $<TARGET_FILE:${GeographicLib_LIBRARIES}> ${CMAKE_CFG_INTDIR}
      COMMENT "Copying shared library for GeographicLib")
  endif()
endif()
