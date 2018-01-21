local CGraph = require 'CGraph'
local array = CGraph.array

local startTime = os.clock()

local A = CGraph.double(3)
local B = CGraph.vector(9, array{1, 2, 3, 4, 5, 6, 7, 8, 9})
local C = CGraph.matrix(3, 3, array{1, 2, 3, 4, 5, 6, 7, 8, 9})

local Z = A + B
local Y = Z - A
local W = Z * Y
local PHI = CGraph.exp(-W)

--local g1 = CGraph.graph("test", Z)
--local g2 = CGraph.graph("test", Y)
--local g3 = CGraph.graph("test", W)
local g4 = CGraph.graph("test", PHI)

print(g4:eval())

  local endTime = os.clock()
  
  
  print(endTime - startTime)