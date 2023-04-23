include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG "main"
)
FetchContent_MakeAvailable(googletest)

add_executable(stress_test stress_test.cpp)

enable_testing()
add_executable(unit_test skiplist_test.cpp)

TARGET_LINK_LIBRARIES(unit_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_test)