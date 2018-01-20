
function tprint (tbl, indent)
  if not indent then indent = 0 end
  if type(tbl) ~= "table" then
	print(tbl)
return 
end
  for k, v in pairs(tbl) do
    formatting = string.rep("  ", indent) .. k .. ": "
    if type(v) == "table" then
      print(formatting)
      tprint(v, indent+1)
    else
      print(formatting , v)
    end
  end
end

local function pretty(a, prefix)
   if type(a) == "number" then
      return tostring(a)
   else
      local s, m, last, number

      prefix = prefix or ""
      m = #a
      last = a[m]
      number = type(last) == "number"

      s = prefix .. "{"

      if not number then
	 s = s .. "\n"
      end
	 
      for i = 1, m - 1 do
	 s = s .. pretty(a[i], prefix .. "  ") .. ", "

	 if not number then
	    s = s .. "\n"
	 end
      end

      s = s .. pretty(last, prefix .. "  ")

      if not number then
	 s = s .. "\n" .. prefix
      end

      return s .. "}"
   end
end

local function size(a)
   return type(a) == "number" and {} or {#a, unpack(size(a[1]))}
end
require('libcgraph')



x = cgraph.matrix(3, 3, cgraph.doubles({1, 2, 3, 4, 5,6 ,7 ,8 ,9}))
y = cgraph.double(3)


z = cgraph.bop(2, x, y) 

g = cgraph.graph("test", z)

v, t = cgraph.compute(g)



print(v)
tprint(v)
print(t)
for i, v in ipairs(t or {}) do
	print(i, v)
end
