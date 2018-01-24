local CGraph = require 'CGraph'
local array = CGraph.array


local function sigmoid(z)
	local Z = CGraph.variable("z")
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	local graph = CGraph.graph("sigmoid", sigmoid)
	graph:setVar("z", z)
	local res = graph:eval()
	graph:plot()
	--print("before")
	--CGraph.dumpMem()
	--print("after")
	--graph:free()
	--CGraph.dumpMem()
	return res
end

print(sigmoid(CGraph.dot( CGraph.vector(3, array {0,0,0}), CGraph.vector(3, array {0,0,0}) )))


return sigmoid
