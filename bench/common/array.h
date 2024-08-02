#pragma once

#include <vector>

inline std::vector<int> MakeArr(int size = 1'000'000) {
  srand(time(NULL));

  std::vector<int> arr(1'000'000);
  for (auto &x : arr) {
    x = rand() % 256;
  }

  return arr;
}
