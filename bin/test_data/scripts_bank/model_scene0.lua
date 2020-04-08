

	include('continent_model.lua')	
	include('raptor_model.lua')
	include('trex_model.lua')
	include('metalcube_model.lua')
    include('spherebump_model.lua')
	include('skyboxmod_model.lua')
	include('boulder_model.lua')
	
	
	continent_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'lit_rendering',
			lit_shader_update_func = continent.update_lit_from_scene_env
		}
	}	
	continent.view.load('continent', continent_passes_config, 'root')
	
	raptor_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'lit_rendering',
			lit_shader_update_func = raptor.update_from_scene_env
		}
	}
	raptor.view.load('r', raptor_passes_config, 'root')
	model.move.setpos('r', -4040.0, 0, -2740)


	trex_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'lit_rendering',
			lit_shader_update_func = trex.update_from_scene_env
		}
	}
	trex.view.load('t', trex_passes_config, 'root')
	model.move.setpos('t', -4088.0, 0, -2740)
	

	metalcube_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'lit_rendering',
			lit_shader_update_func = metalcube.update_from_scene_env
		}
	}
	metalcube.view.load('c', {x = -4010.0, y = 25.0, z = -2740.0}, metalcube_passes_config, 'root' )

	spherebump_passes_config = 
	{
		texture_pass = 
		{
	        rendering_id = 'lit_rendering',
			lit_shader_update_func = spherebump.update_from_scene_env
		}
	}
	spherebump.view.load('s', {x = -4010.0, y = 135.0, z = -2740.0}, spherebump_passes_config, 'root' )
	spherebump.view.load('s2', {x = -4010.0, y = 18.0, z = -2740.0}, spherebump_passes_config, 'root' )
	
	boulder_passes_config = 
	{
	    texture_pass = 
	    {
			rendering_id = 'lit_rendering',
			lit_shader_update_func = boulder.update_lit_from_scene_env
		}
	}
	boulder.view.load('rock', {x = -4010.0, y = 0.0, z = -2740.0}, boulder_passes_config, 'root' )

	skybox_passes_config = 
	{
		texture_pass = 
		{
			rendering_id = 'layer0_rendering',
			lit_shader_update_func = nil
		}
	}
	skyboxmod.view.load('skybox0', skybox_passes_config, 'root')
	
	model.env.setgravity(1)
	
	model.env.setbkcolor(0.55,0.55,0.99)

	model.camera.mvt:set_pos(-4000.0, 15.0, -2684.0)

	model.env.light.setstate( TRUE )
	model.env.light.setdir(1.0, -0.2, 0.0)
	model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
