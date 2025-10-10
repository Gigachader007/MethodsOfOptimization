#include <vector>
#include <cstdint>
#include <stdexcept>

class Matrix{
    size_t rows, cols;
    std::vector<float> vals;
public:
    explicit Matrix(const size_t rows, 
                    const size_t cols, 
                    const std::vector<float>& _vals = {}) : rows(rows), cols(cols), vals(_vals) 
    {
        if(vals.empty()){
            vals.resize(rows * cols);
        }
        else if(vals.size() != rows * cols) {
            throw std::runtime_error("Invalid matrix size");
        }
    }
    
    Matrix(const Matrix&) = default;
    Matrix(Matrix&&) = default;

    Matrix& operator=(const Matrix&) = default;
    Matrix& operator=(Matrix&&) = default;

    float& at(const size_t r, const size_t c) {
        if(r >= rows || c >= cols){
            throw std::runtime_error("Index out of range!");
        }
        return vals.at(cols * r + c);
    }
    float at(const size_t r, const size_t c) const {
        if(r >= rows || c >= cols){
            throw std::runtime_error("Index out of range!");
        }
        return vals.at(cols * r + c);
    }
    std::vector<float>& get_values(){
        return vals;
    }
    std::vector<float> get_values() const {
        return vals;
    }
    size_t cols_num() const {
        return cols;
    }
    size_t rows_num() const {
        return rows;
    }

};