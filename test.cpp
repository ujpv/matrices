#include "matrix.h"

#include <iostream>
#include <stdexcept>

namespace { // test utils

struct TestCase {
  Matrix m1, m2, expexted;
};

const std::vector<TestCase> TESTS {
  {Matrix(0, 0),
   Matrix(0, 0),
   Matrix(0, 0)},

  {Matrix({2}, 1, 1),
   Matrix({3}, 1, 1),
   Matrix({6}, 1, 1)},

  {Matrix({1, 2, 3, 4}, 2, 2),
   Matrix({5, 6, 7, 8}, 2, 2),
   Matrix({19, 22, 43, 50}, 2, 2)},

  {Matrix({1, 2, 3, 4}, 4, 1),
   Matrix({5, 6, 7, 8}, 1, 4),
   Matrix({5,6,7,8,10,12,14,16,15,18,21,24,20,24,28,32}, 4, 4)},
};

constexpr double EPS = 0.1;

} // namespace test utils

int main() {
  for (size_t i = 0; i < TESTS.size(); ++i) {
    std::cout << "Test #" << i + 1 << "...";
    const auto& t = TESTS[i];
    auto result = t.m1 * t.m2;
    if (t.expexted.equals(result, EPS)) {
      std::cout << "Ok\n";
    }else {
      std::cout << "Failed\n";
      std::cout << "Expected:\n"
                << t.expexted
                << "\n\nBut got:\n"
                << result
                << "\n\n";
    }
  }
}

