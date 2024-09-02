#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <mutex>
#include <atomic>

static void BM_SumWithLock(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    std::vector<std::thread> threads;
    int sum = 0;
    std::mutex m;

    for (int i = 0; i < 4; ++i) {
      threads.emplace_back([&arr, i, &m, &sum]() {
        int partial_sum = 0;
        auto start = i * arr.size() / 4;
        auto end = (i + 1) * arr.size() / 4;
        for (int i = start; i < end; ++i) {
          partial_sum += arr[i];
        }

        std::lock_guard<std::mutex> lock(m);
        sum += partial_sum;
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }
  }
}

static void BM_SumWithAtomic(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    std::vector<std::thread> threads;
    std::atomic<int> sum = 0;

    for (int i = 0; i < 4; ++i) {
      threads.emplace_back([&arr, i, &sum]() {
        int partial_sum = 0;
        auto start = i * arr.size() / 4;
        auto end = (i + 1) * arr.size() / 4;
        for (int i = start; i < end; ++i) {
          partial_sum += arr[i];
        }

        sum += partial_sum;
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }
  }
}

static void BM_SumWithNaiveAtomic(benchmark::State& state) {
  auto arr = MakeArr(1'000'000);

  for (auto _ : state) {
    std::vector<std::thread> threads;
    std::atomic<int> sum = 0;

    for (int i = 0; i < 4; ++i) {
      threads.emplace_back([&arr, i, &sum]() {
        auto start = i * arr.size() / 4;
        auto end = (i + 1) * arr.size() / 4;
        for (int i = start; i < end; ++i) {
          sum += arr[i];
        }
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }
  }
}

BENCHMARK(BM_SumWithLock);
BENCHMARK(BM_SumWithAtomic);
BENCHMARK(BM_SumWithNaiveAtomic);

BENCHMARK_MAIN();
