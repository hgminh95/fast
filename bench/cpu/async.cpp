#include <benchmark/benchmark.h>

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

constexpr int N = 100'000;

static std::vector<int> makeChain(uint64_t seed, int size) {
  std::vector<int> arr(size);
  std::vector<int> indices(size);
  std::iota(indices.begin(), indices.end(), 0);
  std::mt19937 rng(seed);
  std::shuffle(indices.begin(), indices.end(), rng);
  for (int i = 0; i < size - 1; ++i) {
    arr[indices[i]] = indices[i + 1];
  }
  arr[indices[size - 1]] = indices[0];
  return arr;
}

// One chain of N pointer chases (serialized, one cache miss at a time)
static void BM_OneChain(benchmark::State& state) {
  auto arr = makeChain(42, N);

  for (auto _ : state) {
    int sum = 0;
    int p = 0;
    for (int i = 0; i < N; ++i) {
      p = arr[p];
      sum += p;
    }
    benchmark::DoNotOptimize(sum);
  }
}

// Two chains of N/2 pointer chases each (interleaved, two cache misses in flight)
static void BM_TwoChains(benchmark::State& state) {
  auto arr1 = makeChain(42, N / 2);
  auto arr2 = makeChain(123, N / 2);

  for (auto _ : state) {
    int sum = 0;
    int p1 = 0, p2 = 0;
    for (int i = 0; i < N / 2; ++i) {
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
