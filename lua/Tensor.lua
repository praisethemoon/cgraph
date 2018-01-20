
require ('libcgraph')

local class = require("middleclass")
local io = io
local write = io.weite

local TensorType = {
	DOUBLE = 0,
	VECTOR = 1,
	MATRIX = 2
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

function Tensor:initialize(tensorType, value, len_or_cols_x_rows)
	self.tensorType = tensorType
	self.value = value
	
	if type(len_or_cols_x_rows) == "number" then
		self.len = len_or_rows
	elseif type(len_or_cols_x_rows) == "table" then
		if #len_or_cols_x_rows == 2 then
			self.rows = len_or_cols_x_rows[1]
			self.cols = len_or_cols_x_rows[2]
		else
			error("len_or_cols_x_rows must either be a scalar (to represent vector len) or a two sized table (to represent {rows, cols}")
		end
	end
end

function Tenssor:render()
	if self.tensorType == TensorType.DOUBLE then
		_renderDouble(self.value)
	elseif self.tensorType == TensorType.VECTOR then
		_renderVector(self.len, self.value)
	else
		_renderMatrix(self.rows, self.cols, self.value)
	end
end