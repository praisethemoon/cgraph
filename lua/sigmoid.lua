local CGraph = require 'CGraph'
local array = CGraph.array


local function sigmoid(z)
	local Z = CGraph.variable("z")
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	local graph = CGraph.graph("sigmoid", sigmoid)
	graph:setVar("z", z)
	graph:plot()
	local res = graph:eval()
	return res
end

print(sigmoid(CGraph.dot( CGraph.vector(3, array {1, 2, 3}), CGraph.vector(3, array {4, 5, 6}) )))

--[[
local node = CGraph.dot( CGraph.vector(3, array {1, 2, 3}), CGraph.vector(3, array {4, 5, 6}) )
local graph = CGraph.graph("sigmoid", node)
print(sigmoid(graph:eval()))
]]

return sigmoid
