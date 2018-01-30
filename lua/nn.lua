 
local CGraph = require 'CGraph'
local array = CGraph.array


local function sigmoid(z)
	local Z = CGraph.variable("z")
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	local graph = CGraph.graph("sigmoid", sigmoid)
	graph:setVar("z", z)
	local res = graph:eval()
	return res
end


local X = array {0, 1, 2}
local theta1 = CGraph.variable 'T_1'
local b1 = CGraph.variable 'b_1'
local theta2 = CGraph.variable 'T_2'
local b2 = CGraph.variable 'b_2'


graph:setVar("T_1", CGraph.matrix(3, 3, {1, 1, 1, 1, 1, 1, 1, 1, 1}))
graph:setVar("b_1", CGraph.double(3)
graph:setVar("T_2", CGraph.matrix(3, 3, {1, 1, 1, 1, 1, 1, 1, 1, 1}))
graph:setVar("b_2", CGraph.double(4)

local L1 = X
local L2 = sigmoid(theta1*L1 + b1)
local L3 = sigmoid(


print(g4:eval())
