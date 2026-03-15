#include <benchmark/benchmark.h>

#include <cstring>
#include <vector>

static void BM_AlignedAccess(benchmark::State& state) {
  std::vector<int64_t> arr(1'000'000, 42);

  for (auto _ : state) {
    int64_t sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[i];
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_MisalignedAccess(benchmark::State& state) {
  constexpr int N = 1'000'000;
  std::vector<char> buf(N * sizeof(int64_t) + 3);
  char* p = buf.data() + 3;

  int64_t val = 42;
  for (int i = 0; i < N; ++i) {
    memcpy(p + i * sizeof(int64_t), &val, sizeof(int64_t));
  }

  for (auto _ : state) {
    int64_t sum = 0;
    for (int i = 0; i < N; ++i) {
      int64_t v;
      memcpy(&v, p + i * sizeof(int64_t), sizeof(int64_t));
      sum += v;
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_AlignedAccess);
BENCHMARK(BM_MisalignedAccess);

BENCHMARK_MAIN();
