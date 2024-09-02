#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

#include <immintrin.h>

static void BM_Write(benchmark::State& state) {
  auto arr = MakeArr<uint8_t>(1'000'000);

  for (auto _ : state) {
    for (auto i = 0; i < 1'000; ++i) {
      arr[rand() % arr.size()] = i;
    }
  }
}

static void BM_WriteMore(benchmark::State& state) {
  auto arr = MakeArr<uint64_t>(1'000'000);

  for (auto _ : state) {
    for (auto i = 0; i < 1'000; ++i) {
      arr[rand() % arr.size()] = i;
    }
  }
}

static void BM_WriteMoore(benchmark::State& state) {
  struct Foo {
    uint64_t values[8];
  };
  auto arr = MakeArr<Foo>(1'000'000);

  for (auto _ : state) {
    for (auto i = 0; i < 1'000; ++i) {
      auto& elem = arr[rand() % arr.size()];
      elem.values[0] = i;
      elem.values[1] = i;
      elem.values[2] = i;
      elem.values[3] = i;
      elem.values[4] = i;
      elem.values[5] = i;
      elem.values[6] = i;
      elem.values[7] = i;
    }
  }
}

static void BM_NormalWrite(benchmark::State& state) {
  auto arr = MakeArr<int64_t>(1'000'000);

  for (auto _ : state) {
    for (auto i = 0u; i < arr.size(); ++i) {
      arr[i] == rand() % 5;
    }
  }

  for (auto i = 0u; i < arr.size(); ++i) {
    benchmark::DoNotOptimize(arr[i]);
  }
}
static void BM_NonTemporalWrite(benchmark::State& state) {
  auto arr = MakeArr<int64_t>(1'000'000);

  for (auto _ : state) {
    for (auto i = 0u; i < arr.size(); ++i) {
      _mm_stream_si64(reinterpret_cast<long long int*>(&arr[i]), rand() % 5);
    }
  }

  for (auto i = 0u; i < arr.size(); ++i) {
    benchmark::DoNotOptimize(arr[i]);
  }
}

BENCHMARK(BM_Write);
BENCHMARK(BM_WriteMore);
BENCHMARK(BM_WriteMoore);
BENCHMARK(BM_NormalWrite);
BENCHMARK(BM_NonTemporalWrite);

BENCHMARK_MAIN();
