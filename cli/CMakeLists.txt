add_executable(kmzcpm main.cpp)

target_link_libraries(kmzcpm PRIVATE
  mini-tools
  cli-menu
  ${PROJECT_NAME}
)
