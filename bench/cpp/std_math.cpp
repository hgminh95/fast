#include <cmath>
#include <string>

// bazel run --config opt cpp:std_math -- 1.0000000000000020 1.5000050000000000
// bazel run --config opt cpp:std_math -- 1.0000000000000020 1.5000000000000000
// bazel run --config opt cpp:std_math -- 0.9999999999999940 1.4166666666666666

int main(int argc, char *argv[]) {
  double x = std::stod(argv[1]);
  double y = std::stod(argv[2]);
  double sum{0};
  for (int i = 0u; i < 1'000'000; ++i) {
    sum += std::powl(x, y) * i;
  }

  return static_cast<int>(sum * 10) % 10;
}
