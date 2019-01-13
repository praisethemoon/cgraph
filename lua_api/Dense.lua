

local cg = require 'CGraph'


local function softmax(Z)
	return cg.exp(Z) / cg.sum(cg.exp(Z), 0)
end

local function sigmoid(Z)
    return cg.double(1) / (cg.double(1) + cg.exp(-Z))
  end

return function (name, size, activation)
    local f = cg.ReLU
    if activation == 'relu' then
        f = cg.ReLU
    elseif activation == "softplus" then
        f = cg.softplus
    elseif activation == 'sigmoid' then
        f = sigmoid
    end

    local W = cg.variable('W_'..name)
    local b = cg.variable('b_'..name)

    return {
        eval = function (X)
            return f(cg.dot(X, W) + b)
        end,
        type="dense",
        activation=activation,
        params = {
            W,
            b,
        },
        size = size
    }
end