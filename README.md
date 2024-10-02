# fast

[whichisfaster.dev](https://whichisfaster.dev)

## Requirements

For the website:

- Python 3.9 or above
- Everything in requirements.txt

For the benchmark:

- Bazel 7
- C++ compiler that supports C++23

## Usage

To build the website,

```
$ python3 gen.py --watch
```

The static files will be generated at `_build`. You can use `python3 -m http.server` to serve these static files.

To run the benchmark,

```
$ cd bench
$ bazel run --config opt memory:tlb_shootdown
...
----------------------------------------------------------
Benchmark                Time             CPU   Iterations
----------------------------------------------------------
BM_SingleThread    7807172 ns      7790222 ns           90
BM_MultiThread      511640 ns       509363 ns         1373
```

**NOTE**: Most benchmark are not properly vested at the moment, the number could be misleading.

## Contributions

- Add the website page templates are at [src/](src/)
- All the data (questions, machine specs) is at [src/data.yml](src/data.yml)
- All the benchmark codes are at [bench/](bench/)

## License

MIT
