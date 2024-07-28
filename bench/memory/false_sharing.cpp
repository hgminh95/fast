#include <benchmark/benchmark.h>

#include <thread>
#include <vector>

struct WithoutAlignment {
  int x;
  int y;
  int z;
};

struct alignas(64) WithAlignment {
  int x;
  int y;
  int z;
};

template <typename T>
static void BM_FalseSharing(benchmark::State& state) {
  std::vector<T> arr(100'000);
  std::vector<std::thread> ts;

  for (auto _ : state) {
    ts.clear();

    for (int i = 0; i < 2; ++i) {
      ts.emplace_back([&arr, tid = i]() {
        for (int i = tid; i < arr.size(); i += 2) {
          benchmark::DoNotOptimize(arr[i].x = i);
          benchmark::DoNotOptimize(arr[i].y = arr.size() - i);
          benchmark::DoNotOptimize(arr[i].z = arr.size() + i);
        }
      });
    }
    
    ts[0].join();
    ts[1].join();
  }
}

BENCHMARK(BM_FalseSharing<WithoutAlignment>);
BENCHMARK(BM_FalseSharing<WithAlignment>);

BENCHMARK_MAIN();
