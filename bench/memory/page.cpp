#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

static void BM_MinorPageFault(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    std::vector<int> arr;
    arr.resize(100'000'000);
    state.ResumeTiming();

    for (auto i = 0u; i < arr.size(); ++i) {
      arr[i] = i;
    }
  }
}

static void BM_NoMinorPageFault(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    std::vector<int> arr;
    arr.resize(100'000'000);
    std::fill(arr.begin(), arr.end(), 0);
    state.ResumeTiming();

    for (auto i = 0u; i < arr.size(); ++i) {
      arr[i] = i;
    }
  }
}

BENCHMARK(BM_MinorPageFault);
BENCHMARK(BM_NoMinorPageFault);

BENCHMARK_MAIN();
