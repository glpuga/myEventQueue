cmake_minimum_required(VERSION 3.10.2)

set(PROJECT_TEST_SRCS runner.cpp test_cases.cpp)

enable_testing()

add_executable(${PROJECT_NAME}_test ${PROJECT_TEST_SRCS})

target_link_libraries(${PROJECT_NAME}_test gtest gmock gtest_main
                      ${PROJECT_NAME})

set_target_properties(
  ${PROJECT_NAME}_test
  PROPERTIES
    CXX_CLANG_TIDY
    "clang-tidy;-header-filter=.;-checks=*,-fuchsia-overloaded-operator,-fuchsia-default-arguments,-cppcoreguidelines-owning-memory,-readability-else-after-*,-cert-err58-cpp,-llvm-header-guard"
    CXX_CPPCHECK
    "cppcheck;--language=c++;--std=c++14;--enable=warning,style,performance,portability;--library=${PROJECT_SOURCE_DIR}/test/googletest.cfg"
    CXX_CPPLINT "cpplint;--linelength=120;--filter=+,-build/c++11")

add_test(${PROJECT_NAME}_test
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}_test)
