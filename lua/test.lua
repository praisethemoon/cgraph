-- Obivously this does work yet, just testing how the Lua API should look like

A = ccgraph.var('A')
B = ccgraph.const({3, 2}, {{1, -1}, {0.5, 3}, {0.3, 8}})

graph = ccgraph.new(A:T() * B)

graph.set('A', ccgraph.Matrix({3, 2}, {{1, -1}, {0.5, 3}, {0.3, 8}}))
