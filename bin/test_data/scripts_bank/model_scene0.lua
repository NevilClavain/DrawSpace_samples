

	include('continent_model.lua')	
	include('raptor_model.lua')
	include('trex_model.lua')
	include('metalcube_model.lua')
    include('spherebump_model.lua')
	include('skyboxmod_model.lua')
	include('raymarching_model.lua')
	
	
	continent.view.load('continent', {lit_rendering='texture_pass'}, 'root')
	
	--[[
	raptor.view.load('r', {lit_rendering='texture_pass'}, 'root')
	--trex.view.load('t', {lit_rendering='texture_pass'}, 'root')
	trex.view.wireframe.load('t', {wireframe_rendering='texture_pass'}, 'root')
	metalcube.view.load('c', {x = -4010.0, y = 15.0, z = -2740.0}, {lit_rendering='texture_pass'}, 'root' )
	spherebump.view.load('s', {x = -4010.0, y = 135.0, z = -2740.0}, {lit_rendering='texture_pass'}, 'root' )

	spherebump.view.load('s2', {x = -4010.0, y = 0, z = -2740.0}, {lit_rendering='texture_pass'}, 'root' )

    model.move.setpos('r', -4020.0, 0, -2740)
	model.move.setpos('t', -4068.0, 0, -2740)
	
	skyboxmod.view.load('skybox0', {layer0_rendering='texture_pass'}, 'root')
	]]

	raymarching.view.load('raymarch', {main_rendering='texture_pass'}, 'root')
	model.move.setpos('raymarch', -4020.0, 12.0, -2735)

	model.env.setgravity(1)
	
	model.env.setbkcolor(0.55,0.55,0.99)

	model.camera.mvt:set_pos(-4000.0, 15.0, -2684.0)
