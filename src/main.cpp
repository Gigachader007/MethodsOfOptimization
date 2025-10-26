#include "Window/Window.hpp"
#include "SimplexTable.hpp"
#include <glad/gl.h>
#include "imgui.h"
#include <map>

int main()
{
    std::vector<std::vector<float>> A = {
        {1, -2},
        {-2, 1},
        {1, 1}
    };
    std::vector<float> b = {
        2,
        -2,
        5
    };
    std::vector<float> c = {
        1, -1
    };

    auto table = SimplexTable(A, b, c);
    table.print();
    while(true){
        auto coords = table.phase1();
        if(coords == std::pair(-1, -1)){
            break;
        }
        table.recalculate_matrix(coords);
        table.print();
    }
    while(true){
        auto coords = table.phase2();
        if(coords == std::pair(-1, -1)){
            break;
        }
        table.recalculate_matrix(coords);
        table.print();
    }
    /*
    table.print();
    auto coords = table.phase1();
    table.recalculate_matrix(coords);
    std::cout << std::endl;
    table.print();
    coords = table.phase2();
    table.recalculate_matrix(coords);
    std::cout << std::endl;
    table.print();
    coords = table.phase2();
    table.recalculate_matrix(coords);
    std::cout << std::endl;
    table.print();
    */
    

    if (Window::init(1920, 1080, "SimplexMethodLab"))
    {
        while (!Window::isShouldClose())
        {
            Window::PollEvents();

            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            if(ImGui::Begin("Simplex Table Solver")){

            }
            ImGui::End();
            
            Window::SwapBuffers();
        }
    }
    Window::term();
}