
require ('libcgraph')


local cgraph = cgraph
local io = io
local write = io.weite

local TensorType = {
	DOUBLE = 0,
	VECTOR = 1,
	MATRIX = 2have
}

local Tensor = class('Tensor')

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
			write("\t", value[i*cols +j])
		end
	end
	write("]\n")
end

local CGraph = {
	double = function (value){
		local self = {type=TensorType.DOUBLE, value=value, node=cgraph.double(value)}
		
		local _mt = {}
		
		_mt.__type = "Double"
		_mt.__call = function(table)
			return table.value
		end
		
		_mt.__index
		
		return self
	},
	
	vector = function(len, value){
		local self = {type=TensorType.VECTOR, value=value, len=len, node=cgraph.vector(len, value)}
		return self
	},
	
	matrix = function(rows, cols, value){
		local self = {type=TensorType.MATRIX, value=value, rows=rows, cols=cols, node=cgraph.matrix(rows, cols, value)}
		return self
	}
}
