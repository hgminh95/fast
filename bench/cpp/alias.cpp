#include <benchmark/benchmark.h>

__attribute__((noinline)) void Add2ElemsRestrict(int * __restrict a, int * __restrict b, int x) {
  for (int i = 0; i < 50; ++i) {
    a[i] += x;
    b[i] += x;
  }
}

__attribute__((noinline)) void Add2Elems(int *a, int *b, int x) {
  for (int i = 0; i < 50; ++i) {
    a[i] += x;
    b[i] += x;
  }
}

void BM_Restrict(benchmark::State& state) {
  int a[50]{0};
  int b[50]{0};
  for (auto _ : state) {
    Add2ElemsRestrict(a, b, rand() % 32);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
  }
}

void BM_NoRestrict(benchmark::State& state) {
  int a[50]{0};
  int b[50]{0};
  for (auto _ : state) {
    Add2Elems(a, b, rand() % 32);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
  }
}

BENCHMARK(BM_NoRestrict);
BENCHMARK(BM_Restrict);

BENCHMARK_MAIN();
