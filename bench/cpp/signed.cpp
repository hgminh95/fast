#include <benchmark/benchmark.h>

#include <thread>
#include <atomic>
#include <chrono>

int CalcSum(int N) {
  int sum{0};
  for (int i = 100; i < N; ++i) {
    sum += i * i;
  }

  return sum;
}

unsigned CalcSumUnsigned(unsigned N) {
  unsigned sum{0};
  for (unsigned i = 100; i < N; ++i) {
    sum += i * i;
  }

  return sum;
}

static void BM_Signed(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(CalcSum(state.range(0)));
  }
}

static void BM_Unsigned(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(CalcSumUnsigned(state.range(0)));
  }
}

BENCHMARK(BM_Signed)->Range(1, 1 << 20);
BENCHMARK(BM_Unsigned)->Range(1, 1 << 20);

BENCHMARK_MAIN();
