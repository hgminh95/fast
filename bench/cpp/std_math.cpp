#include <benchmark/benchmark.h>

#include <cmath>
#include <utility>
#include <array>

double sum{0};

std::array<double, 8> values{
  1.0000000000000020, 1.5000050000000000,
  1.0000000000000020, 1.5000005000000000,
  1.0000000000000020, 1.5000000000000000,
  0.9999999999999940, 0.4166666666666666,
};

static void BM_Pow(benchmark::State& state) {
  for (auto _ : state) {
    sum += std::powl(values[2 * state.range(0)], values[2 * state.range(0) + 1]);
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_Pow)->DenseRange(0, 3, 1);

BENCHMARK_MAIN();
