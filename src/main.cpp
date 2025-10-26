#include "Window/Window.hpp"
#include "SimplexTable.hpp"
#include <glad/gl.h>
#include "imgui.h"
#include <map>
#include <algorithm>

int main()
{
    struct SimplexTableHistory{
        std::map<int, std::string> cols_name{}, rows_name{};
        SimplexTable table;
        std::pair<int, int> selected_coords;
    };
    struct
    {
        std::vector<std::vector<float>> A = {};
        std::vector<float> b = {};
        std::vector<float> c = {};
        std::map<int, std::string> cols_name{}, rows_name{};
        SimplexTable table;
        std::vector<SimplexTableHistory> table_story{};
        bool min_max_checker = true;
        int condition = 0;
    } first_task;
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

            if (ImGui::Begin("Simplex Table Solver"))
            {
                static int mat_size[2] = {1, 1};
                static ImVec4 color = ImVec4(0.235294118523597717F, 1.f, 0.0f, 1.f);
                ImGui::ColorEdit4("Cols/Rows color", (float*)&color);
                ImGui::SliderInt2("Rows and Cols nums", mat_size, 1, 10);
                if (ImGui::Button("Bake tables"))
                {

                    first_task.c = std::vector<float>(mat_size[1]);

                    first_task.A = std::vector<std::vector<float>>(mat_size[0], std::vector<float>(mat_size[1]));
                    first_task.b = std::vector<float>(mat_size[0]);

                }
                ImGui::Text("Condition table");
                if (!first_task.A.empty() && ImGui::BeginTable("Condition table", first_task.A.front().size() + 2, ImGuiTableFlags_Borders))
                {
                    for (auto i = 0; i < first_task.A.front().size(); ++i)
                    {
                        ImGui::TableSetupColumn(("x" + std::to_string(i + 1)).c_str());
                    }
                    ImGui::TableSetupColumn("condition");
                    ImGui::TableSetupColumn("B");

                    ImGui::TableHeadersRow();

                    for (auto i = 0; i < first_task.A.size(); ++i)
                    {
                        ImGui::TableNextRow();
                        for (auto j = 0; j < first_task.A[i].size(); ++j)
                        {
                            ImGui::TableNextColumn();
                            ImGui::InputFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &first_task.A.at(i).at(j));
                        }
                        ImGui::TableNextColumn();
                        static const char* items[] = {"<=", ">="};
                        ImGui::Combo(("##condition" + std::to_string(i)).c_str(), &first_task.condition, items, IM_ARRAYSIZE(items));
                        ImGui::TableNextColumn();
                        ImGui::InputFloat(("##" + std::to_string(i)).c_str(), &first_task.b.at(i));
                    }
                    ImGui::EndTable();
                }
                ImGui::Text("Function table");
                ImGui::Selectable("min / max", &first_task.min_max_checker);
                if (!first_task.c.empty() && ImGui::BeginTable("Function table", first_task.c.size(), ImGuiTableFlags_Borders))
                {
                    for (auto i = 0; i < first_task.c.size(); ++i)
                    {
                        ImGui::TableSetupColumn(("x" + std::to_string(i + 1)).c_str());
                    }
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    for (auto i = 0; i < first_task.c.size(); ++i)
                    {
                        ImGui::TableNextColumn();
                        ImGui::InputFloat(("##func" + std::to_string(i)).c_str(), &first_task.c.at(i));
                    }
                    ImGui::EndTable();
                }
                if (ImGui::Button("Task to Dual Task")){
                    std::swap(first_task.b, first_task.c);
                    std::swap(mat_size[0], mat_size[1]);
                    first_task.condition = 1 - first_task.condition;
                    first_task.min_max_checker = !first_task.min_max_checker;

                    auto new_a_matrix = std::vector<std::vector<float>>(first_task.A.front().size(), std::vector<float>(first_task.A.size()));
                    for(auto i = 0; i < first_task.A.size(); ++i){
                        for(auto j = 0; j < first_task.A[i].size(); ++j){
                            new_a_matrix[j][i] = first_task.A[i][j];
                        }
                    }
                    first_task.A = new_a_matrix;
                }
                if (ImGui::Button("Calculate simplex table"))
                {
                    first_task.table_story.clear();
                    first_task.cols_name.clear();
                    first_task.rows_name.clear();

                    first_task.cols_name.insert({0, "Si0"});
                    first_task.rows_name.insert({mat_size[0], "F"});

                    for (auto j = 0; j < mat_size[1]; ++j)
                    {
                        first_task.cols_name.insert({j + 1, "x" + std::to_string(j + 1)});
                    }
                    for (auto i = 0; i < mat_size[0]; ++i)
                    {
                        first_task.rows_name.insert({i, "x" + std::to_string(i + 1 + mat_size[1])});
                    }
                    auto clone_c = first_task.c;
                    if(first_task.min_max_checker != 1){
                        for(auto& c : clone_c){
                            c *= -1.f;
                        }
                    }
                    auto clone_a = first_task.A;
                    auto clone_b = first_task.b;
                    if(first_task.condition != 0){
                        for(auto& i : clone_b){
                            i *= -1.f;
                        }
                        for(auto& i : clone_a){
                            for(auto& j : i){
                                j *= -1.f;
                            }
                        }
                    }

                    first_task.table = SimplexTable(clone_a, clone_b, clone_c);
                    while (true)
                    {
                        auto coords = first_task.table.phase1();
                        if (coords == std::pair(-1, -1))
                        {
                            break;
                        }

                        auto history = SimplexTableHistory{
                            .cols_name = first_task.cols_name,
                            .rows_name = first_task.rows_name,
                            
                            .table = first_task.table,
                            .selected_coords = coords
                        };

                        first_task.table_story.push_back(history);

                        std::swap(first_task.rows_name[coords.first], first_task.cols_name[coords.second]);
                        first_task.table.recalculate_matrix(coords);
                    }
                    while (true)
                    {
                        auto coords = first_task.table.phase2();
                        if (coords == std::pair(-1, -1))
                        {
                            break;
                        }

                        auto history = SimplexTableHistory{
                            .cols_name = first_task.cols_name,
                            .rows_name = first_task.rows_name,
                            
                            .table = first_task.table,
                            .selected_coords = coords
                        };

                        first_task.table_story.push_back(history);

                        std::swap(first_task.rows_name[coords.first], first_task.cols_name[coords.second]);
                        first_task.table.recalculate_matrix(coords);
                    }
                }
                if(!first_task.table_story.empty()){
                    size_t index = 0;
                    for(const auto& story : first_task.table_story){
                        ImGui::Text("%d step", ++index);
                        if(ImGui::BeginTable(("step" + std::to_string(index)).c_str(), story.table.col_count() + 1, ImGuiTableFlags_Borders)){
                            ImGui::TableSetupColumn("");
                            for(auto j = 0; j < story.table.col_count(); ++j){
                                ImGui::TableSetupColumn(story.cols_name.at(j).c_str());
                            }
                            ImGui::TableHeadersRow();

                            for(auto i = 0; i < story.table.row_count(); ++i){
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                if(i == story.selected_coords.first){
                                    ImGui::TextColored(color, story.rows_name.at(i).c_str());
                                }
                                else{
                                    ImGui::Text(story.rows_name.at(i).c_str());
                                }
                                for(auto j = 0; j < story.table.col_count(); ++j){
                                    ImGui::TableNextColumn();
                                    if(i == story.selected_coords.first || j == story.selected_coords.second){
                                        ImGui::TextColored(color, "%f", story.table.at(i,j));
                                    }
                                    else{
                                        ImGui::Text("%f", story.table.at(i,j));
                                    }
                                }
                            }

                            ImGui::EndTable();
                        }
                    }
                }
                ImGui::Text("Final Simplex Table");
                if(first_task.table.col_count() > 1 && first_task.table.row_count() > 1 && ImGui::BeginTable("Final Simplex Table", first_task.table.col_count() + 1, ImGuiTableFlags_Borders)){
                    ImGui::TableSetupColumn("");
                    for(auto j = 0; j < first_task.table.col_count(); ++j){
                        ImGui::TableSetupColumn(first_task.cols_name.at(j).c_str());
                    }
                    ImGui::TableHeadersRow();

                    for(auto i = 0; i < first_task.table.row_count(); ++i){
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text(first_task.rows_name.at(i).c_str());
                        for(auto j = 0; j < first_task.table.col_count(); ++j){
                            ImGui::TableNextColumn();
                            ImGui::Text("%f", first_task.table.at(i,j));
                        }
                    }

                    ImGui::EndTable();
                }
                if(first_task.table.col_count() > 1 && first_task.table.row_count() > 1){
                    ImGui::TextColored(color, "%s value of F = %f", (first_task.min_max_checker ? "Max" : "Min") , (first_task.min_max_checker == 1 ? -1.f : 1.f) * first_task.table.at(first_task.table.row_count() - 1, 0));
                    auto x_vals = std::vector<float>(first_task.c.size(), 0.f);
                    auto x_names = std::vector<std::string>(first_task.c.size());
                    for(auto i = 0; i < x_names.size(); ++i){
                        x_names[i] = "x" + std::to_string(i+1);
                    }
                    for(auto [row, val] : first_task.rows_name){
                        if(std::find(x_names.begin(), x_names.end(), val) != x_names.end()){
                            auto index = std::find(x_names.begin(), x_names.end(), val) - x_names.begin();
                            auto value = first_task.table.at(row, 0);
                            x_vals[index] = value;
                        }
                    }
                    std::string long_proof_string = "";
                    float res = 0.f;
                    for(auto i = 0; i < first_task.c.size(); ++i){
                        res += x_vals[i] * first_task.c[i];
                        long_proof_string += "(" + std::to_string(x_vals[i]) + ") * (" + std::to_string(first_task.c[i]) + ")";
                        if(i != first_task.c.size() - 1){
                            long_proof_string += " + ";
                        }
                    }
                    long_proof_string += " = " + std::to_string(res);

                    for(auto i = 0; i < first_task.c.size(); ++i){
                        ImGui::TextColored(color, (x_names[i] + " = " + std::to_string(x_vals[i])).c_str());
                    }

                    ImGui::TextColored(color, ("proof: " + (first_task.min_max_checker ? std::string("max") : std::string("min")) + " F = " + long_proof_string).c_str());

                    for(auto i = 0; i < first_task.A.size(); ++i){
                        std::string long_proof = "";
                        float res = 0.f;
                        for(auto j = 0; j < first_task.A[i].size(); ++j){
                            res += first_task.A[i][j] * x_vals[j];
                            long_proof += "(" + std::to_string(x_vals[j]) + ") * (" + std::to_string(first_task.A[i][j]) + ")";
                            if(j != first_task.A[i].size() - 1){
                                long_proof += " + ";
                            }
                        }
                        //long_proof += " = " + std::to_string(res);
                        long_proof += " <= " + std::to_string(first_task.b[i]);
                        /*
                        if(res <= first_task.b[i]) {
                            long_proof += " - True";
                        }
                        else{
                            long_proof += " - False";
                        }
                        */
                        long_proof += " - True";
                        ImGui::TextColored(color, (long_proof).c_str());
                    }
                }
            }
            ImGui::End();

            Window::SwapBuffers();
        }
    }
    Window::term();
}