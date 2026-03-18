#include <benchmark/benchmark.h>
#include <sys/mman.h>

#include <algorithm>
#include <cstring>
#include <numeric>
#include <random>
#include <vector>

constexpr size_t N = 100'000'000;
constexpr size_t NBYTES = N * sizeof(int);

static void BM_MinorPageFault(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    // mmap without populating — pages will fault on first access
    int* arr = static_cast<int*>(
        mmap(nullptr, NBYTES, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    // Disable transparent huge pages to make faults more visible
    madvise(arr, NBYTES, MADV_NOHUGEPAGE);
    state.ResumeTiming();

    for (size_t i = 0; i < N; ++i) {
      arr[i] = static_cast<int>(i);
    }
    benchmark::DoNotOptimize(arr);

    state.PauseTiming();
    munmap(arr, NBYTES);
    state.ResumeTiming();
  }
}

static void BM_NoMinorPageFault(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    // mmap + pre-fault all pages
    int* arr = static_cast<int*>(
        mmap(nullptr, NBYTES, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    madvise(arr, NBYTES, MADV_NOHUGEPAGE);
    memset(arr, 0, NBYTES);  // trigger all page faults upfront
    state.ResumeTiming();

    for (size_t i = 0; i < N; ++i) {
      arr[i] = static_cast<int>(i);
    }
    benchmark::DoNotOptimize(arr);

    state.PauseTiming();
    munmap(arr, NBYTES);
    state.ResumeTiming();
  }
}

BENCHMARK(BM_MinorPageFault);
BENCHMARK(BM_NoMinorPageFault);

BENCHMARK_MAIN();
