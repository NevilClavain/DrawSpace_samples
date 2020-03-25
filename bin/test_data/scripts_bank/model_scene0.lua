

	include('continent_model.lua')	
	include('raptor_model.lua')
	include('trex_model.lua')
	include('metalcube_model.lua')
    include('spherebump_model.lua')
	include('skyboxmod_model.lua')
	include('boulder_model.lua')
	
	
	continent.view.load('continent', {lit_rendering='texture_pass'}, 'root')
	
	
	raptor.view.load('r', {lit_rendering='texture_pass'}, 'root')
	trex.view.load('t', {lit_rendering='texture_pass'}, 'root')
	--trex.view.wireframe.load('t', {wireframe_rendering='texture_pass'}, 'root')
	metalcube.view.load('c', {x = -4010.0, y = 25.0, z = -2740.0}, {lit_rendering='texture_pass'}, 'root' )
	spherebump.view.load('s', {x = -4010.0, y = 135.0, z = -2740.0}, {lit_rendering='texture_pass'}, 'root' )

	spherebump.view.load('s2', {x = -4010.0, y = 18.0, z = -2740.0}, {lit_rendering='texture_pass'}, 'root' )
	

	boulder.view.load('rock', {x = -4010.0, y = 0.0, z = -2740.0}, {lit_rendering='texture_pass'}, 'root' )

    model.move.setpos('r', -4040.0, 0, -2740)
	model.move.setpos('t', -4088.0, 0, -2740)
	
	skyboxmod.view.load('skybox0', {layer0_rendering='texture_pass'}, 'root')
	
	model.env.setgravity(1)
	
	model.env.setbkcolor(0.55,0.55,0.99)

	model.camera.mvt:set_pos(-4000.0, 15.0, -2684.0)

	model.env.light.setstate( TRUE )
	model.env.light.setdir(1.0, -0.2, 0.0)
	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
