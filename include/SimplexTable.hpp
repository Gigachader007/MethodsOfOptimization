#include <optional>

#include "Matrix.hpp"

//   Si0    x1   x2
//x3  2     1    -2
//x4  -2    -2    1
//x5  5     1     1
// F  0     1    -1


class SimplexTable {
    Matrix mat;
public:
    explicit SimplexTable(const Matrix& mat) : mat(mat) {}

    std::optional<size_t> find_negative_free_member_row() const {
        for(auto i = 0; i < mat.rows_num() - 1; ++i){
            if(mat.at(i, 0) < 0.f) {
                return i;
            }
        }
        return std::nullopt;
    }
    std::optional<size_t> find_solving_col() const {
        auto free_member_row = find_negative_free_member_row();
        if(free_member_row.has_value()){
            auto unwraped_free_member_row = free_member_row.value();
            // 0 for free members
            for(auto j = 1; j < mat.cols_num(); ++j){
                if(mat.at(unwraped_free_member_row, j) < 0){
                    return j;
                }
            }
            return std::nullopt;
        }
        // 'cause we ignore free member col
        for(auto j = 1; j < mat.cols_num(); ++j){
            if(mat.at(mat.rows_num() - 1, j) > 0){
                return j;
            }
        }
        return std::nullopt;
    }

    std::optional<size_t> find_solving_row(const size_t col) const {
        std::optional<float>  min_pos_num = std::nullopt;
        std::optional<size_t> row_index = std::nullopt;

        for(auto i = 0; i < mat.rows_num(); ++i){
            auto val = mat.at(i, 0) / mat.at(i, col);

            if(val > 0){
                if(min_pos_num.has_value() && row_index.has_value()){
                    auto unwraped_min_pos_num = min_pos_num.value();

                    if(val < unwraped_min_pos_num){
                        min_pos_num = val;
                        row_index = i;
                    }
                }
                else{
                    min_pos_num = val;
                    row_index = i;
                }
            }
        }

        return row_index;
    }

    void recalculate_matrix(const size_t row, const size_t col) {
        auto matcher = [&](const size_t r, const size_t c){
            if(r == row && c == col) {
                return 1.0f / mat.at(row, col);
            }
            if(r == row){
                return mat.at(r, c) / mat.at(row, col);
            }
            if(c == col){
                return -mat.at(r, c) / mat.at(row, col);
            }
            return mat.at(r, c) - (mat.at(r, col) * mat.at(row, c)) / mat.at(row, col);
        };

        auto new_mat = mat;
        for(auto i = 0; i < mat.rows_num(); ++i){
            for(auto j = 0; j < mat.cols_num(); ++j){
                new_mat.at(i, j) = matcher(i, j);
            }
        }
        mat = std::move(new_mat);
    }

    bool is_done() const {
        if(find_negative_free_member_row().has_value()){
            return false;
        }

        for(auto j = 1; j < mat.cols_num(); ++j){
            if(mat.at(mat.rows_num() - 1, j) > 0){
                return false;
            }
        }
        return true;
    }

    Matrix& get_matrix() {
        return mat;
    }
    Matrix get_matrix() const {
        return mat;
    }
};