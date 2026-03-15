#include <benchmark/benchmark.h>

#include <functional>

#include "common/array.h"

__attribute__((noinline)) void ProcessOne(int& value) {
  value = value * 3 + 1;
  if (value > 1000) value %= 1000;
}

static void BM_OneByOne(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    for (auto& v : arr) {
      ProcessOne(v);
    }
    benchmark::DoNotOptimize(arr.data());
  }
}

__attribute__((noinline)) void ProcessBatch(int* data, int size) {
  for (int i = 0; i < size; ++i) {
    data[i] = data[i] * 3 + 1;
    if (data[i] > 1000) data[i] %= 1000;
  }
}

static void BM_Batched(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    ProcessBatch(arr.data(), arr.size());
    benchmark::DoNotOptimize(arr.data());
  }
}

BENCHMARK(BM_OneByOne);
BENCHMARK(BM_Batched);

BENCHMARK_MAIN();
