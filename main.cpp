#include <iostream>
#include <cmath>

#include "matrix.h"

using namespace std;

std::vector<double> readData(std::istream& stream) {
  std::vector<double> data;
  double number{};

  while (stream >> number)
    data.push_back(number);

  if (stream.eof())
    return data;

  throw std::runtime_error("Parse error");
}

int main()
{
  try {
    Matrix m1;
    Matrix m2;

    {
      auto data = readData(std::cin);
      if (data.size() & 1) {
        std::cerr << "Input data size is odd";
        return 1;
      }

      auto dataSize = data.size() / 2;
      auto height = static_cast<size_t>(std::sqrt(dataSize));
      if (height * height != dataSize) {
        std::cerr << "Matrix is not squared";
        return 1;
      }

      m1 = Matrix(data.begin(), data.begin() + dataSize, height, height);
      m2 = Matrix(data.begin() + dataSize, data.end(), height, height);
      std::cout << m1 * m2;
    }
  } catch (std::exception& e) {
    std::cerr << "Error:\n" << e.what();
    return 1;
  } catch (...) {
    std::cerr << "Unknown error";
    return 1;
  }

  return 0;
}
