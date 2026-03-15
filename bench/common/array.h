#pragma once

#include <type_traits>
#include <vector>

template <typename T = int>
std::vector<T> MakeArr(int size = 1'000'000) {
  srand(time(NULL));

  std::vector<T> arr(size);
  for (auto &x : arr) {
    if constexpr (std::is_same_v<T, int>) {
      x = rand() % 256;
    }
  }

  return arr;
}
