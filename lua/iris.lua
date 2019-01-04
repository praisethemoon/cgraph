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

local Xinput = X
local Yclass = y

local function sigmoid(Z)
  local sigmoid = CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
  return sigmoid
end

local function softmax(Z)
  return (CGraph.exp(Z)) / CGraph.sum(CGraph.exp(Z), 1)
end

local X = CGraph.variable 'X'
local theta1 = CGraph.variable 'T_1'
local b1 = CGraph.variable 'b_1'
local theta2 = CGraph.variable 'T_2'
local b2 = CGraph.variable 'b_2'
local y = CGraph.variable 'y'

local A2 = CGraph.ReLU(CGraph.dot(X, theta1) + b1)
local A3 = CGraph.ReLU(CGraph.dot(A2, theta2) + b2)
local final = A3

local g = CGraph.graph("nn", crossEntroy(softmax(final), y, 3))


function gaussian (mean, variance)
    math.randomseed(os.time()+math.random()*1000)
    local v1  = math.sqrt(-2 * variance * math.log(math.random()))
    math.randomseed(os.time()+math.random()*3000)
    local v2 = math.cos(2 * math.pi * math.random()) + mean
    return v1 * v2
end

function randomWeight(size)
  vec = {}
  for i=1,size do
    table.insert(vec, gaussian(0, 0.1))
  end
  
  return vec
end

function train(X, y)
  g:setVar('T_1', CGraph.matrix(4, 5, randomWeight(20)))
  g:setVar('b_1', CGraph.vector(5, randomWeight(5)))
  g:setVar('T_2', CGraph.matrix(5, 3, randomWeight(15)))
  g:setVar('b_2', CGraph.vector(3, randomWeight(3)))
 
  print(g:getVar('T_1'))
  print(g:getVar('b_1'))
  print(g:getVar('T_2'))
  print(g:getVar('b_2'))
  g:setVar('X', CGraph.matrix(1, 4, _.flatten({X[i]})))
  g:setVar('y', CGraph.vector(1, _.flatten({y[i]})))
 
  local output = g:eval()
 
  local alpha = 0.1
  loss = {}
  for i=1,20 do
    local err = 0
    for i=1,148,4 do
      g:setVar('X', CGraph.matrix(4, 4, _.flatten({X[i], X[i+1], X[i+2],X[i+3]})))
      g:setVar('y', CGraph.vector(4, _.flatten({y[i], y[i+1],y[i+2],y[i+3]})))
      local output = g:eval()
      table.insert(loss, output.value)
      print(output.value)
      g:backProp()
      
      
      t_1 = g:getVar('T_2')
      dxt_1 = g:getVarDiff('T_2')
      t_1_newval = {}
      for k=1,t_1.rows*t_1.cols do
        table.insert(t_1_newval, t_1.value[k] - alpha*dxt_1.value[k])
      end
      g:setVar('T_2', CGraph.matrix(5, 3, t_1_newval))
      
      print(t_1)
      print(dxt_1)
      
      --[[
      t_1 = g:getVar('b_1')
      dxt_1 = g:getVarDiff('b_1')
      t_1_newval = {}
      for k=1,t_1.len do
        table.insert(t_1_newval, t_1.value[k] - alpha*dxt_1.value[k])
      end
      g:setVar('b_1', CGraph.vector(5, t_1_newval))
      
      t_1 = g:getVar('T_2')
      dxt_1 = g:getVarDiff('T_2')
      t_1_newval = {}
      for k=1,t_1.rows*t_1.cols do
        table.insert(t_1_newval, t_1.value[k] - alpha*dxt_1.value[k])
      end
      g:setVar('T_2', CGraph.matrix(5, 3, t_1_newval))
      
      
      t_1 = g:getVar('b_2')
      dxt_1 = g:getVarDiff('b_2')
      t_1_newval = {}
      for k=1,t_1.len do
        table.insert(t_1_newval, t_1.value[k] - alpha*dxt_1.value[k])
      end
      g:setVar('b_2', CGraph.vector(3, t_1_newval))
      ]]
   end
  end
  
  return loss
end

train(Xinput, Yclass)

--[[
g:backProp()

print 'T_1'
print(g:getVarDiff('T_1'))
]]