local CGraph = require 'CGraph'
local array = CGraph.array


local function sigmoid(z)
	local Z = CGraph.variable("z")
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	local graph = CGraph.graph("sigmoid", sigmoid)
	graph:setVar("z", z)
	return CGraph.graphNode(graph)
end

local X = CGraph.double(2) * sigmoid(CGraph.double(0))
graph = CGraph.graph("test", X)
print(graph:eval())


return sigmoid
