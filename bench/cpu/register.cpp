#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

template <size_t N>
static void BM_Register(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    int sum{0};
    for (auto i = 0u; i < arr.size(); i += N) {
      for (int j = 0; j < N; ++j) {
        sum += arr[i + j] * (i + j);
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_Register<4>);
BENCHMARK(BM_Register<8>);

BENCHMARK_MAIN();
