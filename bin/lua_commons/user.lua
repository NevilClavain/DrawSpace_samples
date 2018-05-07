

g:set_texturesrootpath("test_data/textures_bank")
g:set_shadersrootpath("test_data/shaders_bank")
g:set_meshesrootpath("test_data/meshes_bank")
g:set_scriptsrootpath("test_data/scripts_bank")

run=function()
	g:do_file('testwater_i.lua')
end

stop=function()
	g:do_file('testwater_r.lua')
end
