#include <benchmark/benchmark.h>

#include "common/array.h"

static void BM_SequentialAccess(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[i];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_PageStrideAccess(benchmark::State& state) {
  constexpr int PAGE_STRIDE = 4096 / sizeof(int);
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    int sum = 0;
    int idx = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[idx];
      idx = (idx + PAGE_STRIDE) % arr.size();
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_SequentialAccess);
BENCHMARK(BM_PageStrideAccess);

BENCHMARK_MAIN();
