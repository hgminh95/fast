# fast

[whatisfaster.dev](whatisfaster.dev)

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

## Contributions

- Add the website page is stored at 
- All the data is stored at src/data.yml 
- All the benchmark codes are stored at [bench/](bench/)

## License

MIT
