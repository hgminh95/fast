#include <benchmark/benchmark.h>

#include "common/array.h"

static void BM_FalseDependency(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += __builtin_popcount(arr[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_NoFalseDependency(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    int r0 = 0, r1 = 0, r2 = 0, r3 = 0;
    for (auto i = 0u; i < arr.size(); i += 4) {
      r0 += __builtin_popcount(arr[i]);
      r1 += __builtin_popcount(arr[i + 1]);
      r2 += __builtin_popcount(arr[i + 2]);
      r3 += __builtin_popcount(arr[i + 3]);
    }
    benchmark::DoNotOptimize(r0 + r1 + r2 + r3);
  }
}

BENCHMARK(BM_FalseDependency);
BENCHMARK(BM_NoFalseDependency);

BENCHMARK_MAIN();
