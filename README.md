# edb

An in-memory word dictionary.

## Requirements

* C++ Compiler w/ support for C++20
* Intel oneAPI TBB 2021.3.0+
* Python 3.8+

## Quick Start

On macOS with Homebrew:

```bash
brew install python3 tbb
```

On Arch Linux with Pacman:

```bash
pacman -Syu python tbb
```

## Quick Start

Use the included `waf` build system:

```bash
./waf configure
./waf build
```

After the build completes you should have the following binaries available:

* `build/pkg/dictdb/dictdb`
* `build/pkg/dict/dict`

TODO: socket file

Run the server process in the background:

```bash
export PATH=$PATH:$(pwd)/build/pkg/dictdb/
dictdb &
```

Run the client to insert, search, and delete words:

```bash
export PATH=$PATH:$(pwd)/build/pkg/dict/
dict insert foo
dict search foo
dict delete foo
```

## Benchmark

Run the benchmarking script:

```bash
python3 ./benchmark.py -i 100000
```

## References

Tools:

* [waf](https://waf.io/book/)

Libraries:

* [Intel oneAPI Threading Building Blocks](https://github.com/oneapi-src/oneTBB)
  * https://spec.oneapi.io/versions/latest/elements/oneTBB/source/nested-index.html

Articles:

* http://beej.us/guide/bgipc/html/single/bgipc.html
* https://momjian.us/main/blogs/pgblog/2012.html#June_6_2012
* https://attractivechaos.wordpress.com/2018/08/31/a-survey-of-argument-parsing-libraries-in-c-c/
* https://gcc.gnu.org/projects/cxx-status.html
* https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
* https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
* https://stackoverflow.com/a/612476/937006
