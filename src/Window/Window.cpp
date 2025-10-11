#include "Window/Window.hpp"

#include <glad/gl.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    return true;
}



bool Window::isShouldClose() {
    if(!win) return true;
    return glfwWindowShouldClose(win);
}
void Window::PollEvents() {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void Window::SwapBuffers() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(win);
}
void Window::term()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}
