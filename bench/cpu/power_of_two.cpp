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

static void BM_DivideBy128(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum{0};
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[i] % 128;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_DivideBy3(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum{0};
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[i] % 3;
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_DivideBy128);
BENCHMARK(BM_DivideBy3);

BENCHMARK_MAIN();
