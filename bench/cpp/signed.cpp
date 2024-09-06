#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

int CalcSum(int N) {
  int sum = 0;
  for (int i = N; i < 100; ++i) {
    sum += i / 7 + i % 3;
  }
  return sum;
}

unsigned CalcSumUnsigned(unsigned N) {
  int sum = 0;
  for (unsigned i = N; i < 100; ++i) {
    sum += i / 7 + i % 3;
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

static void BM_SignedDivision(benchmark::State& state) {
  auto arr = MakeArr<int64_t>(10'000);

  for (auto _ : state) {
    int sum = 0;
    for (auto elem : arr) {
      sum += elem / 1234;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_UnsignedDivision(benchmark::State& state) {
  auto arr = MakeArr<uint64_t>(10'000);

  for (auto _ : state) {
    int sum = 0;
    for (auto elem : arr) {
      sum += elem / 1234;
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_Signed)->Range(1 << 5, 1 << 10);
BENCHMARK(BM_Unsigned)->Range(1 << 5, 1 << 10);
BENCHMARK(BM_SignedDivision);
BENCHMARK(BM_UnsignedDivision);

BENCHMARK_MAIN();
