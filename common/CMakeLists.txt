cmake_minimum_required(VERSION 3.12)
project(recruitment_task_common_lib LANGUAGES C CXX ASM)

add_library(common_lib OBJECT
  src/utils.cpp
  src/message.cpp
)
target_include_directories(common_lib PUBLIC
  include
)
target_compile_features(common_lib PUBLIC cxx_std_17)
