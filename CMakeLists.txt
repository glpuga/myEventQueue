cmake_minimum_required(VERSION 3.10.2)
project(event_queue VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

include(CTest)

# ---------------

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")

# Prevent GoogleTest from overriding our compiler/linker options when building
# with Visual Studio
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds the following targets: gtest,
# gtest_main, gmock and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
                 "${CMAKE_BINARY_DIR}/googletest-build")

# The gtest/gmock targets carry header search path dependencies automatically
# when using CMake 2.8.11 or later. Otherwise we have to add them here
# ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include"
                      "${gmock_SOURCE_DIR}/include")
endif()

# Now simply link your own targets against gtest, gmock, etc. as appropriate

# ---------------

include(cmake/ccache.cmake)

include(cmake/doxygen.cmake)
enable_doxygen()

include(cmake/compiler_options.cmake)

# ---------------

add_library(${PROJECT_NAME} src/EventQueue.cpp)

set_target_compiler_options(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra
                                               -Wno-missing-field-initializers)

target_include_directories(${PROJECT_NAME} PUBLIC ./include)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    CXX_CLANG_TIDY
    "clang-tidy;-header-filter=.;-checks=*,-fuchsia-*,-google-*,-zircon-*,-abseil-*,-modernize-use-trailing-return-type,-llvm-*"
    CXX_CPPCHECK
    "cppcheck;--language=c++;--std=c++14;--enable=warning,style,performance,portability"
    CXX_CPPLINT "cpplint;--linelength=120;--filter=+,-build/c++11")

add_subdirectory("test")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
