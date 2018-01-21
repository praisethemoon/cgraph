require ('libcgraph')

local cgraph = cgraph
local io = io
local write = io.write

local TensorType = {
	DOUBLE = 0,
	VECTOR = 1,
	MATRIX = 2,
}

local BinaryOperationType = {
	ADD=0,
	SUB=1,
	MULT=2,
	DIV=3,
	POW=4,
	DOT=5,
	TMULT=6, 
}

local UnaryOperationType = {
	MINUS=0,
	INV=1,
	TRANSPOSE=2,
	EXP=3,
	LOG=4,
}

local function bopToString(bop)
	local bopStr = {
		[0] = "+",
		[1] = "-",
		[2] = "*",
		[3] = "/",
		[4] = "^",
		[5] = ".",
		[6] = "T*",
	}
	
	return bopStr[bop]
end

local function uopToString(uop)
	local uopStr = {
		[0] = "-",
		[1] = "inv",
		[2] = "T",
		[3] = "exp",
		[4] = "log",
	}
	
	return uopStr[uop]
end

local function _renderDouble(value)
	return ("Scalar [value: ".. value.."]\n")
end

local function _renderVector(len, value)
	local strRep = ("Vec [len: ".. len.. ", value: \n")
	for i, v in ipairs(value) do
		strRep = strRep .. ("\t[".. (i-1).. "] = \t".. v.. "\n")
	end
	strRep = strRep .. ("]\n")
	return strRep
end


local function _renderMatrix(rows, cols, value)
	local strRep= ("Mat [rows: ".. rows.. ", cols: ".. cols.. ", value: ")
	for i=1,rows do
		strRep = strRep .. ("\n\t")
		for j=1,rows do
			strRep = strRep .. ("\t".. value[(i-1)*cols +j])
		end
	end
	strRep = strRep .. ("\n]\n")
	return strRep
end

local function nodeToString(uhs)
	if uhs.type == 'value' then
		if uhs.tensorType == TensorType.DOUBLE then
			return _renderDouble(uhs.value)
		elseif uhs.tensorType == TensorType.VECTOR then
			return _renderVector(uhs.len, uhs.value)
		elseif uhs.tensorType == TensorType.MATRIX then
			return _renderMatrix(uhs.rows, uhs.cols, uhs.value)
		else 
			return "[UNKNOWN]"
		end
	elseif uhs.type == 'bop' then
		return "Binary  "..bopToString(uhs.opType)
	elseif uhs.type == 'uop' then
		return "Unary  "..uopToString(uhs.opType)
	elseif uhs.type == 'var' then
		return "Variable [name: " .. uhs.name .. ']'
	else
		return "[UNKNOWN]"
	end
		
end

local mt = {
	__unm = function(uhs)
		local node = cgraph.uop(UnaryOperationType.MINUS, uhs.node)
		local op = {type='uop', opType=UnaryOperationType.MINUS, node = node}
		setmetatable(op, getmetatable(uhs))
				
		return op
	end,
	
	__add = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.ADD, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.ADD, node = node}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__sub = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.SUB, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.SUB, node = node}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__mul = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.MULT, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.MULT, node = node}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__div = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.DIV, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.DIV, node = node}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__pow = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.ADD, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.POW, node = node}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__tostring = function(uhs)
		return nodeToString(uhs)
	end
}

--- Here comes the good stuff
local array = cgraph.doubles

local double = function (value)
	local self = {type='value', tensorType=TensorType.DOUBLE, value=value, node=cgraph.double(value)}
	setmetatable(self, mt)
	
	return self
end

local vector = function(len, value)
	local self = {type='value', tensorType=TensorType.VECTOR, value=value, len=len, node=cgraph.vector(len, value)}
	setmetatable(self, mt)
	
	return self
end

local matrix = function(rows, cols, value)
	local self = {type='value', tensorType=TensorType.MATRIX, value=value, rows=rows, cols=cols, node=cgraph.matrix(rows, cols, value)}
	setmetatable(self, mt)
	
	return self
end

local variable = function(name)
	local self = {type='var', name=name, node=cgraph.var(name)}
	setmetatable(self, mt)
	
	return self
end

local pow = function(lhs, rhs)
	local node = cgraph.bop(BinaryOperationType.POW, lhs.node, rhs.node)
	local op = {type='bop', opType=BinaryOperationType.POW, node = node}
	setmetatable(op, mt)
			
	return op
end

local dot = function(lhs, rhs)
	local node = cgraph.bop(BinaryOperationType.DOT, lhs.node, rhs.node)
	local op = {type='bop', opType=BinaryOperationType.DOT, node = node}
	setmetatable(op, mt)
			
	return op
end

local inv = function(uhs)
	local node = cgraph.uop(UnaryOperationType.INV, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.INV, node = node}
	setmetatable(op, mt)
			
	return op
end

local tr = function(uhs)
	local node = cgraph.uop(UnaryOperationType.TRANSPOSE, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.TRANSPOSE, node = node}
	setmetatable(op, mt)
			
	return op
end

local log = function(uhs)
	local node = cgraph.uop(UnaryOperationType.LOG, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.LOG, node = node}
	setmetatable(op, mt)
			
	return op
end

local exp = function(uhs)
	local node = cgraph.uop(UnaryOperationType.EXP, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.EXP, node = node}
	setmetatable(op, mt)
			
	return op
end

local graph = function(name, rootNode)
	local Graph = {}
	Graph.__index = Graph
	
	function Graph:create(name, root)
		local graph = {name= name, root=root, cdata=cgraph.graph(name, root.node)}
		setmetatable(graph, Graph)
		
		return graph
	end
	
	function Graph:setVar(name, uhs)
		cgraph.setVar(self.cdata, name, uhs.node)
	end
	
	function Graph:eval()
		local res = cgraph.compute(self.cdata)
		if res.type == TensorType.DOUBLE then
			return double(res.value)
		elseif res.type == TensorType.VECTOR then
			return vector(res.len, res.value)
		elseif res.type == TensorType.MATRIX then
			return matrix(res.rows, res.cols, res.value)
		else
			return {}
		end
	end
	
	return Graph:create(name, rootNode)
end

local CGraph = {
	array=array,
	double=double,
	vector=vector,
	matrix=matrix,
	variable=variable,
	pow=pow,
	dot=dot,
	inv=inv,
	tr=tr,
	log=log,
	exp=exp,
	graph=graph,
}

return CGraph
