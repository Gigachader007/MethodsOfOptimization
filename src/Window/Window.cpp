#include "Window/Window.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

GLFWwindow* win = nullptr;

static void resize_frame_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

bool Window::init(const uint32_t width, const uint32_t height, const std::string& title)
{
    if(win) return false;

    if(!glfwInit()){
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    win = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if(!win) return false;
    glfwMakeContextCurrent(win);

    glfwSetFramebufferSizeCallback(win, resize_frame_callback);

    if(!gladLoadGL(glfwGetProcAddress)){
        return false;
    }

    return true;
}

bool Window::isShouldClose() {
    if(!win) return true;
    return glfwWindowShouldClose(win);
}
void Window::PollEvents() {
    glfwPollEvents();
}
void Window::SwapBuffers() {
    glfwSwapBuffers(win);
}
void Window::term()
{
    glfwTerminate();
}
