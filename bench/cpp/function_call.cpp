#include <benchmark/benchmark.h>

#include <thread>
#include <atomic>
#include <chrono>

struct Base {
  virtual int AddVirt(int a, int b) = 0;
};

struct Adder : public Base {
  __attribute__((always_inline)) int AddInline(int a, int b) {
    return a + b;
  }

  __attribute__((noinline)) int Add(int a, int b) {
    return a + b;
  }

  int AddVirt(int a, int b) override {
    return a + b;
  }
};

std::vector<int> MakeArr() {
  srand(time(NULL));

  std::vector<int> arr(1'000'000);
  for (auto &x : arr) {
    x = rand() % 256;
  }

  return arr;
}

static void BM_InlineFunction(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum{0};
    Adder adder;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum = adder.AddInline(sum, arr[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_Function(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum{0};
    Adder adder;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum = adder.Add(sum, arr[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_VirtualFunction(benchmark::State& state) {
  auto arr = MakeArr();

  for (auto _ : state) {
    int sum{0};
    Base *adder = new Adder();
    for (auto i = 0u; i < arr.size(); ++i) {
      sum = adder->AddVirt(sum, arr[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_InlineFunction);
BENCHMARK(BM_Function);
BENCHMARK(BM_VirtualFunction);

BENCHMARK_MAIN();
