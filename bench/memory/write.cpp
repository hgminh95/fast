#include <benchmark/benchmark.h>
#include <immintrin.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <thread>

#include "common/array.h"

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

struct alignas(64) CacheLine {
  char data[64];
};

static void BM_PartialCacheLineWrite(benchmark::State& state) {
  constexpr int N = 1'000'000;
  std::vector<CacheLine> arr(N);

  for (auto _ : state) {
    for (int i = 0; i < N; ++i) {
      arr[i].data[0] = static_cast<char>(i);
    }
    benchmark::DoNotOptimize(arr.data());
  }
}

static void BM_FullCacheLineWrite(benchmark::State& state) {
  constexpr int N = 1'000'000;
  std::vector<CacheLine> arr(N);

  for (auto _ : state) {
    for (int i = 0; i < N; ++i) {
      memset(&arr[i], static_cast<char>(i), 64);
    }
    benchmark::DoNotOptimize(arr.data());
  }
}

static void BM_NormalWriteSSE(benchmark::State& state) {
  constexpr size_t N = 20'000'000;
  auto arr = MakeArr<int64_t>(N);
  __m128i val = _mm_set1_epi64x(42);

  for (auto _ : state) {
    __m128i* p = reinterpret_cast<__m128i*>(arr.data());
    for (size_t i = 0; i < N / 2; ++i) {
      _mm_store_si128(p + i, val);
    }
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * N * sizeof(int64_t));
}

static void BM_NonTemporalWriteSSE(benchmark::State& state) {
  constexpr size_t N = 20'000'000;
  auto arr = MakeArr<int64_t>(N);
  __m128i val = _mm_set1_epi64x(42);

  for (auto _ : state) {
    __m128i* p = reinterpret_cast<__m128i*>(arr.data());
    for (size_t i = 0; i < N / 2; ++i) {
      _mm_stream_si128(p + i, val);
    }
    _mm_sfence();
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * N * sizeof(int64_t));
}

BENCHMARK(BM_Write);
BENCHMARK(BM_WriteMore);
BENCHMARK(BM_WriteMoore);
BENCHMARK(BM_PartialCacheLineWrite);
BENCHMARK(BM_FullCacheLineWrite);
BENCHMARK(BM_NormalWriteSSE);
BENCHMARK(BM_NonTemporalWriteSSE);

BENCHMARK_MAIN();
