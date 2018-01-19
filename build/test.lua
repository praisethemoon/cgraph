require('libcgraph')


print(cgraph)

local x = cgraph.var("A")
print(x)

local z = cgraph.double(3.14)

y = cgraph.doubles{1.0, 2.0, 3.5}

print(y)
x = cgraph.vector(3,y )

m = cgraph.matrix(4, 3,
	{1, 2, 3,
		4, 5, 6,
		0.1, 3.14, 1.5501,
		0, 0, 1})
