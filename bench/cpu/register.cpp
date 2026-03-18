#include <benchmark/benchmark.h>

#include <cstdlib>
#include <vector>

// N independent chains using double (SSE/AVX registers).
// x86-64 has 16 xmm registers. When N exceeds that, values spill to stack.
template <size_t N>
__attribute__((optimize("no-tree-vectorize"))) static void BM_Register(benchmark::State& state) {
  std::vector<double> arr(1'000'000);
  srand(42);
  for (auto& x : arr) x = rand() / (double)RAND_MAX;

  for (auto _ : state) {
    double sums[N];
    for (size_t k = 0; k < N; ++k) sums[k] = 1.0;

    for (size_t i = 0; i + N <= arr.size(); i += N) {
      for (size_t j = 0; j < N; ++j) {
        sums[j] = sums[j] * 1.01 + arr[i + j];
        asm volatile("" : "+x"(sums[j]));
      }
    }

    double total = 0;
    for (size_t k = 0; k < N; ++k) total += sums[k];
    benchmark::DoNotOptimize(total);
  }
}

BENCHMARK(BM_Register<14>);
BENCHMARK(BM_Register<16>);

BENCHMARK_MAIN();
