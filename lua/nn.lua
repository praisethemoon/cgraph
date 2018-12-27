 
local CGraph = require 'CGraph'
local array = CGraph.array


local function sigmoid1(Z)
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	return sigmoid
end

local function sigmoid2(Z)
	local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
	return sigmoid
end

local function softmax(Z)
	return CGraph.exp(Z) / CGraph.sum(CGraph.exp(Z), 0)
end


local X = CGraph.variable 'X'
local theta1 = CGraph.variable 'T_1'
local b1 = CGraph.variable 'b_1'
local theta2 = CGraph.variable 'T_2'
local b2 = CGraph.variable 'b_2'
local y = CGraph.variable 'y'



local function crossEntropy(x, y)
	return -y * CGraph.log(x) - (CGraph.double(1) - y)*CGraph.log(CGraph.double(1) - x)
end


local A2 = sigmoid1(CGraph.dot(CGraph.tr(theta1), X ) + b1)
local A3 = sigmoid2(CGraph.dot(CGraph.tr(theta2), A2) + b2)
local final = A3

local g = CGraph.graph("nn", crossEntropy(softmax(final), y))

g:setVar('X', CGraph.vector(4, {5.1,7.5,0.4,1.2}))
g:setVar('T_1', CGraph.matrix(4, 5, {0.21, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('b_1', CGraph.vector(5, {0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('T_2', CGraph.matrix(5, 3, {0.3, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('b_2', CGraph.vector(3, {0.1, 0.1, 0.1}))
g:setVar('y', CGraph.vector(3, {1, 0, 0}))

local output = g:eval()

print(output)


g:backProp()

print 'T_1'
print(g:getVarDiff('T_1'))


print 'T_2'
print(g:getVarDiff('T_2'))

--[[


local x  = CGraph.variable('x')
local y  = CGraph.variable('y')


local cost = CGraph.graph("crossentropy", crossEntropy(output, CGraph.vector(3, {1, 0, 0})))

cost:eval()
]]
