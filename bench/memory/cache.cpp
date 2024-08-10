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

template <bool Prefetch = false>
static void BM_StrideAccess(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    state.PauseTiming();
    for (auto i = 0u; i < arr.size(); ++i) {
      arr[i] = (i + state.range(0)) % arr.size();
    }
    state.ResumeTiming();
    int sum{0};
    int p{0};
    for (auto i = 0u; i < arr.size(); ++i) {
      if constexpr (Prefetch) {
        __builtin_prefetch(&arr[(p + 1 * state.range(0)) % arr.size()], 0, 0);
        __builtin_prefetch(&arr[(p + 2 * state.range(0)) % arr.size()], 0, 0);
        // __builtin_prefetch(&arr[(p + 3 * state.range(0)) % arr.size()], 0, 0);
      }

      sum += arr[p];
      p = arr[p];
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

BENCHMARK(BM_StrideAccess<false>)->Range(1, 1 << 15);
BENCHMARK(BM_StrideAccess<true>)->Range(1, 1 << 15);

BENCHMARK_MAIN();
