package.path = package.path .. ";../lua_api/?.lua"
package.cpath = package.cpath .. ";../lua_api/?.dylib"

local FileReader = require 'io.FileReader'
local datasets = require 'datasets'
local _ = require 'underscore'
local Dense = require 'Dense'
local fit = require 'Network'

  
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

function shuffle(tbl, tbl2)
  size = #tbl
  for i = size, 1, -1 do
    local rand = math.random(i)
    tbl[i], tbl[rand] = tbl[rand], tbl[i]
    tbl2[i], tbl2[rand] = tbl2[rand], tbl2[i]
  end
  return tbl, tbl2
end

X_train, Y_train = shuffle(X_train,Y_train)
X_train, Y_train = shuffle(X_train,Y_train)
X_train, Y_train = shuffle(X_train,Y_train)
X_train, Y_train = shuffle(X_train,Y_train)

local NN = {
    Dense("L1", {784, 1000}, "relu"),
    Dense('L2', {1000, 900}, "relu"),
    Dense('L3', {900, 700}, "relu"),
    Dense('L4', {700, 500}, "relu"),
    Dense('L5', {500, 200}, "relu"),
    Dense('L6', {200, 50}, "relu"),
    Dense('L7', {50, 10}, "relu"),
}

fit('mnist_nn', NN, X_train, Y_train, X_test, Y_test, 10, 0.03)