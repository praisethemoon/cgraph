
require ('libcgraph')
local io = io
local write = io.write

function _renderDouble(value)
	write("Scalar [value=", value,"]\n")
end

function _renderVector(len, value)
	write("Vec [len: ", len, ", value=\n")
	for i, v in ipairs(value) do
		write("\t[", i-1, "] = \t", v, "\n")
	end
	write("]\n")
end


function _renderMatrix(rows, cols, value)
	write("Mat [rows: ", rows, ", cols = ", cols, ", value=")
	for i=1,rows do
		write("\n\t")
		for j=1,rows do
			write("\t", value[(i-1)*cols +j])
		end
	end
	write("\n]\n")
end

x = cgraph.matrix(3, 3, cgraph.doubles({1, 2, 3, 4, 5,6 ,7 ,8 ,9}))
y = cgraph.matrix(3, 3, cgraph.doubles({1, 2, 3, 4, 5,6 ,7 ,8 ,9}))
alpha = cgraph.vector(9, cgraph.doubles({1, 2, 3, 4, 5,6 ,7 ,8 ,9}))
z = cgraph.bop(0, x, y)

v = cgraph.var("a")

u = cgraph.bop(1, z, v) 

 

g = cgraph.graph("test", u)

cgraph.setVar(g, "a", cgraph.double(1))

res = cgraph.compute(g)

print(res.type, res.len or res.rows..'.'..res.cols, res.value)
_renderMatrix(res.rows, res.cols, res.value)
_renderVector(9, cgraph.doubles({1, 2, 3, 4, 5,6 ,7 ,8 ,9}))











