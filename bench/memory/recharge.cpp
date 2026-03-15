#include <benchmark/benchmark.h>

#include "common/array.h"

static void BM_SameRowAccess(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[i];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_DifferentRowAccess(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);
  constexpr int ROW_SIZE = 8192 / sizeof(int);

  for (auto _ : state) {
    int sum = 0;
    int idx = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[idx];
      idx = (idx + ROW_SIZE) % arr.size();
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_SameRowAccess);
BENCHMARK(BM_DifferentRowAccess);

BENCHMARK_MAIN();
