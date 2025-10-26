#include <vector>
#include <iostream>
#include <limits>

class SimplexTable
{
    std::vector<std::vector<float>> simplex_table;

public:
    SimplexTable& operator=(const SimplexTable&) = default;
    SimplexTable(const SimplexTable&) = default;

    SimplexTable& operator=(SimplexTable&&) = default;
    SimplexTable(SimplexTable&&) = default;
    // Ax <= b
    // F = cx -> min
    SimplexTable(const std::vector<std::vector<float>> &A = {}, const std::vector<float> &b = {}, const std::vector<float> &c = {})
    {
        simplex_table = std::vector<std::vector<float>>(b.size() + 1, std::vector<float>(c.size() + 1, 0.f));
        for (auto i = 0; i < simplex_table.size() - 1; ++i)
        {
            simplex_table[i][0] = b[i];
        }
        for (auto i = 0; i < A.size(); ++i)
        {
            for (auto j = 0; j < A[i].size(); ++j)
            {
                simplex_table[i][j + 1] = A[i][j];
            }
        }
        for (auto i = 0; i < c.size(); ++i)
        {
            simplex_table[simplex_table.size() - 1][i + 1] = c[i];
        }
    }
    size_t row_count() const
    {
        return simplex_table.size();
    }
    size_t col_count() const
    {
        return simplex_table.front().size();
    }
    float at(const size_t row, const size_t col) const
    {
        return simplex_table[row][col];
    }
    float &at(const size_t row, const size_t col)
    {
        return simplex_table[row][col];
    }
    void print() const
    {
        for (auto i = 0; i < simplex_table.size(); ++i)
        {
            for (auto j = 0; j < simplex_table[i].size(); ++j)
            {
                std::cout << simplex_table[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    auto phase1()
    {
        auto col = -1;
        for (auto i = 0; i < simplex_table.size() - 1; ++i)
        {
            auto Si0 = simplex_table[i][0];
            if (Si0 < 0.f)
            {
                bool flag = true;
                for (auto j = 1; j < simplex_table[i].size(); ++j)
                {
                    if (simplex_table[i][j] < 0.f)
                    {
                        if (col == -1)
                            col = j;
                        flag = false;
                        break;
                    }
                }
                if (flag)
                    throw std::runtime_error("Функция не ограничена!");
            }
        }
        if (col == -1)
        {
            return std::pair(-1, -1);
        }
        auto row = -1;
        float min_pos_val = std::numeric_limits<float>::max();
        for (auto i = 0; i < simplex_table.size() - 1; ++i)
        {
            auto Si0 = simplex_table[i][0];
            auto col_val = simplex_table[i][col];
            if (Si0 / col_val > 0.f && Si0 / col_val < min_pos_val)
            {
                row = i;
                min_pos_val = Si0 / col_val;
            }
        }
        return std::pair(row, col);
    }
    void recalculate_matrix(const std::pair<int, int> &coords)
    {
        auto [solve_row, solve_col] = coords;

        auto new_simplex_table = simplex_table;
        for (auto i = 0; i < simplex_table.size(); ++i)
        {
            for (auto j = 0; j < simplex_table[i].size(); ++j)
            {
                if (i == solve_row && j == solve_col)
                {
                    new_simplex_table[i][j] = 1.f / simplex_table[solve_row][solve_col];
                }
                else if (i == solve_row)
                {
                    new_simplex_table[i][j] = simplex_table[i][j] / simplex_table[solve_row][solve_col];
                }
                else if (j == solve_col)
                {
                    new_simplex_table[i][j] = -simplex_table[i][j] / simplex_table[solve_row][solve_col];
                }
                else
                {
                    new_simplex_table[i][j] = simplex_table[i][j] - ((simplex_table[solve_row][j] * simplex_table[i][solve_col]) / simplex_table[solve_row][solve_col]);
                }
            }
        }
        simplex_table = new_simplex_table;
    }
    std::pair<int, int> phase2()
    {
        // Поиск ведущего столбца: максимальный положительный коэффициент в строке целевой функции
        int col = -1;
        float max_c = 0.f;
        for (size_t j = 1; j < simplex_table[simplex_table.size() - 1].size(); ++j)
        {
            if (simplex_table[simplex_table.size() - 1][j] > max_c)
            {
                max_c = simplex_table[simplex_table.size() - 1][j];
                col = j;
            }
        }

        // Если нет положительных коэффициентов в целевой функции, решение оптимально
        if (col == -1)
        {
            return {-1, -1};
        }

        // Проверка на неограниченность: если все элементы в столбце col неположительны
        bool unbounded = true;
        for (size_t i = 0; i < simplex_table.size() - 1; ++i)
        {
            if (simplex_table[i][col] > 0.f)
            {
                unbounded = false;
                break;
            }
        }
        if (unbounded)
        {
            throw std::runtime_error("Не ограничена");
        }

        // Выбор ведущей строки: минимальное положительное отношение b[i]/a[i][col]
        float min_ratio = std::numeric_limits<float>::max();
        int row = -1;
        for (size_t i = 0; i < simplex_table.size() - 1; ++i)
        {
            if (simplex_table[i][col] > 0.f)
            {
                float ratio = simplex_table[i][0] / simplex_table[i][col];
                if (ratio < min_ratio && ratio >= 0.f)
                {
                    min_ratio = ratio;
                    row = i;
                }
            }
        }

        // Если не найдена подходящая строка, решение не существует
        if (row == -1)
        {
            throw std::runtime_error("Нет подходящего решения");
        }

        return {row, col};
    }
};