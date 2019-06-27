# kbest-intervals

Implementation of Csürös algorithm for computing the (at most) k-maximally scoring intervals.

## Prerequisites

This module uses pybind11 and therefore needs the following utilities to compile:

* A compiler with C++14 support
* CMake >= 2.8.12 (>= 3.1 on Windows)

## Installation

```
pip install ./k-best
```

## Tests

```
pytest test/
```