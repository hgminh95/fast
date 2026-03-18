#include <benchmark/benchmark.h>

#include <cmath>

// pow() runtime varies depending on input values.
// When the result is a subnormal (denormalized) number, pow takes a slow path.

static void BM_PowNormal(benchmark::State& state) {
  volatile double x = 7.3;
  volatile double y = 4.1;

  for (auto _ : state) {
    double sum = 0;
    for (int i = 0; i < 1'000'000; ++i) {
      sum += std::pow(x, y);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_PowSlow(benchmark::State& state) {
  // Result is subnormal: triggers slow path in pow
  volatile double x = 1e-300;
  volatile double y = 1.1;

  for (auto _ : state) {
    double sum = 0;
    for (int i = 0; i < 1'000'000; ++i) {
      sum += std::pow(x, y);
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_PowNormal);
BENCHMARK(BM_PowSlow);

BENCHMARK_MAIN();
