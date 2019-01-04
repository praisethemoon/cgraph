CGraph
===

![resources/logo.png](resources/logo.png)

> C Computation Graph Library

### Build Status:
[![Build Status](https://travis-ci.org/praisethemoon/cgraph.svg?branch=v1.0.a)](https://travis-ci.org/praisethemoon/cgraph)

### About

CGraph, short for C Computation Graph is a C library for building Tensor graphs.
It will support a Lua API in the future.

> You see the logo? Illuminaty confirmed

### Optimizations
Uses BLAS for complex operations.

The current version focuses on clear code rather than highly performant one.
Once everything is well tested, hard optimizations such as switch statements and others will be improved.
Also not all operations are written in blas, some uses classic for-loop, as I am still learning BLAS,
optimizations will come once the library becomes stable.

### Dependencies:

- LAPACK: `sudo apt-get install libblas-dev liblapack-dev`
- cmake `sudo apt-get install cmake`
- probably `build-essentials` as well.

###### Notes

- Currently tested only on ubuntu 16.04 LTS and Ubuntu 18.04.1 LTS (Plasma Desktop, I know it has nothing to do, just wanted to say it)
- Matrices are by default Row major
- Vectors are treated as column matrices

### Limitations:
- Double numbers only.

### C API Status:
- Memory management has been greatly improved, but needs more checking, especilly with the gradient calculation
- C API is almost stable.
- Could use more unittests

### Lua API status:
- Working but need memory improvements
- No unittest
- Easy to setup and use, but not yet reliable.
- Plotting a graph is currently outdated as more operations has been added to the library and not considered in the render function

### Example (Lua API):

```lua
local CGraph = require 'CGraph'
local array = CGraph.array


local function sigmoid(z)
	local Z = CGraph.variable("z")
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	local graph = CGraph.graph("sigmoid", sigmoid)
	graph:setVar("z", z)
	local res = graph:eval()
	graph:plot()
	return res
end

print(sigmoid(CGraph.dot( CGraph.vector(3, array {0,0,0}), CGraph.vector(3, array {0,0,0}) )))

return sigmoid
```

### Future work
- Graph variables (Done)
- Lua API for graph construction (Done)
- Derivative calculations (Done)
- Usage of BLAS in all operations (In progress)
- GPU BLAS implementation (clBLAS probably & raw OpenCL as well)
- Multithreaded implementation
- Graph plotting and visualization (Done, outdated)
- Switch to LuaJIT instead of Lua API (Must do ASAP)
- Travis CI (done)
- Valgrind to check memory (Done)
- Optimal data fetching and allocation (Lazy evaluation)

### Dependencies included within the source code:
- Lua programming language [https://github.com/lua/lua](https://github.com/lua/lua)
- tinycthreads [https://github.com/tinycthread/tinycthread](https://github.com/tinycthread/tinycthread)
- dmt [https://github.com/rxi/dmt](https://github.com/rxi/dmt)
- map  [https://github.com/rxi/map](https://github.com/rxi/map)
- vec [https://github.com/rxi/vec](https://github.com/rxi/vec)
- smallprofiler [https://github.com/realbogart/smallprofiler](https://github.com/realbogart/smallprofiler)
- luaarray [http://www.nongnu.org/techne/lua/luaarray/](http://www.nongnu.org/techne/lua/luaarray/)
- minunit [https://github.com/siu/minunit](https://github.com/siu/minunit)
- Remotery [https://github.com/Celtoys/Remotery](https://github.com/Celtoys/Remotery)
- Underscore [https://github.com/jtarchie/underscore-lua](https://github.com/jtarchie/underscore-lua)

### Help Wanted
If you would like to contribute, feel free to fork this stuff.
A wonderful start would be to include a unit test file to check all the functionalities of either API.
