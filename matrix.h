#pragma once

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

class MatrixException : public std::runtime_error {
public:
  explicit MatrixException(const std::string& msg);
};

class Matrix {
public:
  Matrix() = default;
  Matrix(size_t height, size_t width);
  template<class It>
  Matrix(It begin, It end, size_t height, size_t width);

  size_t height() const;
  size_t width() const;

  double at(size_t row, size_t col) const;
  double& at(size_t row, size_t col);

  Matrix operator*(const Matrix& other) const;
  friend std::ostream& operator<<(std::ostream& os, const Matrix& m);

private:
  std::vector<std::vector<double>> data_;
};

std::ostream& operator<<(std::ostream& s, const Matrix& m);

template<class It>
Matrix::Matrix(It begin, It end, size_t height, size_t width)
    : data_(height, std::vector<double>(width))
{
  if (!height != !width) // One dimension is zero, other not zero
    throw MatrixException("Not zero dimension for empty matrix");

  for (size_t r = 0; r < height; ++r) {
    for (size_t c = 0; c < width; ++c) {
      if (begin == end)
        throw MatrixException("Wrong range size");
      data_[r][c] = *(begin++);
    }
  }

  if (begin != end)
    throw MatrixException("Wrong input range size");
}
