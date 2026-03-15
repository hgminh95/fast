#include <benchmark/benchmark.h>

#include <cmath>

static void BM_PowInteger(benchmark::State& state) {
  for (auto _ : state) {
    double sum = 0;
    for (int i = 1; i < 100'000; ++i) {
      sum += std::pow(static_cast<double>(i), 4.0);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_PowNonInteger(benchmark::State& state) {
  for (auto _ : state) {
    double sum = 0;
    for (int i = 1; i < 100'000; ++i) {
      sum += std::pow(static_cast<double>(i), 4.5);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_ManualPow4(benchmark::State& state) {
  for (auto _ : state) {
    double sum = 0;
    for (int i = 1; i < 100'000; ++i) {
      double x = static_cast<double>(i);
      double x2 = x * x;
      sum += x2 * x2;
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_PowInteger);
BENCHMARK(BM_PowNonInteger);
BENCHMARK(BM_ManualPow4);

BENCHMARK_MAIN();
