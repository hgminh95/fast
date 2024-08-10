#include <benchmark/benchmark.h>

#include <cmath>

static void BM_String(benchmark::State& state) {
  for (auto _ : state) {
    int cnt = 0;
    while (cnt < 22 * 23) {
      std::string s;
      for (auto i = 0u; i < state.range(0); ++i) {
        s += 'a';
        ++cnt;
      }
      benchmark::DoNotOptimize(s);
    }
  }
}

BENCHMARK(BM_String)->DenseRange(22, 23, 1);

BENCHMARK_MAIN();
