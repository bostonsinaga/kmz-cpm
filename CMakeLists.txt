cmake_minimum_required( VERSION 3.20 )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(kmz-cpm VERSION 2.0.0)

message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Major version: ${PROJECT_VERSION_MAJOR}")
message(STATUS "Minor version: ${PROJECT_VERSION_MINOR}")
message(STATUS "Patch version: ${PROJECT_VERSION_PATCH}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(MINIZIP REQUIRED minizip IMPORTED_TARGET)
pkg_check_modules(GEOGRAPHICLIB REQUIRED geographiclib IMPORTED_TARGET)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cli-menu)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cli)

target_link_libraries(${PROJECT_NAME} PRIVATE
  PkgConfig::MINIZIP
  PkgConfig::GEOGRAPHICLIB
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cli-menu/includes)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
