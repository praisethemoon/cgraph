package.path = package.path .. ";../lua_api/?.lua"
package.cpath = package.cpath .. ";../lua_api/?.dylib"

local FileReader = require 'io.FileReader'
local datasets = require 'datasets'
local _ = require 'underscore'
local Dense = require 'Dense'
local fit = require 'Network'
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


Xinput, Yclass = shuffle(Xinput,Yclass)
Xinput, Yclass = shuffle(Xinput,Yclass)
Xinput, Yclass = shuffle(Xinput,Yclass)
Xinput, Yclass = shuffle(Xinput,Yclass)

local NN = {
    Dense("L1", {4, 5}, "relu"),
    Dense('L2', {5, 5}, "relu"),
    Dense('L3', {5, 3}, "relu"),
}

fit('iris_nn', NN, _.initial(Xinput, 100), _.initial(Yclass, 100), _.last(Xinput, 50), _.last(Yclass, 50), 3, 0.03)

--train(_.initial(Xinput, 100), _.initial(Yclass, 100), _.last(Xinput, 50), _.last(Yclass, 50))
print("good to meet ya")