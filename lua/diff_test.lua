local CGraph = require 'CGraph'
local array = CGraph.array



local A = CGraph.double(2)
local B = CGraph.pow(CGraph.variable('x'), CGraph.double(3) )

local C = CGraph.double(3)

local f = A*B + C

local graph = CGraph.graph("test", f)
local dgraph = graph:diff('x', 'diff_1')
dgraph:plot()
dgraph.name = "op_diff_1"
dgraph:optimize()
dgraph:plot()


--local ddgraph = dgraph:diff('x', 'diff_2')
--ddgraph:plot()
--local dddgraph = ddgraph:diff('x', 'diff_3')
--dddgraph:plot()