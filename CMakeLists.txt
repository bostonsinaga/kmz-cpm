cmake_minimum_required( VERSION 3.20 )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(kmz-cpm VERSION 2.0.0)

message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Major version: ${PROJECT_VERSION_MAJOR}")
message(STATUS "Minor version: ${PROJECT_VERSION_MINOR}")
message(STATUS "Patch version: ${PROJECT_VERSION_PATCH}")

find_package(Boost REQUIRED)
find_package(GeographicLib REQUIRED)

# find all required 'LIBKML'
find_library(KMLBASE_LIBRARY NAMES kmlbase PATHS "/ucrt64/lib")
find_library(KMLDOM_LIBRARY NAMES kmldom PATHS "/ucrt64/lib")
find_library(KMLENGINE_LIBRARY NAMES kmlengine PATHS "/ucrt64/lib")
find_library(KMLCONVENIENCE_LIBRARY NAMES kmlconvenience PATHS "/ucrt64/lib")

set(LIBKML_LIBRARIES
  ${KMLBASE_LIBRARY}
  ${KMLDOM_LIBRARY}
  ${KMLENGINE_LIBRARY}
  ${KMLCONVENIENCE_LIBRARY}
)

# for non-MSVC (Linux/macOS) builds
if (NOT MSVC)
  set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cli-menu)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cli)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${Boost_LIBRARIES}
  ${GeographicLib_LIBRARIES}
  ${LIBKML_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cli-menu/includes)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# only for 'GeographicLib'
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

