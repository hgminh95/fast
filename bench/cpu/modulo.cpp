#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

static void BM_Modulo(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  std::array<int, 5> modulos{11, 107, 1013, 19211, 81727};

  for (auto _ : state) {
    int sum{0};
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[i] % modulos[i * 5 / arr.size()];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_StaticModulo(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    int sum{0};
    for (auto i = 0u; i < arr.size(); ++i) {
      switch (i * 5 / arr.size()) {
        case 0:
          sum += arr[i] % 11;
          break;
        case 1:
          sum += arr[i] % 107;
          break;
        case 2:
          sum += arr[i] % 1013;
          break;
        case 3:
          sum += arr[i] % 19211;
          break;
        case 4:
          sum += arr[i] % 81727;
          break;
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_Modulo);
BENCHMARK(BM_StaticModulo);

BENCHMARK_MAIN();
