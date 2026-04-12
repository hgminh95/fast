#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <vector>
#include <x86intrin.h>

// Pointer chasing through a large array to force serialized DRAM loads.
// Each load depends on the previous, defeating prefetching.
// Array must exceed L3 cache so accesses go to DRAM.
//
// We record per-access latency via rdtsc and look for periodic spikes
// caused by DRAM refresh (tREFI ≈ 7.8µs).

static constexpr size_t ARRAY_SIZE = 64'000'000;  // 256MB, well past L3
static constexpr int SAMPLES = 200'000;

static inline uint64_t rdtsc_fenced() {
  _mm_lfence();
  uint64_t t = __rdtsc();
  _mm_lfence();
  return t;
}

int main() {
  // Build a random pointer-chasing chain through the array.
  // Each element stores the index of the next element to visit.
  std::vector<uint64_t> arr(ARRAY_SIZE);

  // Fisher-Yates shuffle to create a single Hamiltonian cycle
  // visiting every element exactly once (ensures full DRAM coverage).
  std::vector<size_t> order(ARRAY_SIZE);
  std::iota(order.begin(), order.end(), 0);
  srand(42);
  for (size_t i = ARRAY_SIZE - 1; i > 0; --i) {
    size_t j = rand() % (i + 1);
    std::swap(order[i], order[j]);
  }
  for (size_t i = 0; i < ARRAY_SIZE - 1; ++i) {
    arr[order[i]] = order[i + 1];
  }
  arr[order[ARRAY_SIZE - 1]] = order[0];  // close the cycle

  // Warmup: traverse once to fault in pages
  uint64_t idx = order[0];
  for (size_t i = 0; i < ARRAY_SIZE; ++i) {
    idx = arr[idx];
  }

  // Measure per-access latency
  std::vector<uint64_t> latencies(SAMPLES);
  for (int i = 0; i < SAMPLES; ++i) {
    uint64_t t0 = rdtsc_fenced();
    idx = arr[idx];
    uint64_t t1 = rdtsc_fenced();
    latencies[i] = t1 - t0;
  }
  // Prevent optimizing away
  if (idx == 0xDEAD) printf("%lu\n", idx);

  // Get TSC frequency estimate (cycles per µs)
  uint64_t t0 = rdtsc_fenced();
  struct timespec ts = {0, 10'000'000};  // 10ms
  nanosleep(&ts, nullptr);
  uint64_t t1 = rdtsc_fenced();
  double cycles_per_us = (double)(t1 - t0) / 10000.0;

  // Print histogram
  std::sort(latencies.begin(), latencies.end());
  uint64_t p50 = latencies[SAMPLES / 2];
  uint64_t p90 = latencies[SAMPLES * 90 / 100];
  uint64_t p99 = latencies[SAMPLES * 99 / 100];
  uint64_t p999 = latencies[SAMPLES * 999 / 1000];
  uint64_t pmax = latencies[SAMPLES - 1];

  printf("=== DRAM Refresh Latency Test ===\n");
  printf("Array: %zu MB (pointer chasing, random order)\n",
         ARRAY_SIZE * sizeof(uint64_t) / (1024 * 1024));
  printf("Samples: %d\n", SAMPLES);
  printf("TSC freq: %.0f MHz\n\n", cycles_per_us);

  printf("Latency percentiles (cycles / ns):\n");
  printf("  p50:   %4lu cyc  (%5.1f ns)\n", p50, p50 / cycles_per_us * 1000);
  printf("  p90:   %4lu cyc  (%5.1f ns)\n", p90, p90 / cycles_per_us * 1000);
  printf("  p99:   %4lu cyc  (%5.1f ns)\n", p99, p99 / cycles_per_us * 1000);
  printf("  p99.9: %4lu cyc  (%5.1f ns)\n", p999, p999 / cycles_per_us * 1000);
  printf("  max:   %4lu cyc  (%5.1f ns)\n\n", pmax, pmax / cycles_per_us * 1000);

  // Time-series: print latency every ~7.8µs worth of accesses
  // With ~70ns per DRAM access, 7800/70 ≈ 111 accesses per refresh interval
  printf("Latency time series (every access, first 500):\n");
  printf("  # access   cycles    ns\n");
  // Re-measure a small series for time-domain view
  std::vector<uint64_t> ts_latencies(500);
  for (int i = 0; i < 500; ++i) {
    uint64_t ta = rdtsc_fenced();
    idx = arr[idx];
    uint64_t tb = rdtsc_fenced();
    ts_latencies[i] = tb - ta;
  }
  if (idx == 0xDEAD) printf("%lu\n", idx);

  uint64_t threshold = p50 * 2;  // flag accesses > 2x median
  int spike_count = 0;
  for (int i = 0; i < 500; ++i) {
    const char* flag = ts_latencies[i] > threshold ? " <<<" : "";
    if (ts_latencies[i] > threshold) spike_count++;
    printf("  %4d    %6lu  %6.1f%s\n", i, ts_latencies[i],
           ts_latencies[i] / cycles_per_us * 1000, flag);
  }

  double access_ns = p50 / cycles_per_us * 1000;
  double window_accesses = 7800.0 / access_ns;
  printf("\nExpected refresh interval: ~7.8µs\n");
  printf("Median access latency: %.0fns\n", access_ns);
  printf("Accesses per refresh window: ~%.0f\n", window_accesses);
  printf("Spikes (>2x median) in 500 accesses: %d (expected ~%.0f)\n",
         spike_count, 500.0 / window_accesses);

  return 0;
}
