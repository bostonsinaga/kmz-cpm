set (SOURCES
  xml/node.cpp
  xml/derivers.cpp
  xml/map.cpp
  xml/parser.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
  mini-tools cli-menu
)
