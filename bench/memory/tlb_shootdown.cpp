#include <benchmark/benchmark.h>

#include <thread>
#include <atomic>
#include <chrono>

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

BENCHMARK(BM_SingleThread);
BENCHMARK(BM_MultiThread);

BENCHMARK_MAIN();
