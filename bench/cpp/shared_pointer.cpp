#include <benchmark/benchmark.h>

#include <memory>
#include <vector>

struct Foo {
  int value{42};
  int compute() { return value * 2 + 1; }
};

static void BM_UniquePtr(benchmark::State& state) {
  std::vector<std::unique_ptr<Foo>> arr;
  for (int i = 0; i < 100'000; ++i) {
    arr.push_back(std::make_unique<Foo>());
  }

  for (auto _ : state) {
    int sum = 0;
    for (auto& p : arr) {
      sum += p->compute();
    }
    benchmark::DoNotOptimize(sum);
  }
}

__attribute__((noinline)) int ProcessShared(std::shared_ptr<Foo> p) { return p->compute(); }

__attribute__((noinline)) int ProcessUnique(Foo* p) { return p->compute(); }

static void BM_SharedPtrPassByValue(benchmark::State& state) {
  auto arr = std::vector<std::shared_ptr<Foo>>();
  for (int i = 0; i < 100'000; ++i) {
    arr.push_back(std::make_shared<Foo>());
  }

  for (auto _ : state) {
    int sum = 0;
    for (auto& p : arr) {
      sum += ProcessShared(p);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_RawPtrPass(benchmark::State& state) {
  auto arr = std::vector<std::shared_ptr<Foo>>();
  for (int i = 0; i < 100'000; ++i) {
    arr.push_back(std::make_shared<Foo>());
  }

  for (auto _ : state) {
    int sum = 0;
    for (auto& p : arr) {
      sum += ProcessUnique(p.get());
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_UniquePtr);
BENCHMARK(BM_SharedPtrPassByValue);
BENCHMARK(BM_RawPtrPass);

BENCHMARK_MAIN();
