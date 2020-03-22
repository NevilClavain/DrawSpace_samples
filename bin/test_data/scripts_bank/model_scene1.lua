

	include('skyboxmod_model.lua')
	include('raymarching_model.lua')
	
	
	skyboxmod.view.load('skybox0', {layer0_rendering='texture_pass'}, 'root')
	

	raymarching.view.load('raymarch', {main_rendering='texture_pass'}, 'root')
	model.move.setpos('raymarch', 0.0, 0.0, 0.0)

	
	model.env.setbkcolor(0.55,0.55,0.99)

	model.camera.mvt:set_pos(0.0, 0.0, 8.0)
	model.camera.speed = 3.0
