#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

template <size_t N>
static void BM_TlbMiss(benchmark::State& state) {
  constexpr int PAGE_SIZE = 16384;
  constexpr int PAGE_SIZE_IN_INT = PAGE_SIZE / 4;
  auto arr = MakeArr(PAGE_SIZE_IN_INT * 8192 * 4);

  for (auto _ : state) {
    int sum{0};
    for (int i = 0u; i < 100'000; ++i) {
      sum += arr[rand() % (PAGE_SIZE_IN_INT * N)];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_SingleThread(benchmark::State& state) {
  std::vector<int> arr(100'000'000, 1);

  for (auto _ : state) {
    for (int i = 0u; i < arr.size() * 10; ++i) {
      benchmark::DoNotOptimize(arr[i % arr.size()]);
    }
  }
}

static void BM_MultiThread(benchmark::State& state) {
  std::vector<int> arr(100'000'000, 1);

  std::atomic<bool> active{true};
  int *x;
  std::thread t([&active, &x]() {
    int cnt = 0;
    while (active) {
      if (cnt % 2 == 0) {
        x = static_cast<int*>(malloc(4096 * 32 * 4 * 1024));
      } else {
        free(x);
      }

      ++cnt;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  for (auto _ : state) {
    for (int i = 0u; i < arr.size() * 5; ++i) {
      benchmark::DoNotOptimize(arr[i % arr.size()]);
    }
  }

  active = false;
  t.join();
}

BENCHMARK(BM_TlbMiss<1>);
BENCHMARK(BM_TlbMiss<512>);
BENCHMARK(BM_TlbMiss<1024>);
BENCHMARK(BM_TlbMiss<2048>);
BENCHMARK(BM_TlbMiss<8192>);
BENCHMARK(BM_SingleThread);
BENCHMARK(BM_MultiThread);

BENCHMARK_MAIN();
