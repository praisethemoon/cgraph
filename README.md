CCGraph
===

> C Computation Graph Library

### Dependencies:

- LAPACK: `sudo apt-get install libblas-dev liblapack-dev`
- cmake `sudo apt-get install cmake`
- probably `build-essentials` as well.

###### Currently works on linux only.

### Limitations:
- Double numbers only.

## Future work
- Graph variables (no idea how to do it yet `:(`)
- Graph as nodes
- Lua graph construction
- Derivative calculations
- GPU BLAS Implentations (cuBLAS probably)
- Analyze graph to optimize calculations i.e `A^T.xB` is three operations that can be reduced to one in BLAS.
- Graph plotting and visualization

## Dependencies included within the source code:
- Lua programming language [https://github.com/lua/lua](https://github.com/lua/lua)
- tinycthreads [https://github.com/tinycthread/tinycthread](https://github.com/tinycthread/tinycthread)
- dmt [https://github.com/rxi/dmt](https://github.com/rxi/dmt)
- map  [https://github.com/rxi/map](https://github.com/rxi/map)
- vec [https://github.com/rxi/dmt](https://github.com/rxi/dmt)
- smallprofiler [https://github.com/realbogart/smallprofiler](https://github.com/realbogart/smallprofiler)
