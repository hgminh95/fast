#include <benchmark/benchmark.h>

#include <atomic>
#include <thread>
#include <vector>

static void BM_TrueSharing(benchmark::State& state) {
  for (auto _ : state) {
    std::atomic<int> counter{0};
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
      threads.emplace_back([&counter]() {
        for (int i = 0; i < 100'000; ++i) {
          counter.fetch_add(1, std::memory_order_relaxed);
        }
      });
    }
    for (auto& t : threads) t.join();
    benchmark::DoNotOptimize(counter.load());
  }
}

static void BM_NoTrueSharing(benchmark::State& state) {
  for (auto _ : state) {
    std::atomic<int> counter{0};
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
      threads.emplace_back([&counter]() {
        int local = 0;
        for (int i = 0; i < 100'000; ++i) {
          local++;
        }
        counter.fetch_add(local, std::memory_order_relaxed);
      });
    }
    for (auto& t : threads) t.join();
    benchmark::DoNotOptimize(counter.load());
  }
}

BENCHMARK(BM_TrueSharing);
BENCHMARK(BM_NoTrueSharing);

BENCHMARK_MAIN();
