

engine=RandomEngine(12344)

distr=Distribution("uniform_int_distribution", 0, 19)

g:print('->'..distr:generate(engine))
g:print('->'..distr:generate(engine))
g:print('->'..distr:generate(engine))
g:print('->'..distr:generate(engine))
g:print('->'..distr:generate(engine))

