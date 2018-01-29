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
	SIN=5,
	COS=6,
	TAN=7,
	TANH=8,
	SUM=9
}

local function bopToString(bop)
	return cgraph.bopToString(bop)
end

local function uopToString(uop)
	return cgraph.uopToString(uop)
end

local function nodeTypeToString(n)
	return cgraph.nodeTypeToString(n)
end

local function tensorTypeToString(t)
	return cgraph.varTypeToString(t)
end

local function errorTypeToString(e)
	return cgraph.errorTypeToString(e)
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
			return "[UNKNOWN]\n"
		end
	elseif uhs.type == 'bop' then
		return "Binary  "..bopToString(uhs.opType) .. '\n'
	elseif uhs.type == 'uop' then
		return "Unary  "..uopToString(uhs.opType)..'\n'
	elseif uhs.type == 'var' then
		return "Variable [name: " .. uhs.name .. ']\n'
	else
		return "[UNKNOWN]\n"
	end
end

local mt = {
	__unm = function(uhs)
		local node = cgraph.uop(UnaryOperationType.MINUS, uhs.node)
		local op = {type='uop', opType=UnaryOperationType.MINUS, node = node, uhs=uhs}
		setmetatable(op, getmetatable(uhs))
				
		return op
	end,
	
	__add = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.ADD, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.ADD, node = node, lhs=lhs, rhs=rhs}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__sub = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.SUB, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.SUB, node = node, lhs=lhs, rhs=rhs}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__mul = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.MULT, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.MULT, node=node, lhs=lhs, rhs=rhs}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__div = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.DIV, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.DIV, node = node, lhs=lhs, rhs=rhs}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__pow = function(lhs, rhs)
		local node = cgraph.bop(BinaryOperationType.POW, lhs.node, rhs.node)
		local op = {type='bop', opType=BinaryOperationType.POW, node = node, lhs=lhs, rhs=rhs}
		setmetatable(op, getmetatable(lhs))
				
		return op
	end,
	
	__tostring = function(uhs)
		return nodeToString(uhs)
	end
}

--- Here comes the good stuff
local array = cgraph.doubles
local dumpMem = cgraph.dumpMem

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
	local op = {type='bop', opType=BinaryOperationType.POW, node = node, lhs=lhs, rhs=rhs}
	setmetatable(op, mt)
			
	return op
end

local dot = function(lhs, rhs)
	local node = cgraph.bop(BinaryOperationType.DOT, lhs.node, rhs.node)
	local op = {type='bop', opType=BinaryOperationType.DOT, node = node, lhs=lhs, rhs=rhs}
	setmetatable(op, mt)
			
	return op
end

local inv = function(uhs)
	local node = cgraph.uop(UnaryOperationType.INV, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.INV, node = node, uhs=uhs}
	setmetatable(op, mt)
			
	return op
end

local tr = function(uhs)
	local node = cgraph.uop(UnaryOperationType.TRANSPOSE, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.TRANSPOSE, node=node, uhs=uhs}
	setmetatable(op, mt)
			
	return op
end

local log = function(uhs)
	local node = cgraph.uop(UnaryOperationType.LOG, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.LOG, node = node, uhs=uhs}
	setmetatable(op, mt)
			
	return op
end

local exp = function(uhs)
	local node = cgraph.uop(UnaryOperationType.EXP, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.EXP, node = node, uhs=uhs}
	setmetatable(op, mt)
			
	return op
end

local sin = function(uhs)
	local node = cgraph.uop(UnaryOperationType.SIN, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.SIN, node = node, uhs=uhs}
	setmetatable(op, mt)
			
	return op
end

local cos = function(uhs)
	local node = cgraph.uop(UnaryOperationType.COS, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.COS, node = node, uhs=uhs}
	setmetatable(op, mt)
			
	return op
end

local tan = function(uhs)
	local node = cgraph.uop(UnaryOperationType.TAN, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.TAN, node = node, uhs=uhs}
	setmetatable(op, mt)
			
	return op
end

local tanh = function(uhs)
	local node = cgraph.uop(UnaryOperationType.TANH, uhs.node)
	local op = {type='uop', opType=UnaryOperationType.TANH, node = node, uhs=uhs}
	setmetatable(op, mt)
			
	return op
end

local function nodeToDot(graph, uhs, str)
	function listNodeToString(uhs, str, idCounter)
		local idCounter = idCounter + 1
		uhs.__id__ = idCounter
		local fillAttribute = ''
		
		--print(uhs.type, uhs.opType, uhs.node, graph.err.node, errorTypeToString(graph.err.type))
		
		if uhs.node ~= nil and graph.err ~= nil and uhs.node == graph.err.node then
			fillAttribute =', style="filled", fillcolor=red '
		end
		if uhs.type == 'value' then
			if uhs.tensorType == TensorType.DOUBLE then
				str = str .. "\t" .. idCounter ..' [label="Scalar ='..(uhs.value)..'", style="filled", fillcolor=".7 .3 1.0"];\n';
			elseif uhs.tensorType == TensorType.VECTOR then
				str = str .. "\t" .. idCounter ..' [label="Vector <'..(uhs.len)..'>", style="filled", fillcolor=".7 .5 1.0"];\n';
			elseif uhs.tensorType == TensorType.MATRIX then
				str = str .. "\t" .. idCounter ..' [label="Matrix <'..(uhs.rows)..'x'..(uhs.cols)..'>", style="filled", fillcolor=".7 .8 1.0"];\n';
			else 
				str = str .. "\t" .. idCounter ..' [label="UNKNOWN"];\n';
			end
			return str, idCounter
		elseif uhs.type == 'bop' then
			str = str .. "\t" .. idCounter ..' [label="'..bopToString(uhs.opType)..'"'..fillAttribute..', shape=circle];\n';
			str, idCounter = listNodeToString(uhs.lhs, str, idCounter)
			str, idCounter= listNodeToString(uhs.rhs, str, idCounter)
			return str, idCounter
		elseif uhs.type == 'uop' then
			str = str .. "\t" .. idCounter ..' [label="'..uopToString(uhs.opType)..'"'..fillAttribute..', shape=circle];\n';
			str, idCounter = listNodeToString(uhs.uhs, str, idCounter)
			return str, idCounter
		elseif uhs.type == 'var' then
			str = str .. "\t" .. idCounter ..' [label="Var '..(uhs.name)..'"'..fillAttribute..', shape=box];\n';
			local v = graph:getVar(uhs.name)
			if v ~= nil then
				str, idCounter = listNodeToString(v, str, idCounter)
			end
			
			return str, idCounter
		else
			str = str .. "\t" .. idCounter ..' [label="UNKNOWN"'..fillAttribute..', shape=plaintext];\n';
		end
	end
	
	function renderNodesToString(node, str)
		if node.type == 'bop' then
			str = str .. "\t" .. node.lhs.__id__ .. ' -> ' .. node.__id__ .. ';\n'
			str = str .. "\t" .. node.rhs.__id__ .. ' -> ' .. node.__id__ .. ';\n'
			str = renderNodesToString(node.lhs, str)
			str = renderNodesToString(node.rhs, str)
			return str
		elseif node.type == 'uop' then
			str = str .. "\t" .. node.uhs.__id__ .. ' -> ' .. node.__id__ .. ';\n'
			str = renderNodesToString(node.uhs, str)
			return str
		elseif node.type == 'var' then
			local v = graph:getVar(node.name)
			if v ~= nil then
				str = str .. "\t" .. v.__id__ .. ' -> ' .. node.__id__ .. ';\n'
				str = renderNodesToString(v, str)
			end
			
		end
		
		return str
	end
	
	str, id = listNodeToString(uhs, str, 0)
	str = str .. '\t' ..(uhs.__id__ ).. ' -> Result ;\n'
	str = renderNodesToString(uhs, str)
	return str
end

local graph = function(name, rootNode)
	local Graph = {}
	Graph.__index = Graph
	
	function Graph:create(name, root)
		local graph = {name= name, root=root, cdata=cgraph.graph(name, root.node), vars={}}
		setmetatable(graph, Graph)
		
		return graph
	end
	
	function Graph:setVar(name, uhs)
		cgraph.setVar(self.cdata, name, uhs.node)
		self.vars[name] = uhs
	end
	
	function Graph:getNativeVar(name)
		return cgraph.getVar(self.cdata, name)
	end
	
	function Graph:getVar(name)
		return self.vars[name]
	end
	
	function Graph:eval()
		self.err = {}
		local res = cgraph.compute(self.cdata)
		
		if res.error then
			print('error', errorTypeToString(res.error))
			self.err = res
			return res
		end
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
	
	function Graph:plot()
		local str = 'digraph ' .. self.name .. '{\n'
		str = nodeToDot(self, self.root, str)
		str = str .. '}'
		
		local f = io.open(self.name..".dot", "w")
		if f ~= nil then
			f:write(str)
			f:close()
		else
			print("Cannot open file '"..self.name..'.dot"')
		end
	end
	
	function Graph:diff(var, newName)
		local res = cgraph.diff(self.cdata, newName, var)
		local graph = {name= newName, root=res.root, cdata=res.graph, vars=self.vars}
		setmetatable(graph, Graph)
		
		return graph
	end
	
	function Graph:free()
		cgraph.freeGraph(self.cdata)
	end
	
	return Graph:create(name, rootNode)
end


local CGraph = {
	dumpMem=dumpMem,
	array=array,
	double=double,
	vector=vector,
	matrix=matrix,
	variable=variable,
	pow=pow,
	sin=sin,
	cos=cos,
	tan=tan,
	tanh=tanh,
	dot=dot,
	inv=inv,
	tr=tr,
	log=log,
	exp=exp,
	graph=graph,
}

return CGraph
