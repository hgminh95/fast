#include <benchmark/benchmark.h>

#include <thread>
#include <atomic>
#include <chrono>

struct Adder {
  static inline int a{0};

  int AddToA1(int x) {
    a += x;
    return a;
  }

  int AddToA2(int x) {
    static int a{0};
    a += x;
    return a;
  }
};

static void BM_StaticMemberVariable(benchmark::State& state) {
  for (auto _ : state) {
    Adder a;
    for (int i = 0; i < 1'000'000; i += 2) {
      benchmark::DoNotOptimize(a.AddToA1(i));
      a.AddToA1(i + 1);
    }
    benchmark::DoNotOptimize(a.AddToA1(0));
  }
}

static void BM_StaticFunctionVariable(benchmark::State& state) {
  for (auto _ : state) {
    Adder a;
    for (int i = 0; i < 1'000'000; i += 2) {
      benchmark::DoNotOptimize(a.AddToA2(i));
      a.AddToA1(i + 1);
    }
  }
}

BENCHMARK(BM_StaticMemberVariable);
BENCHMARK(BM_StaticFunctionVariable);

BENCHMARK_MAIN();
