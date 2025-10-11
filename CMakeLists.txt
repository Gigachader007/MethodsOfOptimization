cmake_minimum_required(VERSION 3.31)
project(MethodsOfOptimization)

include(FetchContent)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  cmake
)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw glm glad)

glad_add_library(glad_gl_core_46 STATIC REPRODUCIBLE LOADER API gl:core=4.6)

file(GLOB IMGUI_SRC "ex/imgui/*.cpp" "ex/imgui/backends/imgui_impl_glfw.cpp" "ex/imgui/backends/imgui_impl_opengl3.cpp")

file(GLOB_RECURSE SRC "src/*.cpp")
include_directories(include ex/imgui ex/imgui/backends)

add_executable(${PROJECT_NAME} ${SRC} ${IMGUI_SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad_gl_core_46 glm::glm)