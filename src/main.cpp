#include "SimplexTable.hpp"
#include "Window/Window.hpp"

#include <glad/gl.h>
#include "imgui.h"
#include <map>

int main()
{

    struct TableStory{
        SimplexTable table;
        std::map<uint32_t, std::string> cols_names, rows_names;
        uint32_t selected_row, selected_col;
    };

    struct
    {
        Matrix cond_table = Matrix(0, 0);
        Matrix cond_vals_table = Matrix(0, 0);
        std::vector<int> symbol_items;
        std::vector<float> function_vals;
        uint32_t num_of_vals_in_F = 0;
        uint32_t num_of_cond = 0;
        uint32_t old_num_of_vals_in_F = 0;
        uint32_t old_num_of_cond = 0;
        int min_max_checker = 0;
        SimplexTable table = SimplexTable(Matrix(0, 0));
        std::map<uint32_t, std::string> cols_names, rows_names;
        
        std::vector<TableStory> table_story;
        ImVec4 color = ImVec4(0, 0.588235319, 1, 1);
    } first_task;

    struct {

    } second_task;
    
    first_task.function_vals.resize(first_task.old_num_of_vals_in_F + 1);

    if (Window::init(1920, 1080, "SimplexMethodLab"))
    {
        while (!Window::isShouldClose())
        {
            Window::PollEvents();

            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Settings"))
                {
                    if (ImGui::BeginMenu("Theme"))
                    {
                        if (ImGui::MenuItem("Dark"))
                        {
                            ImGui::StyleColorsDark();
                        }
                        if (ImGui::MenuItem("Light"))
                        {
                            ImGui::StyleColorsLight();
                        }
                        if (ImGui::MenuItem("Classic"))
                        {
                            ImGui::StyleColorsClassic();
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            if (ImGui::Begin("Simplex table solver"))
            {
                ImGui::InputScalar("Number of variables in F (without constant)", ImGuiDataType_U32, &first_task.num_of_vals_in_F); // cols
                ImGui::InputScalar("Number of conditions", ImGuiDataType_U32, &first_task.num_of_cond);                             // rows

                if (ImGui::Button("Bake condition table"))
                {
                    first_task.cond_table.resize(first_task.num_of_cond, first_task.num_of_vals_in_F);
                    first_task.cond_vals_table.resize(first_task.num_of_cond, 1);
                    first_task.symbol_items.resize(first_task.num_of_cond);
                    first_task.function_vals.resize(first_task.num_of_vals_in_F + 1);
                    first_task.old_num_of_cond = first_task.num_of_cond;
                    first_task.old_num_of_vals_in_F = first_task.num_of_vals_in_F;
                }

                ImGui::Text("Condition table");
                if (ImGui::BeginTable("Condition table", first_task.old_num_of_vals_in_F + 2, ImGuiTableFlags_Borders))
                {
                    for (auto i = 0; i < first_task.old_num_of_vals_in_F; ++i)
                    {
                        ImGui::TableSetupColumn(("x" + std::to_string(i + 1)).c_str(), ImGuiTableColumnFlags_WidthStretch);
                    }
                    ImGui::TableSetupColumn("condition", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("C", ImGuiTableColumnFlags_WidthStretch);

                    ImGui::TableHeadersRow();

                    for (auto row = 0; row < first_task.old_num_of_cond; ++row)
                    {
                        ImGui::TableNextRow();
                        for (auto col = 0; col < first_task.old_num_of_vals_in_F; ++col)
                        {
                            ImGui::TableNextColumn();
                            ImGui::InputFloat(("##cond" + std::to_string(row) + "x" + std::to_string(col)).c_str(), &first_task.cond_table.at(row, col));
                        }
                        ImGui::TableNextColumn();
                        static char *symbols[] = {"<=", ">=", "="};
                        ImGui::Combo(("##combo" + std::to_string(row)).c_str(), &first_task.symbol_items[row], symbols, IM_ARRAYSIZE(symbols));

                        ImGui::TableNextColumn();
                        ImGui::InputFloat(("##vals" + std::to_string(row)).c_str(), &first_task.cond_vals_table.at(row, 0));
                    }
                    ImGui::EndTable();
                }
                ImGui::Text("Function");
                if (ImGui::BeginTable("Function", first_task.old_num_of_vals_in_F + 1, ImGuiTableFlags_Borders))
                {
                    for (auto i = 0; i < first_task.old_num_of_vals_in_F; ++i)
                    {
                        ImGui::TableSetupColumn(("x" + std::to_string(i + 1)).c_str(), ImGuiTableColumnFlags_WidthStretch);
                    }
                    ImGui::TableSetupColumn("C", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();

                    ImGui::TableNextRow();
                    for (auto i = 0; i < first_task.old_num_of_vals_in_F + 1; ++i)
                    {
                        ImGui::TableNextColumn();
                        ImGui::InputFloat(("##func" + std::to_string(i)).c_str(), &first_task.function_vals[i]);
                    }

                    ImGui::EndTable();
                }
                static char *variants[] = {"min", "max"};
                ImGui::Combo("<- F", &first_task.min_max_checker, variants, IM_ARRAYSIZE(variants));
                ImGui::ColorEdit4("Selected col/row color", (float*)&first_task.color);
                if (ImGui::Button("Solve"))
                {
                    first_task.table_story.clear();
                    first_task.cols_names.clear();
                    first_task.rows_names.clear();
                    //   Si0    x1   x2
                    // x3  2     1    -2
                    // x4  -2    -2    1
                    // x5  5     1     1
                    // F  0     1    -1
                    uint32_t simplex_table_cols = first_task.old_num_of_vals_in_F + 1; // +1 for constant
                    uint32_t simplex_table_rows = first_task.old_num_of_cond + 1;      // +1 for F
                    for (const auto item : first_task.symbol_items)
                    {
                        if (item == 2)
                            simplex_table_rows++; // if '=' in condition, we split = into <= and >=
                    }
                    first_task.cols_names.insert({0, "Si0"});
                    for(auto i = 0; i < first_task.old_num_of_vals_in_F; ++i){
                        first_task.cols_names.insert({i+1, "x"+std::to_string(i+1)});
                    }
                    for(auto i = 0; i < simplex_table_rows - 1; ++i){
                        first_task.rows_names.insert({i, "x"+std::to_string(first_task.old_num_of_vals_in_F+i+1)});
                    }
                    first_task.rows_names.insert({simplex_table_rows - 1, "F"});

                    first_task.table = SimplexTable(Matrix(simplex_table_rows, simplex_table_cols));
                    auto &mat = first_task.table.get_matrix();
                    uint32_t counter = 0;
                    
                    auto k = first_task.min_max_checker == 0 ? 1 : -1;
                    mat.at(mat.rows_num() - 1, 0) = k * first_task.function_vals[first_task.function_vals.size() - 1];
                    for (auto col = 1; col < mat.cols_num(); ++col)
                    {
                        mat.at(mat.rows_num() - 1, col) = (-1.f) * k * first_task.function_vals[col - 1];
                    }
                    for(auto row = 0; row < mat.rows_num() - 1; ++row){
                        if(first_task.symbol_items[row - counter] != 2){
                            auto k = first_task.symbol_items[row - counter] == 0 ? 1.f : -1.f;
                            mat.at(row, 0) = k*first_task.cond_vals_table.at(row - counter, 0);
                            for(auto col = 0; col < first_task.cond_table.cols_num(); ++col){
                                mat.at(row, col + 1) = k*first_task.cond_table.at(row - counter, col);
                            }
                        }
                        else{
                            mat.at(row, 0) = first_task.cond_vals_table.at(row - counter, 0);
                            mat.at(row+1, 0) = -first_task.cond_vals_table.at(row - counter, 0);
                            for(auto col = 0; col < first_task.cond_table.cols_num(); ++col){
                                mat.at(row, col + 1) = first_task.cond_table.at(row - counter, col);
                                mat.at(row+1, col + 1) = -first_task.cond_table.at(row - counter, col);
                            }
                            ++row;
                            ++counter;
                        }
                    }

                    while (!first_task.table.is_done())
                    {
                        TableStory story{
                            .table = first_task.table,
                            .cols_names = first_task.cols_names,
                            .rows_names = first_task.rows_names,
                        };
                        auto solve_col = first_task.table.find_solving_col();
                        if (!solve_col.has_value())
                        {
                            break;
                        }
                        auto unwraped_solve_col = solve_col.value();
                        story.selected_col = unwraped_solve_col;
                        auto solve_row = first_task.table.find_solving_row(unwraped_solve_col);
                        if (!solve_row.has_value())
                        {
                            break;
                        }
                        auto unwraped_solve_row = solve_row.value();
                        story.selected_row = unwraped_solve_row;

                        first_task.table_story.push_back(story);

                        std::swap(first_task.cols_names[unwraped_solve_col], first_task.rows_names[unwraped_solve_row]);

                        first_task.table.recalculate_matrix(unwraped_solve_row, unwraped_solve_col);
                    }
                }
                uint32_t step = 0;
                for(const auto& story : first_task.table_story){
                    ImGui::Text("Simplex table: step %i", step++);
                    const auto& mat = story.table.get_matrix();
                    if(ImGui::BeginTable(("Simplex table: step " + std::to_string(step)).c_str(), mat.cols_num() + 1, ImGuiTableFlags_Borders)){
                        ImGui::TableSetupColumn("");
                        for(auto col = 0; col < mat.cols_num(); ++col){
                            ImGui::TableSetupColumn(story.cols_names.at(col).c_str());
                        }
                        ImGui::TableHeadersRow();
                        for(auto row = 0; row < mat.rows_num(); ++row){
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            if(row == story.selected_row){
                                ImGui::TextColored(first_task.color, "%s", story.rows_names.at(row).c_str());
                            }
                            else{
                                ImGui::Text("%s",story.rows_names.at(row).c_str());
                            }
                            for(auto col = 0; col < mat.cols_num(); ++col){
                                ImGui::TableNextColumn();
                                if(col == story.selected_col || row == story.selected_row){
                                    ImGui::TextColored(first_task.color, "%f", mat.at(row, col));
                                }
                                else{
                                    ImGui::Text("%f", mat.at(row, col));
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                }
                ImGui::Text("Result Simplex table");
                auto& mat = first_task.table.get_matrix();
                if(mat.cols_num() && ImGui::BeginTable("Result Simplex Table", mat.cols_num() + 1, ImGuiTableFlags_Borders)){
                    ImGui::TableSetupColumn("");
                    for(auto col = 0; col < mat.cols_num(); ++col){
                        ImGui::TableSetupColumn(first_task.cols_names[col].c_str());
                    }
                    ImGui::TableHeadersRow();
                    for(auto row = 0; row < mat.rows_num(); ++row){
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%s",first_task.rows_names[row].c_str());
                        for(auto col = 0; col < mat.cols_num(); ++col){
                            ImGui::TableNextColumn();
                            ImGui::Text("%f", mat.at(row, col));
                        }
                    }
                    ImGui::EndTable();
                }
                if(mat.rows_num()) ImGui::Text("%s value of F = %f", (first_task.min_max_checker == 0 ? "min" : "max"), mat.at(mat.rows_num()-1, 0) * (first_task.min_max_checker == 0 ? 1.f : -1.f));
            }
            ImGui::End();
            if(ImGui::Begin("Direct task to Dual task")){
                ImGui::TextColored(first_task.color, "Using table from 1 task to convert Direct task to Dual task");
                if(ImGui::Button("Convert")){
                    if(first_task.cond_table.cols_num() != first_task.cond_table.rows_num()) {
                        auto max_size = std::max(first_task.cond_table.cols_num(), first_task.cond_table.rows_num());
                        auto new_cond_table = Matrix(max_size, max_size);
                        for(auto i = 0; i < first_task.cond_table.rows_num(); ++i){
                            for(auto j = 0; j < first_task.cond_table.cols_num(); ++j){
                                new_cond_table.at(i, j) = first_task.cond_table.at(i, j);
                            }
                        }
                        first_task.cond_table = new_cond_table;
                        first_task.cond_vals_table.resize(max_size, 1);
                        first_task.function_vals.resize(max_size + 1);
                        first_task.symbol_items.resize(max_size);

                        first_task.num_of_cond = first_task.num_of_vals_in_F = first_task.old_num_of_cond = first_task.old_num_of_vals_in_F = max_size;
                    }

                    auto new_cond_vals = Matrix(first_task.function_vals.size() - 1, 1);
                    for(auto i = 0; i < new_cond_vals.rows_num(); ++i){
                        new_cond_vals.at(i, 0) = first_task.function_vals.at(i);
                    }
                    std::vector<float> new_function_vals(first_task.cond_vals_table.rows_num() + 1);
                    for(auto i = 0; i < first_task.cond_vals_table.rows_num(); ++i){
                        new_function_vals.at(i) = first_task.cond_vals_table.at(i, 0);
                    }
                    new_function_vals.at(first_task.cond_vals_table.rows_num()) = first_task.function_vals.at(first_task.function_vals.size() - 1);

                    std::swap(first_task.num_of_cond, first_task.num_of_vals_in_F);
                    std::swap(first_task.old_num_of_cond, first_task.old_num_of_vals_in_F);

                    first_task.function_vals = new_function_vals;
                    first_task.cond_vals_table = new_cond_vals;

                    first_task.min_max_checker = 1 - first_task.min_max_checker;

                    for(auto& symbol : first_task.symbol_items){
                        if(symbol == 2) continue;
                        symbol = 1 - symbol;
                    }

                    auto new_cond_table = Matrix(first_task.cond_table.cols_num(), first_task.cond_table.rows_num());
                    for(auto i = 0; i < first_task.cond_table.cols_num(); ++i){
                        for(auto j = 0; j < first_task.cond_table.rows_num(); ++j){
                            new_cond_table.at(i, j) = first_task.cond_table.at(j, i);
                        }
                    }
                    first_task.cond_table = new_cond_table;
                }
            }
            ImGui::End();
            Window::SwapBuffers();
        }
    }
    Window::term();
}