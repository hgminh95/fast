#include <benchmark/benchmark.h>

#include <thread>
#include <atomic>
#include <chrono>

std::vector<int> MakeArr() {
  srand(time(NULL));

  std::vector<int> arr(1'000'000);
  for (auto &x : arr) {
    x = rand() % 256;
  }

  return arr;
}

static void BM_UnsortedArray(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      if (arr[i] > 128)
        sum += arr[i];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_UnsortedArrayNoPredict(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      if (__builtin_expect_with_probability(arr[i] > 128, 1, 0.5))
        sum += arr[i];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_SortedArray(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    std::sort(arr.begin(), arr.end());

    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      if (arr[i] > 128)
        sum += arr[i];
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_UnsortedArray);
BENCHMARK(BM_UnsortedArrayNoPredict);
BENCHMARK(BM_SortedArray);

BENCHMARK_MAIN();
