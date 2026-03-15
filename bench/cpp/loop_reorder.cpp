#include <benchmark/benchmark.h>

#include <vector>

constexpr int ROWS = 1000;
constexpr int COLS = 1000;

static void BM_RowMajor(benchmark::State& state) {
  std::vector<std::vector<int>> matrix(ROWS, std::vector<int>(COLS, 1));

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < ROWS; ++i) {
      for (int j = 0; j < COLS; ++j) {
        sum += matrix[i][j];
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_ColumnMajor(benchmark::State& state) {
  std::vector<std::vector<int>> matrix(ROWS, std::vector<int>(COLS, 1));

  for (auto _ : state) {
    int sum = 0;
    for (int j = 0; j < COLS; ++j) {
      for (int i = 0; i < ROWS; ++i) {
        sum += matrix[i][j];
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_RowMajor);
BENCHMARK(BM_ColumnMajor);

BENCHMARK_MAIN();
