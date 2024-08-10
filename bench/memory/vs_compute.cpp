#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

template <size_t N>
struct FooWithCachedBar {
  int values[N];
  int cached_bar{0};

  int bar() {
    if (cached_bar == 0) {
      for (int i = 0; i < N; ++i) {
        cached_bar += values[i] * i;
      }
    }

    return cached_bar;
  }
};

template <size_t N>
struct FooWithoutCachedBar {
  int values[N];

  int bar() {
    int res{0};
    for (int i = 0; i < N; ++i) {
      res += values[i] * i;
    }

    return res;
  }
};

static void BM_FooWithCachedBar(benchmark::State& state) {
  auto arr = MakeArr<FooWithCachedBar<30>>(1'000'000);

  for (auto _ : state) {
    for (auto &foo : arr) {
      benchmark::DoNotOptimize(foo.bar());
    }
  }
}

static void BM_FooWithoutCachedBar(benchmark::State& state) {
  auto arr = MakeArr<FooWithoutCachedBar<30>>(1'000'000);

  for (auto _ : state) {
    for (auto &foo : arr) {
      benchmark::DoNotOptimize(foo.bar());
    }
  }
}

BENCHMARK(BM_FooWithCachedBar);
BENCHMARK(BM_FooWithoutCachedBar);

BENCHMARK_MAIN();
