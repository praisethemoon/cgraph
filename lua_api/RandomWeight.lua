local math = math

local  function gaussian (mean, variance)
    math.randomseed(os.time()+math.random()*1000)
    local v1  = math.sqrt(-2 * variance * math.log(math.random()))
    math.randomseed(os.time()+math.random()*3000)
    local v2 = math.cos(2 * math.pi * math.random()) + mean
    return v1 * v2
end

local function randomWeight(size)
  local vec = {}
  for i=1,size do
    math.randomseed(os.time()+math.random()*1000)
    local x = math.random()*3
    math.randomseed(os.time()+math.random()*1000)
    local y = math.random()/100
    table.insert(vec, gaussian(x,y))
  end
  
  return vec
end

return {
    randomWeight = randomWeight
}