#include <benchmark/benchmark.h>

#include <thread>
#include <atomic>
#include <chrono>

int CalcSum(int N) {
  int sum{0};
  for (int i = 0; i < N; ++i) {
    sum += i * i;
  }

  return sum;
}

unsigned CalcSumUnsigned(unsigned N) {
  unsigned sum{0};
  for (unsigned i = 0; i < N; ++i) {
    sum += i * i;
  }

  return sum;
}

static void BM_Signed(benchmark::State& state) {
  for (auto _ : state) {
    for (int i = 0; i < 1'000'000; ++i) {
      benchmark::DoNotOptimize(CalcSum(i));
    }
  }
}

static void BM_Unsigned(benchmark::State& state) {
  for (auto _ : state) {
    for (unsigned i = 0; i < 1'000'000; ++i) {
      benchmark::DoNotOptimize(CalcSumUnsigned(i));
    }
  }
}

BENCHMARK(BM_Signed);
BENCHMARK(BM_Unsigned);

BENCHMARK_MAIN();
