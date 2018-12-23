local CGraph = require 'CGraph'
local array = CGraph.array

local FileReader = require 'FileReader'

local function sigmoid(Z)
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	return sigmoid
end


local X = CGraph.variable 'X'
local theta1 = CGraph.variable 'T_1'
local b1 = CGraph.variable 'b_1'
local theta2 = CGraph.variable 'T_2'
local b2 = CGraph.variable 'b_2'
local y = CGraph.variable 'y'



local function crossEntropy(x, y)
	local epsilon = CGraph.double(0.0001)
	return CGraph.sum(y * CGraph.log(x) + (CGraph.double(1) - y)*CGraph.log(CGraph.double(1) - x), 1)
end

local function softmax(Z)
	return CGraph.tr(CGraph.exp(Z)) / CGraph.sum(CGraph.exp(Z), 0)
end

local A2 = sigmoid(CGraph.dot(X, theta1) + b1)
local A3 = sigmoid(CGraph.dot(A2, theta2) + b2)
local final = A3

local g = CGraph.graph("nn", crossEntropy(softmax(final), y))

g:setVar('X', CGraph.matrix(2, 4, {5.1,7.5,0.4,1.2, 0, 1, 5.2, 3.33, 0.01}))
g:setVar('T_1', CGraph.matrix(4, 5, {0.21, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('b_1', CGraph.vector(5, {0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('T_2', CGraph.matrix(5, 1, {0.3, 0.4, 0.12, 0.14, 0.1}))
g:setVar('b_2', CGraph.double(0.0))
g:setVar('y', CGraph.vector(2, {0, 1}))

local fn = function(line)
	local id, sl, sw, pl, pw, c = line:match('(%S+),(%S+),(%S+),(%S+),(%S+),(%S+)')
	return {tonumber(sl), tonumber(sw), tonumber(pl), tonumber(pw), classes[c]}
end



local output = g:eval()
print(output)
