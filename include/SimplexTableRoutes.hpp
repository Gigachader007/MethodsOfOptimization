#pragma once
#include "SimplexTable.hpp"
#include <map>
#include <memory>
#include <algorithm>

class SimplexTableRoutes
{
private:
    std::vector<std::vector<float>> A = {};
    std::vector<float> b = {};
    std::vector<float> c = {};
    std::map<int, std::string> cols_name{}, rows_name{};
    SimplexTable table;
    bool all_integers;
    size_t selected_x = -1;

    std::shared_ptr<SimplexTableRoutes> left = nullptr, right = nullptr;

public:
    SimplexTableRoutes(const std::vector<std::vector<float>> &A = {}, const std::vector<float> &b = {}, const std::vector<float> &c = {}) : A(A), b(b), c(c), table(A, b, c) {}

    static auto make_shared(const std::vector<std::vector<float>> &A = {}, const std::vector<float> &b = {}, const std::vector<float> &c = {})
    {
        return std::make_shared<SimplexTableRoutes>(A, b, c);
    }
    auto &get_right()
    {
        return right;
    }
    auto &get_left()
    {
        return left;
    }
    auto is_integer()
    {
        return all_integers;
    }
    auto selected_x_val_index(){
        return selected_x;
    }
    auto &get_cols_name()
    {
        return cols_name;
    }
    auto &get_rows_name()
    {
        return rows_name;
    }
    auto &get_table()
    {
        return table;
    }
    auto value_of_F(const bool min_max)
    {
        auto val = table.at(table.row_count() - 1, 0);
        if (min_max)
        {
            val *= -1.f;
        }
        return val;
    }
    auto get_xvals()
    {
        auto x_vals = std::vector<float>(c.size(), 0.f);
        auto x_names = std::vector<std::string>(c.size());
        for (auto i = 0; i < x_names.size(); ++i)
        {
            x_names[i] = "x" + std::to_string(i + 1);
        }
        for (auto [row, val] : rows_name)
        {
            if (std::find(x_names.begin(), x_names.end(), val) != x_names.end())
            {
                auto index = std::find(x_names.begin(), x_names.end(), val) - x_names.begin();
                auto value = table.at(row, 0);
                x_vals[index] = value;
            }
        }
        return x_vals;
    }
    void solve(const bool min_max_checker, const bool condition)
    {
        cols_name.clear();
        rows_name.clear();

        cols_name.insert({0, "Si0"});
        rows_name.insert({A.size(), "F"});

        left.reset();
        right.reset();

        for (auto j = 0; j < A[0].size(); ++j)
        {
            cols_name.insert({j + 1, "x" + std::to_string(j + 1)});
        }
        for (auto i = 0; i < A.size(); ++i)
        {
            rows_name.insert({i, "x" + std::to_string(i + 1 + A[0].size())});
        }
        auto clone_c = c;
        if (!min_max_checker)
        {
            for (auto &c : clone_c)
            {
                c *= -1.f;
            }
        }
        auto clone_a = A;
        auto clone_b = b;
        if (condition)
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
        table = SimplexTable(clone_a, clone_b, clone_c);

        while (true)
        {
            auto coords = table.phase1();
            if (coords == std::pair(-1, -1))
            {
                break;
            }

            std::swap(rows_name[coords.first], cols_name[coords.second]);
            table.recalculate_matrix(coords);
        }
        while (true)
        {
            auto coords = table.phase2();
            if (coords == std::pair(-1, -1))
            {
                break;
            }

            std::swap(rows_name[coords.first], cols_name[coords.second]);
            table.recalculate_matrix(coords);
        }
        auto x_vals = get_xvals();

        all_integers = true;
        for (auto i = 0; i < x_vals.size(); ++i)
        {
            if (x_vals[i] != float(int(x_vals[i])))
            {
                selected_x = i;
                all_integers = false;
                {
                    auto left_a = clone_a;
                    left_a.resize(left_a.size() + 1);
                    left_a[left_a.size() - 1].resize(left_a.front().size());

                    auto left_b = clone_b;
                    left_b.resize(left_b.size() + 1);

                    left_a[left_a.size() - 1][i] = 1;
                    left_b[left_b.size() - 1] = float(int(x_vals[i]));

                    auto _left = SimplexTableRoutes::make_shared(left_a, left_b, c);
                    try
                    {
                        _left->solve(min_max_checker, false);
                        left = _left;
                    }
                    catch (std::exception &ex)
                    {
                        std::cerr << ex.what() << std::endl;
                    }
                }
                {
                    auto right_a = clone_a;
                    right_a.resize(right_a.size() + 1);
                    right_a[right_a.size() - 1].resize(right_a.front().size());

                    auto right_b = clone_b;
                    right_b.resize(right_b.size() + 1);

                    right_a[right_a.size() - 1][i] = -1;
                    right_b[right_b.size() - 1] = -(float(int(x_vals[i])) + 1);
                    auto _right = SimplexTableRoutes::make_shared(right_a, right_b, c);
                    try
                    {
                        _right->solve(min_max_checker, false);
                        right = _right;
                    }
                    catch (std::exception &ex)
                    {
                        std::cerr << ex.what() << std::endl;
                    }
                }
                break;
            }
        }
        if (all_integers)
            return;
    }
};