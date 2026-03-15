#include <benchmark/benchmark.h>

struct TriviallyCopyable {
  int x;
  int y;
};

struct NonTriviallyCopyable {
  int x;
  int y;
  NonTriviallyCopyable(int a, int b) : x(a), y(b) {}
  NonTriviallyCopyable(const NonTriviallyCopyable& o) : x(o.x), y(o.y) {}
  ~NonTriviallyCopyable() {}
};

__attribute__((noinline)) TriviallyCopyable MakeTrivial(int a, int b) { return {a + 1, b + 2}; }

__attribute__((noinline)) NonTriviallyCopyable MakeNonTrivial(int a, int b) {
  return NonTriviallyCopyable(a + 1, b + 2);
}

static void BM_TrivialReturn(benchmark::State& state) {
  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < 100'000; ++i) {
      auto result = MakeTrivial(i, i + 1);
      benchmark::DoNotOptimize(result);
      sum += result.x + result.y;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_NonTrivialReturn(benchmark::State& state) {
  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < 100'000; ++i) {
      auto result = MakeNonTrivial(i, i + 1);
      benchmark::DoNotOptimize(result);
      sum += result.x + result.y;
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_TrivialReturn);
BENCHMARK(BM_NonTrivialReturn);

BENCHMARK_MAIN();
