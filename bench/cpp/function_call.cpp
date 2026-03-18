#include <benchmark/benchmark.h>

#include <memory>
#include <random>
#include <vector>

struct Base {
  virtual int Compute(int a, int b) noexcept = 0;
  virtual ~Base() = default;
};

struct Impl1 : public Base {
  __attribute__((noinline)) int Compute(int a, int b) noexcept override {
    return (a * 3 + b * 7) ^ (a - b);
  }
};

struct Impl2 : public Base {
  __attribute__((noinline)) int Compute(int a, int b) noexcept override {
    return (a * 3 + b * 7) ^ (a - b);
  }
};

__attribute__((noinline)) int ComputeFn(int a, int b) noexcept { return (a * 3 + b * 7) ^ (a - b); }

__attribute__((always_inline)) inline int ComputeInline(int a, int b) {
  return (a * 3 + b * 7) ^ (a - b);
}

static void BM_InlineFunction(benchmark::State& state) {
  std::vector<int> arr(1'000'000);
  std::mt19937 rng(42);
  for (auto& x : arr) x = rng() % 256;

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum = ComputeInline(sum, arr[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_Function(benchmark::State& state) {
  std::vector<int> arr(1'000'000);
  std::mt19937 rng(42);
  for (auto& x : arr) x = rng() % 256;

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum = ComputeFn(sum, arr[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_VirtualFunction(benchmark::State& state) {
  std::vector<int> arr(1'000'000);
  std::mt19937 rng(42);
  for (auto& x : arr) x = rng() % 256;

  // Randomly mix two derived types to prevent branch prediction of vtable
  std::vector<std::unique_ptr<Base>> callers(64);
  for (size_t i = 0; i < callers.size(); ++i) {
    if (rng() % 2)
      callers[i] = std::make_unique<Impl1>();
    else
      callers[i] = std::make_unique<Impl2>();
  }

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum = callers[i % callers.size()]->Compute(sum, arr[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_InlineFunction);
BENCHMARK(BM_Function);
BENCHMARK(BM_VirtualFunction);

BENCHMARK_MAIN();
