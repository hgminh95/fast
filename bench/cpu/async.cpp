#include <benchmark/benchmark.h>

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

static void BM_OneChain(benchmark::State& state) {
  constexpr int N = 100'000;
  std::vector<int> arr(N);

  std::vector<int> indices(N);
  std::iota(indices.begin(), indices.end(), 0);
  std::mt19937 rng(42);
  std::shuffle(indices.begin(), indices.end(), rng);

  for (int i = 0; i < N - 1; ++i) {
    arr[indices[i]] = indices[i + 1];
  }
  arr[indices[N - 1]] = indices[0];

  for (auto _ : state) {
    int sum = 0;
    int p = indices[0];
    for (int i = 0; i < N; ++i) {
      p = arr[p];
      sum += p;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_TwoChains(benchmark::State& state) {
  constexpr int N = 100'000;
  std::vector<int> arr1(N), arr2(N);

  std::vector<int> idx1(N), idx2(N);
  std::iota(idx1.begin(), idx1.end(), 0);
  std::iota(idx2.begin(), idx2.end(), 0);
  std::mt19937 rng1(42), rng2(123);
  std::shuffle(idx1.begin(), idx1.end(), rng1);
  std::shuffle(idx2.begin(), idx2.end(), rng2);

  for (int i = 0; i < N - 1; ++i) {
    arr1[idx1[i]] = idx1[i + 1];
    arr2[idx2[i]] = idx2[i + 1];
  }
  arr1[idx1[N - 1]] = idx1[0];
  arr2[idx2[N - 1]] = idx2[0];

  for (auto _ : state) {
    int sum = 0;
    int p1 = idx1[0], p2 = idx2[0];
    for (int i = 0; i < N; ++i) {
      p1 = arr1[p1];
      p2 = arr2[p2];
      sum += p1 + p2;
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_OneChain);
BENCHMARK(BM_TwoChains);

BENCHMARK_MAIN();
