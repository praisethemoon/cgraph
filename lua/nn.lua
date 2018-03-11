 
local CGraph = require 'CGraph'
local array = CGraph.array


local function sigmoid(Z)
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	return sigmoid
end


local X = CGraph.variable 'X'
local theta1 = CGraph.variable 'T_1'
local b1 = CGraph.variable 'b_1'
local theta2 = CGraph.variable 'T_2'
local b2 = CGraph.variable 'b_2'

--[[

]]

local A2 = sigmoid(CGraph.tr(theta1) * X + b1)
local A3 = sigmoid(CGraph.tr(theta2) * A2 + b2)

local g = CGraph.graph("nn", A3)

g:setVar('X', CGraph.vector(4, {5.1,7.5,0.4,1.2}))
g:setVar('T_1', CGraph.matrix(4, 5, {0.21, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('b_1', CGraph.vector(5, {0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('T_2', CGraph.matrix(5, 3, {0.3, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('b_2', CGraph.vector(3, {0.1, 0.1, 0.1, 0.1, 0.1}))

--g:plot()
print(g:eval())

