#include "Window/Window.hpp"
#include "SimplexTable.hpp"
#include "SimplexTableRoutes.hpp"
#include <glad/gl.h>
#include "imgui.h"
#include <map>
#include <algorithm>
#include <memory>
#include <sstream>
#include <functional>


std::string to_string(float val){
    std::stringstream ss;
    ss.precision(3);
    ss << std::fixed << val;
    return std::move(ss).str();
}


int main()
{
    struct SimplexTableHistory
    {
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

    static std::shared_ptr<SimplexTableRoutes> table_routes = nullptr;

    if (Window::init(1920, 1080, "SimplexMethodLab"))
    {
        while (!Window::isShouldClose())
        {
            Window::PollEvents();

            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            static bool has_exception = false;
            static ImVec4 color = ImVec4(1.f, 113.f / 255.f, 0.0f, 1.f);

            if (ImGui::Begin("Simplex Table Solver"))
            {
                static int mat_size[2] = {1, 1};

                ImGui::ColorEdit4("Color", (float *)&color);
                ImGui::SliderInt2("Rows and Cols nums", mat_size, 1, 10);
                if (ImGui::Button("Bake tables"))
                {

                    // first_task.c = std::vector<float>(mat_size[1]);
                    first_task.c.resize(mat_size[1]);

                    // first_task.A = std::vector<std::vector<float>>(mat_size[0], std::vector<float>(mat_size[1]));
                    first_task.A.resize(mat_size[0]);
                    for (auto &t : first_task.A)
                    {
                        t.resize(mat_size[1]);
                    }
                    // first_task.b = std::vector<float>(mat_size[0]);
                    first_task.b.resize(mat_size[0]);
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
                        static const char *items[] = {"<=", ">="};
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
                if (ImGui::Button("Task to Dual Task"))
                {
                    std::swap(first_task.b, first_task.c);
                    std::swap(mat_size[0], mat_size[1]);
                    first_task.condition = 1 - first_task.condition;
                    first_task.min_max_checker = !first_task.min_max_checker;

                    auto new_a_matrix = std::vector<std::vector<float>>(first_task.A.front().size(), std::vector<float>(first_task.A.size()));
                    for (auto i = 0; i < first_task.A.size(); ++i)
                    {
                        for (auto j = 0; j < first_task.A[i].size(); ++j)
                        {
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
                    if (first_task.min_max_checker != 1)
                    {
                        for (auto &c : clone_c)
                        {
                            c *= -1.f;
                        }
                    }
                    auto clone_a = first_task.A;
                    auto clone_b = first_task.b;
                    if (first_task.condition != 0)
                    {
                        for (auto &i : clone_b)
                        {
                            i *= -1.f;
                        }
                        for (auto &i : clone_a)
                        {
                            for (auto &j : i)
                            {
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
                            .selected_coords = coords};

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
                            .selected_coords = coords};

                        first_task.table_story.push_back(history);

                        std::swap(first_task.rows_name[coords.first], first_task.cols_name[coords.second]);
                        first_task.table.recalculate_matrix(coords);
                    }
                }
                if (!first_task.table_story.empty())
                {
                    size_t index = 0;
                    for (const auto &story : first_task.table_story)
                    {
                        ImGui::Text("%d step", ++index);
                        if (ImGui::BeginTable(("step" + std::to_string(index)).c_str(), story.table.col_count() + 1, ImGuiTableFlags_Borders))
                        {
                            ImGui::TableSetupColumn("");
                            for (auto j = 0; j < story.table.col_count(); ++j)
                            {
                                ImGui::TableSetupColumn(story.cols_name.at(j).c_str());
                            }
                            ImGui::TableHeadersRow();

                            for (auto i = 0; i < story.table.row_count(); ++i)
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                if (i == story.selected_coords.first)
                                {
                                    ImGui::TextColored(color, story.rows_name.at(i).c_str());
                                }
                                else
                                {
                                    ImGui::Text(story.rows_name.at(i).c_str());
                                }
                                for (auto j = 0; j < story.table.col_count(); ++j)
                                {
                                    ImGui::TableNextColumn();
                                    if (i == story.selected_coords.first || j == story.selected_coords.second)
                                    {
                                        ImGui::TextColored(color, "%s", to_string(story.table.at(i, j)).c_str());
                                    }
                                    else
                                    {
                                        ImGui::Text("%s", to_string(story.table.at(i, j)).c_str());
                                    }
                                }
                            }

                            ImGui::EndTable();
                        }
                    }
                }
                ImGui::Text("Final Simplex Table");
                if (first_task.table.col_count() > 1 && first_task.table.row_count() > 1 && ImGui::BeginTable("Final Simplex Table", first_task.table.col_count() + 1, ImGuiTableFlags_Borders))
                {
                    ImGui::TableSetupColumn("");
                    for (auto j = 0; j < first_task.table.col_count(); ++j)
                    {
                        ImGui::TableSetupColumn(first_task.cols_name.at(j).c_str());
                    }
                    ImGui::TableHeadersRow();

                    for (auto i = 0; i < first_task.table.row_count(); ++i)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text(first_task.rows_name.at(i).c_str());
                        for (auto j = 0; j < first_task.table.col_count(); ++j)
                        {
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", to_string(first_task.table.at(i, j)).c_str());
                        }
                    }

                    ImGui::EndTable();
                }
                if (first_task.table.col_count() > 1 && first_task.table.row_count() > 1)
                {
                    ImGui::TextColored(color, "%s value of F = %s", (first_task.min_max_checker ? "Max" : "Min"), to_string((first_task.min_max_checker == 1 ? -1.f : 1.f) * first_task.table.at(first_task.table.row_count() - 1, 0)).c_str());
                    auto x_vals = std::vector<float>(first_task.c.size(), 0.f);
                    auto x_names = std::vector<std::string>(first_task.c.size());
                    for (auto i = 0; i < x_names.size(); ++i)
                    {
                        x_names[i] = "x" + std::to_string(i + 1);
                    }
                    for (auto [row, val] : first_task.rows_name)
                    {
                        if (std::find(x_names.begin(), x_names.end(), val) != x_names.end())
                        {
                            auto index = std::find(x_names.begin(), x_names.end(), val) - x_names.begin();
                            auto value = first_task.table.at(row, 0);
                            x_vals[index] = value;
                        }
                    }
                    std::string long_proof_string = "";
                    float res = 0.f;
                    for (auto i = 0; i < first_task.c.size(); ++i)
                    {
                        res += x_vals[i] * first_task.c[i];
                        long_proof_string += "(" + to_string(x_vals[i]) + ") * (" + to_string(first_task.c[i]) + ")";
                        if (i != first_task.c.size() - 1)
                        {
                            long_proof_string += " + ";
                        }
                    }
                    long_proof_string += " = " + to_string(res);

                    for (auto i = 0; i < first_task.c.size(); ++i)
                    {
                        ImGui::TextColored(color, (x_names[i] + " = " + to_string(x_vals[i])).c_str());
                    }

                    ImGui::TextColored(color, ("proof: " + (first_task.min_max_checker ? std::string("max") : std::string("min")) + " F = " + long_proof_string).c_str());

                    for (auto i = 0; i < first_task.A.size(); ++i)
                    {
                        std::string long_proof = "";
                        float res = 0.f;
                        for (auto j = 0; j < first_task.A[i].size(); ++j)
                        {
                            res += first_task.A[i][j] * x_vals[j];
                            long_proof += "(" + to_string(x_vals[j]) + ") * (" + to_string(first_task.A[i][j]) + ")";
                            if (j != first_task.A[i].size() - 1)
                            {
                                long_proof += " + ";
                            }
                        }
                        // long_proof += " = " + std::to_string(res);
                        long_proof += " <= " + to_string(first_task.b[i]);
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
                if (ImGui::Button("Find best integer solution"))
                {
                    table_routes = SimplexTableRoutes::make_shared(first_task.A, first_task.b, first_task.c);
                    try
                    {
                        has_exception = true;
                        table_routes->solve(first_task.min_max_checker, first_task.condition);
                        has_exception = false;
                    }
                    catch (std::exception &ex)
                    {
                        std::cerr << ex.what() << std::endl;
                    }
                    std::cout << table_routes << std::endl;
                }
                static std::vector<float> good_point_val;
                static float pretty_good_function_value;
                if (ImGui::Button("Find best integer solution (bruteforce)"))
                {
                    good_point_val.clear();
                    pretty_good_function_value = std::numeric_limits<float>::max();
                    if (first_task.min_max_checker)
                    {
                        pretty_good_function_value = std::numeric_limits<float>::min();
                    }
                    std::vector<std::pair<int, int>> min_max_x_vals_for_brute_force(first_task.c.size(), std::make_pair(std::numeric_limits<int>::max(), 0));
                    for (auto i = 0; i < first_task.A.size(); ++i)
                    {
                        for (auto j = 0; j < first_task.A[i].size(); ++j)
                        {
                            auto &pair = min_max_x_vals_for_brute_force[j];
                            auto val = first_task.A[i][j];
                            auto c = first_task.c[i];
                            if (first_task.condition == 0)
                            {
                                if (val > 0 && c > 0)
                                {
                                    pair.second = std::max(int(c / val) + 1, pair.second);
                                }
                                if (val < 0 && c < 0)
                                {
                                    pair.first = std::min(int(c / val), pair.first);
                                }
                                if ((val < 0 && c > 0) || (val > 0 && c < 0))
                                {
                                    pair.first = std::min(pair.first, 0);
                                }
                            }
                            else
                            {
                                if (val > 0 && c > 0)
                                {
                                    pair.first = std::min(int(c / val), pair.first);
                                }
                                if (val < 0 && c < 0)
                                {
                                    pair.second = std::max(int(c / val), pair.second);
                                }
                                if ((val < 0 && c > 0) || (val > 0 && c < 0))
                                {
                                    pair.second = std::max(pair.second, 0);
                                }
                            }
                        }
                    }
                    for (auto &pair : min_max_x_vals_for_brute_force)
                    {
                        if (pair.first == std::numeric_limits<int>::max())
                        {
                            pair.first = 0;
                        }
                        if (pair.second == 0)
                        {
                            pair.second = std::numeric_limits<int>::max();
                        }
                    }

                    std::function<std::vector<std::vector<float>>(const std::vector<std::pair<int, int>> &, const size_t)> func;
                    func = std::function<std::vector<std::vector<float>>(const std::vector<std::pair<int, int>> &, const size_t)>(
                        [&func](const std::vector<std::pair<int, int>> &vec, const size_t i) -> std::vector<std::vector<float>>
                        {
                            if (i >= vec.size())
                                return {};
                            if (i == vec.size() - 1)
                            {
                                std::vector<std::vector<float>> v{};
                                for (auto iter = vec[i].first; iter <= vec[i].second; ++iter)
                                {
                                    std::vector<float> tmp{};
                                    tmp.push_back(iter);
                                    v.push_back(tmp);
                                }
                                return v;
                            }

                            std::vector<std::vector<float>> res{};
                            auto vecs = func(vec, i + 1);
                            for (auto iter = vec[i].first; iter <= vec[i].second; ++iter)
                            {
                                for (const auto &vec : vecs)
                                {
                                    std::vector<float> tmp{};
                                    tmp.push_back(iter);
                                    tmp.insert(tmp.end(), vec.begin(), vec.end());
                                    res.push_back(tmp);
                                }
                            }

                            return res;
                        });
                    auto all_points = func(min_max_x_vals_for_brute_force, 0);
                    for (const auto &point : all_points)
                    {
                        bool good_point = true;
                        for (auto i = 0; i < first_task.A.size(); ++i)
                        {
                            auto val = first_task.b[i];
                            auto res = 0.f;
                            for (auto j = 0; j < first_task.A[i].size(); ++j)
                            {
                                res += first_task.A[i][j] * point[j];
                            }
                            if (first_task.condition)
                            {
                                good_point &= (res >= val);
                            }
                            else
                            {
                                good_point &= (res <= val);
                            }
                            if (!good_point)
                                break;
                        }
                        if (good_point)
                        {
                            auto res = 0.f;
                            for (auto i = 0; i < point.size(); ++i)
                            {
                                res += point[i] * first_task.c[i];
                            }
                            if (first_task.min_max_checker)
                            {
                                if (pretty_good_function_value < res)
                                {
                                    pretty_good_function_value = res;
                                    good_point_val = point;
                                }
                            }
                            else
                            {
                                if (pretty_good_function_value > res)
                                {
                                    pretty_good_function_value = res;
                                    good_point_val = point;
                                }
                            }
                        }
                    }
                }
                if (!good_point_val.empty())
                {
                    ImGui::TextColored(color, "Best Integer Solution via BruteForce");
                    for (auto i = 0; i < good_point_val.size(); ++i)
                    {
                        ImGui::TextColored(color, ("x" + std::to_string(i + 1) + " = " + to_string(good_point_val[i])).c_str());
                    }
                    ImGui::TextColored(color, "value of F = %s", to_string(pretty_good_function_value).c_str());
                }
            }
            ImGui::End();
            if (!has_exception && table_routes)
            {
                float pretty_good_value = std::numeric_limits<float>::max();
                if (first_task.min_max_checker)
                {
                    pretty_good_value = std::numeric_limits<float>::min();
                }
                auto x_vals_for_pretty_god_value = std::vector<float>();

                ImGui::Begin("Tree Table");
                std::function<void(std::shared_ptr<SimplexTableRoutes> &, const std::string &)> func;
                func = std::function<void(std::shared_ptr<SimplexTableRoutes> &, const std::string &)>(
                    [&func, &first_task, &pretty_good_value, &x_vals_for_pretty_god_value](std::shared_ptr<SimplexTableRoutes> &node, const std::string &name)
                    {
                        if (ImGui::TreeNode(name.c_str()))
                        {
                            if (node)
                            {

                                ImGui::TextColored(color, "value of F = %s", to_string(node->value_of_F(first_task.min_max_checker)).c_str());
                                auto x_vals = node->get_xvals();
                                for (auto i = 0; i < x_vals.size(); ++i)
                                {
                                    ImGui::TextColored(color, ("x" + std::to_string(i + 1) + " = " + to_string(x_vals[i])).c_str());
                                }
                                ImGui::TextColored(color, ("Is integer solution ? " + (node->is_integer() ? std::string("True") : std::string("False"))).c_str());

                                if (ImGui::BeginTable(name.c_str(), node->get_table().col_count() + 1, ImGuiTableFlags_Borders))
                                {
                                    ImGui::TableSetupColumn("");
                                    for (auto j = 0; j < node->get_table().col_count(); ++j)
                                    {
                                        ImGui::TableSetupColumn(node->get_cols_name().at(j).c_str());
                                    }
                                    ImGui::TableHeadersRow();

                                    for (auto i = 0; i < node->get_table().row_count(); ++i)
                                    {
                                        ImGui::TableNextRow();
                                        ImGui::TableNextColumn();
                                        ImGui::Text(node->get_rows_name().at(i).c_str());
                                        for (auto j = 0; j < node->get_table().col_count(); ++j)
                                        {
                                            ImGui::TableNextColumn();
                                            ImGui::Text("%s", to_string(node->get_table().at(i, j)).c_str());
                                        }
                                    }
                                }
                                ImGui::EndTable();

                                auto x_index = node->selected_x_val_index();
                                if (x_index != (size_t)-1)
                                {
                                    auto val = x_vals[x_index];
                                    func(node->get_left(), "x" + std::to_string(x_index + 1) + " <= " + std::to_string(int(val)));
                                    func(node->get_right(), "x" + std::to_string(x_index + 1) + " >= " + std::to_string(int(val) + 1));
                                }
                            }
                            else
                            {
                                ImGui::Text("NO SOLUTION!");
                            }
                            ImGui::TreePop();
                        }
                    });
                // if (node)
                //         {
                //             if (first_task.min_max_checker)
                //             {
                //                 if (node->value_of_F(first_task.min_max_checker) > pretty_good_value && node->is_integer())
                //                 {
                //                     pretty_good_value = node->value_of_F(first_task.min_max_checker);
                //                     x_vals_for_pretty_god_value = node->get_xvals();
                //                 }
                //             }
                //             else
                //             {
                //                 if (node->value_of_F(first_task.min_max_checker) < pretty_good_value && node->is_integer())
                //                 {
                //                     pretty_good_value = node->value_of_F(first_task.min_max_checker);
                //                     x_vals_for_pretty_god_value = node->get_xvals();
                //                 }
                //             }
                //         }    
                std::function<void(std::shared_ptr<SimplexTableRoutes>&)> func2;
                func2 = std::function<void(std::shared_ptr<SimplexTableRoutes>&)>(
                    [&func2, &first_task, &pretty_good_value, &x_vals_for_pretty_god_value](std::shared_ptr<SimplexTableRoutes>& node){
                        if (node)
                        {
                            if (first_task.min_max_checker)
                            {
                                if (node->value_of_F(first_task.min_max_checker) > pretty_good_value && node->is_integer())
                                {
                                    pretty_good_value = node->value_of_F(first_task.min_max_checker);
                                    x_vals_for_pretty_god_value = node->get_xvals();
                                }
                            }
                            else
                            {
                                if (node->value_of_F(first_task.min_max_checker) < pretty_good_value && node->is_integer())
                                {
                                    pretty_good_value = node->value_of_F(first_task.min_max_checker);
                                    x_vals_for_pretty_god_value = node->get_xvals();
                                }
                            }
                            func2(node->get_left());
                            func2(node->get_right());
                        }
                    }
                );
                func2(table_routes);
                ImGui::TextColored(color, "Best integer solution");
                for (auto i = 0; i < x_vals_for_pretty_god_value.size(); ++i)
                {
                    ImGui::TextColored(color, ("x" + std::to_string(i + 1) + " = " + to_string(x_vals_for_pretty_god_value[i])).c_str());
                }
                ImGui::TextColored(color, "F = %s", to_string(pretty_good_value).c_str());
                func(table_routes, "Root");
                ImGui::End();
            }

            Window::SwapBuffers();
        }
    }
    Window::term();
}