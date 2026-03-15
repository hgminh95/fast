#include <benchmark/benchmark.h>

#include <atomic>
#include <thread>

#include "common/array.h"

static void BM_SingleThread(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < 10'000'000; ++i) {
      sum += arr[i % arr.size()];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_MultiThread(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);
  std::atomic<bool> running{true};

  std::thread allocator([&running]() {
    int* x = nullptr;
    int cnt = 0;
    while (running) {
      if (++cnt % 2 == 0)
        x = new int[10000];
      else {
        delete[] x;
        x = nullptr;
      }
    }
    delete[] x;
  });

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < 1'000'000; ++i) {
      sum += arr[i % arr.size()];
    }
    benchmark::DoNotOptimize(sum);
  }

  running = false;
  allocator.join();
}

BENCHMARK(BM_SingleThread);
BENCHMARK(BM_MultiThread);

BENCHMARK_MAIN();
