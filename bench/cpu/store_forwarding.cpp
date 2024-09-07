#include <benchmark/benchmark.h>

#include "common/array.h"

#include <atomic>

struct Foo1 {
  int32_t part1{0};
  int32_t part2{0};

  void Store(int32_t x) {
    // One of these must be correct, tested with both
    part1 = 0;
    part2 = x;

    // part2 = 0;
    // part1 = x;
  }

  int64_t Load() {
    return *reinterpret_cast<int64_t*>(this);
  }
};

struct Foo2 {
  int32_t part1{0};
  int32_t part2{0};

  void Store(int32_t x) {
    *reinterpret_cast<int64_t*>(this) = x;
  }

  int64_t Load() {
    return *reinterpret_cast<int64_t*>(this);
  }
};

template <typename Foo>
static void BM_StoreForwarding(benchmark::State& state) {
  auto arr = MakeArr<Foo>(1'000'000);
  volatile uint16_t x;

  int64_t sum{0};
  for (auto _ : state) {
    for (auto &elem : arr) {
      elem.Store(x);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      sum += elem.Load();
      benchmark::DoNotOptimize(sum);
    }
  }
}

BENCHMARK(BM_StoreForwarding<Foo1>);
BENCHMARK(BM_StoreForwarding<Foo2>);

BENCHMARK_MAIN();
