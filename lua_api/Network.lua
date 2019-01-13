local cg = require 'CGraph'
local rand = require 'RandomWeight'
local flot = require 'flot'
local Dense = require 'Dense'
local _ = require 'underscore'

local crossEntropy = cg.crossEntropyLoss

local function softmax(Z)
	return cg.exp(Z) / cg.sum(cg.exp(Z), 0)
end


function buildConfusionMatrix(nb_classes)
    local mat = {}
    for i=1,nb_classes do
      mat[i] = {}
      for j=1,nb_classes do
        mat[i][j] = 0
      end
    end
  
    return mat
end

--[[
    default unchangable parameters:
        - optimizer: SGD
        - Batch size: 1

]]
function fit(name, NN, X_train, Y_train, X_test, Y_test, num_classes, alpha)
    local X = cg.variable 'X'
    local y = cg.variable 'y'

    -- we only support cross entropy loss ATM

    print("Constructing graph")
    local node = NN[1].eval(X)

    for i=2,#NN do
        local v = NN[i]
        node = v.eval(node)
    end

    local cost = crossEntropy(node, y, num_classes)
    local eval = cg.argmax(softmax(node))
    local g = cg.graph(name, cost)


    print("Initializing random weights")
    for i, v in ipairs(NN) do
        if v.type == 'dense' then
            g:setVar(v.params[1].name, cg.matrix(v.size[1], v.size[2], rand.randomWeight(v.size[1]*v.size[2])))
            g:setVar(v.params[2].name, cg.vector(v.size[2], rand.randomWeight(v.size[2])))
        end
    end


    
    print("Training")
    local points = {}
    local loss = {}
    for k=1,1000 do
        local err = 0
        local X, y = shuffle(X_train, Y_train)
        for i=1,#X,1 do
            local x_i = _.flatten({X[i]})
            local y_i = _.flatten({y[i]})

            g:setVar('X', cg.matrix(1, #x_i, x_i))
            g:setVar('y', cg.vector(#y_i, y_i))
            local output = g:eval()
            err = err + output.value

            g:backProp()
            
            for i, v in ipairs(NN) do
                if v.type == 'dense' then
                    updateWeights(v.params[1].name, g, alpha)
                    updateWeights(v.params[2].name, g, alpha)
                end
            end
        end
        table.insert(loss, {k, err/#X})
    end
    local p = flot.Plot { -- legend at 'south east' corner
      legend = { position = "se" },
    }
    p:add_series("Avg. Loss", loss)
  
    flot.render(p)
    print("training complete, Testting")

    local confMat = buildConfusionMatrix(3)
    for i=1,#X_test,1 do
        local x_i = _.flatten({X_test[i]})
        local y_i = _.flatten({Y_test[i]})

        g:setVar('X', cg.matrix(1, #x_i, x_i))
        g:setVar('y', cg.vector(#y_i, y_i))
        
        local idx = g:evalNode(eval).value
        --local max, idx = argmax(output.value)
        confMat[idx+1][Y_test[i]+1] = confMat[idx+1][Y_test[i]+1] + 1
    end

    for i=1,3 do
        print(confMat[i][1], confMat[i][2], confMat[i][3])
    end

    g:plot()
end


function updateWeights(name, g, alpha)
    local t_1 = g:getVar(name)
    local dxt_1 = g:getVarDiff(name)
    --print(name, t_1, dxt_1)
    local size = t_1.len or t_1.cols*t_1.rows
  
    local t_1_newval = {}
    for k=1,size do
      table.insert(t_1_newval, t_1.value[k] - alpha*dxt_1.value[k])
    end
    
    if t_1.len then
      g:setVar(name, cg.vector(size, t_1_newval))
    else
      g:setVar(name, cg.matrix(t_1.rows, t_1.cols, t_1_newval))
    end
  end

  return fit