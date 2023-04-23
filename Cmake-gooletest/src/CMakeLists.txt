add_library(math math.cpp)
target_include_directories(math PUBLIC .)

add_executable(math-lib main.cpp)
target_link_libraries(math-lib math)
