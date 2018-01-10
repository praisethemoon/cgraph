--[[
	Tensor representation
]]

local class = require 'middleclass'

local Tensor = class 'Tensor'

local TensorType = {
	SCALAR = 1,
	VECTOR = 2,
	MATRIX = 3
}

local InvertedTensorType = {
	[1] = 'SCALAR',
	[2] = 'VECTOR',
	[3] = 'MATRIX'
}

function Tensor:initialize(type, name, value)
	self.type = type
	self.name = name
	self.value = value
end

function Tensor:print()
	if self.name then
		print("Tensor", self.name)
	end
	print('', "Type", InvertedTensorType[self.type])
end

return Tensor, TensorType