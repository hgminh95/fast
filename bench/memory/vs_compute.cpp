#include <benchmark/benchmark.h>

#include <cstdlib>
#include <vector>

struct WithCache {
  int input;
  int cached_result;
};

struct WithoutCache {
  int input;
};

inline int compute(int x) {
  x = x * 7 + 13;
  x = x ^ (x >> 3);
  x = x * 31 + 7;
  x = x ^ (x >> 5);
  x = x * 127 + 63;
  x = x ^ (x >> 7);
  x = x * 17 + 5;
  return x;
}

static void BM_ReadCached(benchmark::State& state) {
  constexpr int N = 10'000'000;
  std::vector<WithCache> arr(N);
  srand(42);
  for (auto& e : arr) {
    e.input = rand() % 1000;
    e.cached_result = compute(e.input);
  }

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < N; ++i) {
      sum += arr[i].cached_result;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_Recompute(benchmark::State& state) {
  constexpr int N = 10'000'000;
  std::vector<WithoutCache> arr(N);
  srand(42);
  for (auto& e : arr) {
    e.input = rand() % 1000;
  }

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < N; ++i) {
      sum += compute(arr[i].input);
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_ReadCached);
BENCHMARK(BM_Recompute);

BENCHMARK_MAIN();
