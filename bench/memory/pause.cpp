#include <benchmark/benchmark.h>

#include <atomic>
#include <thread>

std::atomic<bool> lock_flag{false};

static void BM_SpinlockWithPause(benchmark::State& state) {
  for (auto _ : state) {
    for (int i = 0; i < 100'000; ++i) {
      while (lock_flag.exchange(true, std::memory_order_acquire)) {
#if defined(__x86_64__) || defined(_M_X64)
        __builtin_ia32_pause();
#elif defined(__aarch64__)
        asm volatile("yield");
#endif
      }
      lock_flag.store(false, std::memory_order_release);
    }
  }
}

static void BM_SpinlockWithoutPause(benchmark::State& state) {
  for (auto _ : state) {
    for (int i = 0; i < 100'000; ++i) {
      while (lock_flag.exchange(true, std::memory_order_acquire)) {
        // busy spin without pause
      }
      lock_flag.store(false, std::memory_order_release);
    }
  }
}

BENCHMARK(BM_SpinlockWithPause);
BENCHMARK(BM_SpinlockWithoutPause);

BENCHMARK_MAIN();
