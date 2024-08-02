#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

static void BM_Dependency(benchmark::State& state) {
  auto a = MakeArr(90);

  for (auto _ : state) {
    int sum{0};
    for (auto i = 0u; i < 90; ++i) {
      sum *= 10;
      sum += a[i];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_LessDependency(benchmark::State& state) {
  auto a = MakeArr(90);

  for (auto _ : state) {
    int sum{0};
    for (auto i = 0u; i < 90; i += 3) {
      sum *= 1000;
      sum += a[i] * 100;
      sum += a[i + 1] * 10;
      sum += a[i + 2];
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_Dependency);
BENCHMARK(BM_LessDependency);

BENCHMARK_MAIN();
