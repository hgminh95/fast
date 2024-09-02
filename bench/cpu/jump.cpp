#include <benchmark/benchmark.h>

template <size_t NopCount>
void Nop() {
  asm("nop");
  if constexpr (NopCount > 1) {
    Nop<NopCount - 1>();
  }
}

template <size_t NopCount>
static void BM_Jump(benchmark::State& state) {
  for (auto _ : state) {
    for (auto i = 0u; i < 100'000; ++i) {
      Nop<NopCount>();
      benchmark::DoNotOptimize(i);
    }
  }
}

BENCHMARK(BM_Jump<1>);
BENCHMARK(BM_Jump<2>);
BENCHMARK(BM_Jump<3>);
BENCHMARK(BM_Jump<4>);
BENCHMARK(BM_Jump<5>);
BENCHMARK(BM_Jump<6>);
BENCHMARK(BM_Jump<7>);
BENCHMARK(BM_Jump<8>);
BENCHMARK(BM_Jump<9>);
BENCHMARK(BM_Jump<10>);
BENCHMARK(BM_Jump<11>);
BENCHMARK(BM_Jump<12>);
BENCHMARK(BM_Jump<13>);
BENCHMARK(BM_Jump<14>);
BENCHMARK(BM_Jump<15>);
BENCHMARK(BM_Jump<16>);
BENCHMARK(BM_Jump<17>);
BENCHMARK(BM_Jump<18>);
BENCHMARK(BM_Jump<19>);
BENCHMARK(BM_Jump<20>);
BENCHMARK(BM_Jump<21>);
BENCHMARK(BM_Jump<22>);
BENCHMARK(BM_Jump<23>);
BENCHMARK(BM_Jump<24>);
BENCHMARK(BM_Jump<25>);
BENCHMARK(BM_Jump<26>);
BENCHMARK(BM_Jump<27>);
BENCHMARK(BM_Jump<28>);
BENCHMARK(BM_Jump<29>);
BENCHMARK(BM_Jump<30>);
BENCHMARK(BM_Jump<31>);
BENCHMARK(BM_Jump<32>);

BENCHMARK_MAIN();
