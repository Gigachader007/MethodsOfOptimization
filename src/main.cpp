#include "SimplexTable.hpp"
#include "Window/Window.hpp"

#include <glad/gl.h>

//   Si0    x1   x2
//x3  2     1    -2
//x4  -2    -2    1
//x5  5     1     1
// F  0     1    -1

int main() {
    /*
    SimplexTable table(Matrix(4,3, 
        {
            2,  1,  -2,
            -2, -2,  1,
            5,   1,  1,
            0,   1,  -1
        }
    ));
    auto& mat = table.get_matrix();

    while(!table.is_done()){
        auto solve_col = table.find_solving_col();
        if(!solve_col.has_value()){
            break;
        }
        auto unwraped_solve_col = solve_col.value();
        auto solve_row = table.find_solving_row(unwraped_solve_col);
        if(!solve_row.has_value()){
            break;
        }
        auto unwraped_solve_row = solve_row.value();

        table.recalculate_matrix(unwraped_solve_row, unwraped_solve_col);
    }
    */

    if(Window::init(1920, 1080, "SimplexMethodLab")){
        while(!Window::isShouldClose()){
            Window::PollEvents();

            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            

            Window::SwapBuffers();
        }
    }
    Window::term();
}