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
    auto data = readData(std::cin);
    if (data.size() & 1u) {
      std::cerr << "Input data size is odd\n";
      return 1;
    }

    auto dataSize = data.size() / 2;
    // root calculation below safe if dataSize < (2^53)
    // in perfect world we should avoid algorithms with float precision numbers
    auto height = static_cast<size_t>(std::sqrt(dataSize));
    if (height * height != dataSize) {
      std::cerr << "Matrix is not squared\n";
      return 1;
    }

    std::cout << Matrix(data.begin(), data.begin() + dataSize, height, height) *
                 Matrix(data.begin() + dataSize, data.end(), height, height);
  } catch (std::exception& e) {
    std::cerr << "Error:\n" << e.what() << '\n';
    return 1;
  } catch (...) {
    std::cerr << "Unknown error\n";
    return 1;
  }

  return 0;
}
