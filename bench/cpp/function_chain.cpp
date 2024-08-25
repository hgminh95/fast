#include <benchmark/benchmark.h>

#include "common/array.h"

#include <thread>
#include <atomic>
#include <chrono>

struct Context {
  int a;
  int b{2};
  int c{7};
  int d{8};
  int e{1};
  int f{2};

  Context() {
    a = rand() % 1000;
  }
};

void Step1(Context &ctx) {
  ctx.a += ctx.a;
  ctx.c *= ctx.a;
  ctx.b -= ctx.a;
  if (ctx.b == 0) 
    ctx.b = 1;
  ctx.f -= 100;
  ctx.a = (ctx.a + ctx.a) / ctx.b + ctx.c * ctx.d - ctx.e % ctx.f;
}

void Step2(Context &ctx) {
  ctx.c += ctx.b;
  ctx.d *= ctx.b;
  ctx.e -= ctx.b;
  if (ctx.e == 0) 
    ctx.e = 1;
  ctx.f -= 100;
  ctx.b = (ctx.b + ctx.c) / ctx.e + ctx.d * ctx.d - ctx.f % ctx.e;
}

static void BM_ChainFunction(benchmark::State& state) {
  auto contexts = MakeArr<Context>(1'000'000);

  for (auto _ : state) {
    int sum = 0;
    for (auto &context : contexts) {
      Step1(context);
    }
    for (auto &context : contexts) {
      Step2(context);
    }
    for (auto &context : contexts) {
      Step1(context);
    }
    for (auto &context : contexts) {
      Step2(context);
      sum += context.a + context.b;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_ChainFunction2(benchmark::State& state) {
  auto contexts = MakeArr<Context>(1'000'000);

  for (auto _ : state) {
    int sum = 0;
    for (auto &context : contexts) {
      Step1(context);
      Step2(context);
      Step1(context);
      Step2(context);
      sum += context.a + context.b;
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_ChainFunction);
BENCHMARK(BM_ChainFunction2);

BENCHMARK_MAIN();
