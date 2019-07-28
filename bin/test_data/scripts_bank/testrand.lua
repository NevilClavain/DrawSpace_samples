

engine=RandomEngine(8885)

--[[
distr=Distribution("uniform_int_distribution", 0, 19)

g:print('->'..distr:generate(engine))
g:print('->'..distr:generate(engine))
g:print('->'..distr:generate(engine))
g:print('->'..distr:generate(engine))
g:print('->'..distr:generate(engine))
]]

distr2=Distribution("uniform_real_distribution", -5.0, 5.0)

g:print('->'..distr2:generate(engine))

for i = 0, 9, 1 do
  g:print('->'..distr2:generate(engine))
end