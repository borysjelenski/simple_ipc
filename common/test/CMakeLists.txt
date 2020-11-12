
cmake_minimum_required(VERSION 3.12)
project(recruitment_task_common_lib_test LANGUAGES C CXX ASM)

include(FetchContent)
FetchContent_Declare(
  cpputest
  GIT_REPOSITORY https://github.com/cpputest/cpputest.git
)
FetchContent_GetProperties(cpputest)
if(NOT cpputest_POPULATED)
  message(STATUS "Populating CppUTest...")
  FetchContent_Populate(cpputest)
endif()

set(CppUTestRootDirectory ${cpputest_SOURCE_DIR})
add_subdirectory(${CppUTestRootDirectory}/src/CppUTest)
add_subdirectory(${CppUTestRootDirectory}/src/CppUTestExt)
target_sources(CppUTest PRIVATE ${CppUTestRootDirectory}/src/Platforms/Gcc/UtestPlatform.cpp)
target_sources(CppUTestExt PRIVATE ${CppUTestRootDirectory}/src/Platforms/Gcc/UtestPlatform.cpp)

add_executable(common_lib_test
  ../src/message.cpp
  src/sample_test.cpp
  src/message_serialize_test.cpp
  src/message_deserialize_test.cpp
  src/main.cpp
)
target_include_directories(common_lib_test PRIVATE ../include)
target_link_libraries(common_lib_test PRIVATE CppUTest CppUTestExt)
