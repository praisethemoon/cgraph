local CGraph = require 'CGraph'
local array = CGraph.array

local FileReader = require 'FileReader'

local cg = CGraph

local crossEntroy = CGraph.crossEntropyLoss

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



local function softmax(Z)
	return CGraph.tr(CGraph.exp(Z)) / CGraph.sum(CGraph.exp(Z), 0)
end

local A2 = sigmoid(CGraph.dot(X, theta1) + b1)
local A3 = sigmoid(CGraph.dot(A2, theta2) + b2)
local final = A3

local g = CGraph.graph("nn", crossEntroy(softmax(final), y, 2))

g:setVar('X', CGraph.matrix(2, 4, {5.1, 7.5, 0.4, 1.2, 0, 1, 5.2, 3.33}))
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


g:backProp()

print 'T_1'
print(g:getVarDiff('T_1'))
