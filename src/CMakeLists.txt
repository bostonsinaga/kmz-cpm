set (SOURCES
  xml/node.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
  mini-tools cli-menu
)
