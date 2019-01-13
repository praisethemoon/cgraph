package.path = package.path .. ";../lua_api/?.lua"
package.cpath = package.cpath .. ";../lua_api/?.dylib"

local flot = require 'flot'
local CGraph = require 'CGraph'
local array = CGraph.array

local FileReader = require 'io.FileReader'
local datasets = require 'datasets'
local _ = require 'underscore'
local cg = CGraph

local crossEntroy = CGraph.crossEntropyLoss

--[[
  Reading IRIS CSV Dataset
]]

classes= {}

-- some version uses these class names

classes["Iris-setosa"] = 0
classes["Iris-versicolor"] = 1
classes["Iris-virginica"] = 2

-- others uses these
classes["setosa"] = 0
classes["versicolor"] = 1
classes["virginica"] = 2

X = {}
y = {}

local fn = function(line)
  local sl, sw, pl, pw, c = line:match('(%S+),(%S+),(%S+),(%S+),(%S+)')
  table.insert(X, {tonumber(sl), tonumber(sw), tonumber(pl), tonumber(pw)})
  table.insert(y, classes[c])
end

function file_exists(name)
  local f=io.open(name,"r")
  if f~=nil then io.close(f) return true else return false end
end

if not file_exists "../datasets/iris/iris.csv" then
  datasets.iris.download("../datasets/iris/")
end

print(datasets.iris.about)

FileReader.read("../datasets/iris/iris.csv", fn)


local Xinput = X
local Yclass = y

print(#Xinput, #Yclass)

function shuffle(tbl, tbl2)
  size = #tbl
  for i = size, 1, -1 do
    local rand = math.random(i)
    tbl[i], tbl[rand] = tbl[rand], tbl[i]
    tbl2[i], tbl2[rand] = tbl2[rand], tbl2[i]
  end
  return tbl, tbl2
end
local  function gaussian (mean, variance)
  math.randomseed(os.time()+math.random()*1000)
  local v1  = math.sqrt(-2 * variance * math.log(math.random()))
  math.randomseed(os.time()+math.random()*3000)
  local v2 = math.cos(2 * math.pi * math.random()) + mean
  return v1 * v2
end

local function randomWeight(size)
local vec = {}
for i=1,size do
  math.randomseed(os.time()+math.random()*1000)
  local x = math.random()*3
  math.randomseed(os.time()+math.random()*1000)
  local y = math.random()/100
  table.insert(vec, gaussian(x,y))
end

return vec
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
local softplus = CGraph.softplus

local A2 = relu(CGraph.dot(X, theta1) + b1)
local A3 = relu(CGraph.dot(A2, theta2) + b2)
local final = relu(CGraph.dot(A3, theta3) + b3)

local eval = CGraph.argmax(softmax(final))

local g = CGraph.graph("nn", crossEntroy((final), y, 3))




function updateWeights(name)
  local t_1 = g:getVar(name)
  local dxt_1 = g:getVarDiff(name)
  
  local size = t_1.len or t_1.cols*t_1.rows
  
  local alpha = 0.03

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
  g:setVar('T_1', CGraph.matrix(4, 5, randomWeight(20)))
  g:setVar('b_1', CGraph.vector(5, randomWeight(5)))
  g:setVar('T_2', CGraph.matrix(5, 5, randomWeight(25)))
  g:setVar('b_2', CGraph.vector(5, randomWeight(5)))
  g:setVar('T_3', CGraph.matrix(5, 3, randomWeight(15)))
  g:setVar('b_3', CGraph.vector(3, randomWeight(3)))
 
  
  local points = {}
  local loss = {}
  for k=1,1000 do
    local err = 0
    X, y = shuffle(X, y)
    for i=1,#X,1 do
      g:setVar('X', CGraph.matrix(1, 4, _.flatten({X[i]})))
      g:setVar('y', CGraph.vector(1, _.flatten({y[i]})))
      local output = g:eval()
      err = err + output.value
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
   table.insert(loss, {k, err/#X})
  end
  local p = flot.Plot { -- legend at 'south east' corner
    legend = { position = "se" },
  }
  p:add_series("Avg. Loss", loss)

  flot.render(p)

  local confMat = buildConfusionMatrix(3)
  for i=1,#X_test,1 do
    g:setVar('X', CGraph.matrix(1, 4, _.flatten({X_test[i]})))
    g:setVar('y', CGraph.vector(1, _.flatten({Y_test[i]})))
    
    local idx = g:evalNode(eval).value


    --local max, idx = argmax(output.value)
    confMat[idx+1][Y_test[i]+1] = confMat[idx+1][Y_test[i]+1] + 1
  end

  for i=1,3 do
    print(confMat[i][1], confMat[i][2], confMat[i][3])
  end
end

Xinput, Yclass = shuffle(Xinput,Yclass)
Xinput, Yclass = shuffle(Xinput,Yclass)
Xinput, Yclass = shuffle(Xinput,Yclass)
Xinput, Yclass = shuffle(Xinput,Yclass)

train(_.initial(Xinput, 100), _.initial(Yclass, 100), _.last(Xinput, 50), _.last(Yclass, 50))
print("good to meet ya")