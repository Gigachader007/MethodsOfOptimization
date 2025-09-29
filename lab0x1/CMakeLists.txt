project(lab0x1)

# Первая лабораторная работа на Simplex Method

file(GLOB_RECURSE SRC "src/*.cpp")
include_directories(include)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME})