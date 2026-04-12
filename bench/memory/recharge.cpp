#include <benchmark/benchmark.h>
#include <sys/mman.h>
#include <cstdio>
#include <cstdlib>

#include "common/array.h"

// Matrix dimensions chosen so each row = one DRAM row (8KB)
static constexpr int COLS = 8192 / sizeof(int);  // 2048 ints = 8KB
static constexpr int ROWS = 31250;
static constexpr int N = ROWS * COLS;  // 64M elements = 256MB

// --- Huge page allocation (global, allocated once) ---

static int* g_huge_arr = nullptr;

static int* GetHugeArr() {
  if (g_huge_arr) return g_huge_arr;

  size_t bytes = N * sizeof(int);
  void* p = mmap(nullptr, bytes, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_2MB,
                 -1, 0);
  if (p == MAP_FAILED) {
    fprintf(stderr, "HUGETLB mmap failed, falling back to normal pages\n");
    p = mmap(nullptr, bytes, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  } else {
    fprintf(stderr, "HUGETLB mmap succeeded (%zu MB)\n", bytes / (1024*1024));
  }
  g_huge_arr = static_cast<int*>(p);
  srand(42);
  for (size_t i = 0; i < N; ++i) g_huge_arr[i] = rand() % 256;
  return g_huge_arr;
}

// --- Row-major (sequential): visits every element once ---

static void BM_SameRowAccess(benchmark::State& state) {
  auto arr = MakeArr(N);

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < ROWS; ++i)
      for (int j = 0; j < COLS; ++j)
        sum += arr[i * COLS + j];
    benchmark::DoNotOptimize(sum);
  }
}

// --- Column-major (strided): visits every element once, stride = 8KB ---

static void BM_DifferentRowAccess(benchmark::State& state) {
  auto arr = MakeArr(N);

  for (auto _ : state) {
    int sum = 0;
    for (int j = 0; j < COLS; ++j)
      for (int i = 0; i < ROWS; ++i)
        sum += arr[i * COLS + j];
    benchmark::DoNotOptimize(sum);
  }
}

// --- Huge page variants ---

static void BM_SameRowAccess_HugePage(benchmark::State& state) {
  int* arr = GetHugeArr();

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < ROWS; ++i)
      for (int j = 0; j < COLS; ++j)
        sum += arr[i * COLS + j];
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_DifferentRowAccess_HugePage(benchmark::State& state) {
  int* arr = GetHugeArr();

  for (auto _ : state) {
    int sum = 0;
    for (int j = 0; j < COLS; ++j)
      for (int i = 0; i < ROWS; ++i)
        sum += arr[i * COLS + j];
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_SameRowAccess);
BENCHMARK(BM_DifferentRowAccess);
BENCHMARK(BM_SameRowAccess_HugePage);
BENCHMARK(BM_DifferentRowAccess_HugePage);

BENCHMARK_MAIN();
