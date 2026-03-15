#include <benchmark/benchmark.h>

#include <algorithm>

std::vector<int> MakeArr() {
  srand(time(NULL));

  std::vector<int> arr(1'000'000);
  for (auto& x : arr) {
    x = rand() % 256;
  }

  return arr;
}

// Force a real branch by making the body opaque to the optimizer
static void BM_UnsortedArray(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      if (arr[i] > 128) {
        asm volatile("" ::: "memory");
        sum += arr[i];
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_UnsortedArrayNoBranch(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[i] * (arr[i] > 128);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_UnsortedArrayNoBranch2(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum[2] = {0, 0};
    for (auto i = 0u; i < arr.size(); ++i) {
      sum[arr[i] > 128] += arr[i];
    }
    benchmark::DoNotOptimize(sum[true]);
  }
}

static void BM_SortedArray(benchmark::State& state) {
  auto arr = MakeArr();
  std::sort(arr.begin(), arr.end());

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      if (arr[i] > 128) {
        asm volatile("" ::: "memory");
        sum += arr[i];
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_UnsortedArray);
BENCHMARK(BM_UnsortedArrayNoBranch);
BENCHMARK(BM_UnsortedArrayNoBranch2);
BENCHMARK(BM_SortedArray);

BENCHMARK_MAIN();
