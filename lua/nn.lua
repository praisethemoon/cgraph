local CGraph = require 'CGraph'
local array = CGraph.array

local FileReader = require 'FileReader'
local _ = require 'underscore'
local cg = CGraph

local crossEntroy = CGraph.crossEntropyLoss

classes= {}

classes["Iris-setosa"] = 0
classes["Iris-versicolor"] = 1
classes["Iris-virginica"] = 2

X = {}
y = {}

local fn = function(line)
  local id, sl, sw, pl, pw, c = line:match('(%S+),(%S+),(%S+),(%S+),(%S+),(%S+)')
  table.insert(X, {tonumber(sl), tonumber(sw), tonumber(pl), tonumber(pw)})
  table.insert(y, classes[c])
end

FileReader.read("Iris.csv", fn)

table.remove(X, 1)

print(#X)
print(#y)

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
  return CGraph.tr(CGraph.exp(Z) / CGraph.sum(CGraph.exp(Z), 0))
end

local A2 = sigmoid(CGraph.dot(X, theta1) + b1)
local A3 = sigmoid(CGraph.dot(A2, theta2) + b2)
local final = A3

local g = CGraph.graph("nn", crossEntroy(softmax(final), y, 3))

g:setVar('X', CGraph.matrix(1, 4, _.flatten({X[1]})))
g:setVar('T_1', CGraph.matrix(4, 5, {0.21, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('b_1', CGraph.vector(5, {0.1, 0.1, 0.1, 0.1, 0.1}))
g:setVar('T_2', CGraph.matrix(5, 1, {0.3, 0.4, 0.12, 0.14, 0.1}))
g:setVar('b_2', CGraph.double(0.0))
g:setVar('y', CGraph.double(y[1]))

function gaussian (mean, variance)
    return  math.sqrt(-2 * variance * math.log(math.random())) *
            math.cos(2 * math.pi * math.random()) + mean
end

function randomWeight(size)
  vec = {}
  for i=1,size do
    vec[#vec] = gaussian(0, 0.5)
  end
  
  return vec
end

function train(X, y)
  g:setVar('T_1', CGraph.matrix(4, 5, randomWeight(20)))
  g:setVar('b_1', CGraph.vector(5, randomWeight(5)))
  g:setVar('T_2', CGraph.matrix(5, 1, randomWeight(5)))
  g:setVar('b_2', CGraph.vector(1, randomWeight(1)))
  
  local alpha = 0.1
  
  for i=1,500 do
    local err = 0
    for i=1,#X do
      g:setVar('X', CGraph.matrix(1, 4, _.flatten({X[i]})))
      g:setVar('y', CGraph.vector(1, _.flatten({y[i]})))
      
      local output = g:eval()
      g:backProp()
      g:getVarDiff('T_1')
      
    end
  end
end



local output = g:eval()
print(output)

