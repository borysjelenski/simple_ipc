cmake_minimum_required(VERSION 3.12)
project(recruitment_task LANGUAGES C CXX ASM)

find_library(cairo_lib NAMES cairo)
if(NOT cairo_lib)
  message(FATAL_ERROR "Cairo not found") 
endif()

add_subdirectory(common)

add_executable(program1_exec
  program1/src/cmd_parser.cpp
  program1/src/main.cpp
)
target_include_directories(program1_exec PRIVATE
  program1/include
)
target_link_libraries(program1_exec PRIVATE common_lib)

add_executable(program2_exec
  program2/src/draw_msg_visitor.cpp
  program2/src/main.cpp
)
target_include_directories(program2_exec PRIVATE
  program2/include
)
target_link_libraries(program2_exec PRIVATE common_lib ${cairo_lib})
