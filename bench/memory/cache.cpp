#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

template <size_t N>
static void BM_Cache(benchmark::State& state) {
  auto arr1 = MakeArr(N);
  auto arr2 = MakeArr(N);

  for (auto _ : state) {
    int sum{0};
    for (auto j = 0u; j < 1024 / N; ++j) {
      for (auto i = 0u; i < N; ++i) {
        sum += arr1[i] + arr2[i];
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_Cache<1>);
BENCHMARK(BM_Cache<2>);
BENCHMARK(BM_Cache<4>);
BENCHMARK(BM_Cache<8>);
BENCHMARK(BM_Cache<16>);

BENCHMARK(BM_Cache<64>);
BENCHMARK(BM_Cache<128>);
BENCHMARK(BM_Cache<256>);

BENCHMARK_MAIN();
