#include "matrix.h"

#include <thread>
#include <atomic>
#include <cmath>

// Classic single-thread Matrix multiplication code:
// for (size_t row = 0; row < m1.height(); ++row) {
//   for (size_t col = 0; col < m2.width(); ++col) {
//     for (size_t k = 0; k < m1.width(); ++k) {
//       result.at(row, col) += (m1.at(row, k) * m2.at(k, col));
//     }
//   }
// }

// Optimized CPU cache code(two inner loops is swapped):
// for (size_t row = 0; row < m1.height(); ++row) {
//   for (size_t k = 0; k < m1.width(); ++k) {
//     for (size_t col = 0; col < m2.width(); ++col) {
//       result.at(row, col) += (m1.at(row, k) * m2.at(k, col));
//     }
//   }
// }
// Optimized code works 2-3 time faster

// We take the body of the inner loop as a job with parameters col and k.

namespace {

inline auto indexToJob(size_t index, size_t width)
{
  struct {
    size_t row;
    size_t k;
  } res{index / width, index % width};

  return res;
}

Matrix multiThreadImpl(const Matrix& m1, const Matrix& m2)
{
  if (m1.height() != m2.width())
    throw MatrixException("Wrong size");

  Matrix result(m1.height(), m2.width());
  const size_t totalJobs = m1.height() * m1.width();
  std::atomic_size_t currentJob{};

  auto worker = [&]() noexcept {
    while (currentJob != totalJobs) {
      auto cur = currentJob.load();
      if (currentJob.compare_exchange_strong(cur, cur + 1)) {
        auto job = indexToJob(cur, m1.width());
        for (size_t col = 0; col < m2.width(); ++col)
          result.at(job.row, col) += m1.at(job.row, job.k) * m2.at(job.k, col);
      }
    }
  };

  std::vector<std::thread> threadPool;
  for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
    threadPool.emplace_back(worker);

  for (auto& thread: threadPool)
    thread.join();

  return result;
}

} // namespace

MatrixException::MatrixException(const std::string &msg)
  : std::runtime_error(msg)
{}

Matrix::Matrix(size_t height, size_t width)
  : height_(height)
  , width_(width)
  , data_(height, std::vector<double>(width))
{
  if (!height != !width) // One of dimensions is zero other not zero
    throw MatrixException("Not zero dimension for empty matrix");
}

Matrix::Matrix(std::vector<double> values, size_t height, size_t width)
  : Matrix(values.begin(), values.end(), height, width)
{}

size_t Matrix::height() const
{
  return height_;
}

size_t Matrix::width() const
{
  return width_;
}

double Matrix::at(size_t row, size_t col) const
{
  return data_[row][col];
}

double &Matrix::at(size_t row, size_t col)
{
  return data_[row][col];
}

Matrix Matrix::operator*(const Matrix &other) const
{
  return multiThreadImpl(*this, other);
//  return singleThreadImpl(*this, other);
}

bool Matrix::equals(const Matrix &other, double eps) const
{
  if (width() != other.width() || height() != other.height())
    return false;

  for (size_t row = 0; row < height(); ++row) {
    for (size_t col = 0; col < width(); ++col) {
      if (std::abs(at(row, col) - other.at(row, col)) > eps) {
        return false;
      }
    }
  }


  return true;
}

std::ostream &operator<<(std::ostream &s, const Matrix &m)
{
  if (!m.height()) {
    s << "Empty";
    return s;
  }

  for (const auto& row: m.data_) {
    for (const auto& val: row) {
      s << val << '\t';
    }
    s << '\n';
  }

  return s;
}
