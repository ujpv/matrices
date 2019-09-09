#include "matrix.h"

#include <thread>
#include <atomic>

MatrixException::MatrixException(const std::string &msg)
  : std::runtime_error(msg)
{}

Matrix::Matrix(size_t height, size_t width)
  : data_(height, std::vector<double>(width))
{
  if (!height != !width) // One dimmestion is zero other not zero
    throw MatrixException("Not zero dimension for empty matrix");
}

size_t Matrix::height() const
{
  return data_.size();
}

size_t Matrix::width() const
{
  return data_.front().size();
}

double Matrix::at(size_t row, size_t col) const
{
  return data_[row][col];
}

double &Matrix::at(size_t row, size_t col)
{
  return data_[row][col];
}

// Classic single thread code:
// for (size_t row = 0; row < lhs_.height(); ++row) {
//   for (size_t col = 0; col < rhs_.width(); ++col) {
//     for (size_t k = 0; k < lhs_.width(); ++k) {
//       res_.at(i, j) += (lhs_.at(i, k) * rhs_.at(k, j));
//     }
//   }
// }

// Optimised single thread code:
// for (size_t row = 0; row < lhs_.height(); ++row) {
//   for (size_t k = 0; k < lhs_.width(); ++k) {
//     for (size_t col = 0; col < rhs_.width(); ++col) {
//       res_.at(i, j) += (lhs_.at(i, k) * rhs_.at(k, j));
//     }
//   }
// }

namespace  {

inline auto indexToJob(size_t index, size_t width) {
  struct {
    size_t row;
    size_t k;
  } res {index / width, index % width};

  return res;
}

Matrix mutl_multithread_impl(const Matrix& m1, const Matrix& m2)
{
  if (m1.height() != m2.width())
    throw MatrixException("Wrong size");

  Matrix result(m1.height(), m2.width());
  const size_t totalJob = m1.height() * m2.width();
  std::atomic_size_t currentJob;

  auto worker = [&]() noexcept {
    while (currentJob != totalJob) {
      auto cur = currentJob.load();
      if (currentJob.compare_exchange_strong(cur, cur + 1)) {
        auto job = indexToJob(cur, m1.width());
        for (size_t col = 0; col < m1.width(); ++col) {
          result.at(job.row, col) += (m1.at(job.row, job.k) * m2.at(job.k, col));
        }
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

//inline auto indexToRc(size_t index, size_t width) {
//  struct {
//    size_t row;
//    size_t col;
//  } res = {index / width, index % width};

//  return res;
//}

//Matrix mutl_multithread_not_optimised_impl(const Matrix& m1, const Matrix& m2)
//{
//  Matrix result(m1.height(), m2.width());
//  const size_t totalJob = m1.height() * m2.width();
//  std::atomic_size_t currentJob;

//  auto worker = [&](){
//    while (currentJob != totalJob) {
//      auto cur = currentJob.load();
//      if (currentJob.compare_exchange_weak(cur, cur + 1)) {
//        auto pos = indexToRc(cur, m1.width());
//        for (size_t k = 0; k < m1.width(); ++k) {
//          result.at(pos.row, pos.col) += (m1.at(pos.row, k) * m2.at(k, pos.col));
//        }
//      }
//    }
//  };

//  std::vector<std::thread> threadPool;
//  threadPool.resize(std::thread::hardware_concurrency());
//  for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
//    threadPool.emplace_back(worker);

//  for (auto& thread: threadPool)
//      thread.join();

//  return result;
//}

} // namespace


Matrix Matrix::operator*(const Matrix &other) const
{
  return mutl_multithread_impl(*this, other);
}

std::ostream &operator<<(std::ostream &s, const Matrix &m)
{
  for (const auto& row: m.data_) {
    for (const auto& val: row) {
      s << val << '\t';
    }
    s << '\n';
  }

  return s;
}
