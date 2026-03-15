#include <benchmark/benchmark.h>

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

struct Node {
  int value;
  Node* next;
};

static void BM_LinkedList(benchmark::State& state) {
  constexpr int N = 100'000;
  std::vector<Node> nodes(N);

  std::vector<int> indices(N);
  std::iota(indices.begin(), indices.end(), 0);
  std::mt19937 rng(42);
  std::shuffle(indices.begin(), indices.end(), rng);

  for (int i = 0; i < N - 1; ++i) {
    nodes[indices[i]].value = indices[i];
    nodes[indices[i]].next = &nodes[indices[i + 1]];
  }
  nodes[indices[N - 1]].value = indices[N - 1];
  nodes[indices[N - 1]].next = nullptr;

  Node* head = &nodes[indices[0]];

  for (auto _ : state) {
    int sum = 0;
    Node* p = head;
    while (p) {
      sum += p->value;
      p = p->next;
    }
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_ArrayTraversal(benchmark::State& state) {
  constexpr int N = 100'000;
  std::vector<int> arr(N);
  std::iota(arr.begin(), arr.end(), 0);

  for (auto _ : state) {
    int sum = 0;
    for (auto i = 0u; i < arr.size(); ++i) {
      sum += arr[i];
    }
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_LinkedList);
BENCHMARK(BM_ArrayTraversal);

BENCHMARK_MAIN();
