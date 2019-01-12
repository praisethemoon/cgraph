require 'package_config'

local CGraph = require 'CGraph'
local array = CGraph.array

local FileReader = require 'io.FileReader'
local datasets = require 'datasets'
local _ = require 'underscore'
local cg = CGraph

local crossEntroy = CGraph.crossEntropyLoss


  
function file_exists(name)
    local f=io.open(name,"r")
    if f~=nil then io.close(f) return true else return false end
end

if not file_exists "../datasets/mnist/train.csv" then
    datasets.mnist.download("../datasets/mnist/")
end
print("\n\n")
print(datasets.mnist.about)
print("\n\n")

function split(s, delimiter)
    result = {};
    for match in (s..delimiter):gmatch("(.-)"..delimiter) do
        table.insert(result, tonumber(match));
    end
    return result;
end

print("Reading train set")

X_train = {}
Y_train = {}

local fn = function(line)
    local data = split(line, ',')
    table.insert(Y_train, data[1])
    table.remove(data, 1)
    table.insert(X_train, data)
end
FileReader.read("../datasets/mnist/train.csv", fn)

X_test = {}
Y_test = {}

print("Reading test set")
local fn2 = function(line)
    local data = split(line, ',')
    table.insert(Y_test, data[1])
    table.remove(data, 1)
    table.insert(X_test, data)
end

FileReader.read("../datasets/mnist/test.csv", fn2)



function shuffle(tbl, tbl2)
  size = #tbl
  for i = size, 1, -1 do
    local rand = math.random(i)
    tbl[i], tbl[rand] = tbl[rand], tbl[i]
    tbl2[i], tbl2[rand] = tbl2[rand], tbl2[i]
  end
  return tbl, tbl2
end


local function sigmoid(Z)
  return CGraph.double(1) / (CGraph.double(1) + CGraph.exp(-Z))
end


local function softmax(Z)
	return CGraph.exp(Z) / CGraph.sum(CGraph.exp(Z), 0)
end

local X = CGraph.variable 'X'
local theta1 = CGraph.variable 'T_1'
local b1 = CGraph.variable 'b_1'
local theta2 = CGraph.variable 'T_2'
local b2 = CGraph.variable 'b_2'
local theta3 = CGraph.variable 'T_3'
local b3 = CGraph.variable 'b_3'
local y = CGraph.variable 'y'

local relu = CGraph.ReLU

local A2 = sigmoid(CGraph.dot(X, theta1) + b1)
local A3 = sigmoid(CGraph.dot(A2, theta2) + b2)
local final = sigmoid(CGraph.dot(A3, theta3) + b3)

local eval = softmax(final)

local g = CGraph.graph("nn", crossEntroy((final), y, 10))


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
    math.randomseed(os.time()+math.random()*1000)
    local x = math.random()*3
    math.randomseed(os.time()+math.random()*1000)
    local y = math.random()/100
    table.insert(vec, gaussian(x,y))
  end
  
  return vec
end


function updateWeights(name)
  local t_1 = g:getVar(name)
  local dxt_1 = g:getVarDiff(name)
  
  local size = t_1.len or t_1.cols*t_1.rows
  
  local alpha = 0.3

  local t_1_newval = {}
  for k=1,size do
    table.insert(t_1_newval, t_1.value[k] - alpha*dxt_1.value[k])
  end
  
  if t_1.len then
    g:setVar(name, CGraph.vector(size, t_1_newval))
  else
    g:setVar(name, CGraph.matrix(t_1.rows, t_1.cols, t_1_newval))
  end
end

function argmax(t)
  local max, max_idx = t[1], 1
  for i=1,#t do
    if v > max then
      max = v
      max_idx = i
    end
  end

  return max, max_idx
end

function buildConfusionMatrix(nb_classes)
  local mat = {}
  for i=1,nb_classes do
    mat[i] = {}
    for j=1,nb_classes do
      mat[i][j] = 0
    end
  end

  return mat
end

function train(X, y, X_test, Y_test)
  print('Training on '..#X..' samples and testing on '..#X_test..' samples.')
  g:setVar('T_1', CGraph.matrix(784, 650, randomWeight(509600)))
  g:setVar('b_1', CGraph.vector(650, randomWeight(650)))
  g:setVar('T_2', CGraph.matrix(650, 300, randomWeight(195000)))
  g:setVar('b_2', CGraph.vector(300, randomWeight(300)))
  g:setVar('T_3', CGraph.matrix(300, 10, randomWeight(3000)))
  g:setVar('b_3', CGraph.vector(10, randomWeight(10)))
 
  
  loss = {}
  for k=1,1 do
    print('epoch', k)
    local err = 0
    X, y = shuffle(X, y)
    for i=1,10,1 do
      g:setVar('X', CGraph.matrix(1, 784, _.flatten({X[i]})))
      g:setVar('y', CGraph.vector(1, _.flatten({y[i]})))
      local output = g:eval()
      table.insert(loss, output.value)
      --print(output.value)
      
      g:backProp()
      
      --local dxt_1 = g:getVarDiff('b_1')
      --print(dxt_1)

      updateWeights('T_1')
      updateWeights('b_1')
      updateWeights('T_2')
      updateWeights('b_2')
      updateWeights('T_3')
      updateWeights('b_3')
      
   end
  end
  print(loss[#loss-1])
  local confMat = buildConfusionMatrix(10)
  for i=1,#X_test,1 do
    g:setVar('X', CGraph.matrix(1, 784, _.flatten({X_test[i]})))
    g:setVar('y', CGraph.vector(1, _.flatten({Y_test[i]})))
    local output = g:evalNode(eval)
    print(output.value)
    local max, idx = argmax(output.value)
    confMat[idx][Y_test[i]+1] = confMat[idx][Y_test[i]+1] + 1
  end

  for i=1,10 do
    for j=1,10 do
        io.write(confMat[i][j] .. ', ')
    end
    io.write("\n")
  end
end

train(_.initial(X_train,100) , _.initial(Y_train, 100), _.initial(X_test, 100), _.initial(Y_test, 100))

g:free()