 
local CGraph = require 'CGraph'
local array = CGraph.array

local function crossEntropy(x, y)
    -- − y∗log(x)−(1−y)(log(1−x)), d/dx
	return -y * CGraph.log(x) - (CGraph.double(1) - y)*CGraph.log(CGraph.double(1) - x)
end

local x  = CGraph.variable('x')
local y  = CGraph.variable('y')


local g = CGraph.graph("crossentropy", crossEntropy(x, y))


local dgraph = g:diff(x, 'diff_1')
dgraph:optimize()

dgraph:setVar('x', CGraph.double(0.5))
dgraph:setVar('y', CGraph.double(0.3))

print(dgraph:eval())