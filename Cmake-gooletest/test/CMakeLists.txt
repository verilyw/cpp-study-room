include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY git@github.com:google/googletest.git
    GIT_TAG main
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(math_test math_test.cpp)
target_link_libraries(math_test math GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(math_test)