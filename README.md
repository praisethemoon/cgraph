CCGraph
===

> C Computation Graph Library

### About

CCGraph, short for C Computation Graph is a C library for building Tensor graphs.
It will support a Lua API in the future.

### Optimizations
Uses BLAS for complex operations.

The current version focuses on clear code rather than highly performant.
Once everything is well tested, hard optimizations such as switch statements and others will be improved.

### Dependencies:

- LAPACK: `sudo apt-get install libblas-dev liblapack-dev`
- cmake `sudo apt-get install cmake`
- probably `build-essentials` as well.

###### Notes

- Currently tested only on ubuntu 16.04
- Matrices are by default Row major.

### Limitations:
- Double numbers only.

### Future work
- Graph variables (no idea how to do it yet `:(`)
- Graph as nodes
- Lua API for graph construction
- Derivative calculations
- GPU BLAS Implentations (cuBLAS probably)
- Analyze graph to optimize calculations i.e `A^T.xB` is three operations that can be reduced to one in BLAS.
- Graph plotting and visualization
- Switch to LuaJIT

### Dependencies included within the source code:
- Lua programming language [https://github.com/lua/lua](https://github.com/lua/lua)
- tinycthreads [https://github.com/tinycthread/tinycthread](https://github.com/tinycthread/tinycthread)
- dmt [https://github.com/rxi/dmt](https://github.com/rxi/dmt)
- map  [https://github.com/rxi/map](https://github.com/rxi/map)
- vec [https://github.com/rxi/dmt](https://github.com/rxi/dmt)
- smallprofiler [https://github.com/realbogart/smallprofiler](https://github.com/realbogart/smallprofiler)
