
commons = {}

commons.utils = {}
commons.trash = {}

commons.utils.PI = 3.1415927

REFLECTIONS_OFF=0
REFLECTIONS_ON=1


commons.utils.deg_to_rad = function(angle)
	return ( ( angle * commons.utils.PI ) / 180.0 );
end

commons.init_final_pass = function(p_rendergraph, p_passid)

	p_rendergraph:create_pass_viewportquad(p_passid)

	local finalpass_rss=RenderStatesSet()

	finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
	finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")

	local textures = TexturesSet()

	local fxparams = FxParams()
	fxparams:add_shaderfile('texture.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('texture.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(finalpass_rss)


	rendercontext = RenderContext(p_passid)
	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)

	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)
	p_rendergraph:configure_pass_viewportquad_resources(p_passid,renderconfig)

end

commons.init_final_pass_water_mask = function(p_rendergraph, p_passid)

	p_rendergraph:create_pass_viewportquad(p_passid)

	local finalpass_rss=RenderStatesSet()

	finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
	finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")

	local textures = TexturesSet()

	local fxparams = FxParams()
	fxparams:add_shaderfile('water_mask.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('water_mask.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(finalpass_rss)


	rendercontext = RenderContext(p_passid)
	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)

	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)
	p_rendergraph:configure_pass_viewportquad_resources(p_passid,renderconfig)

end

commons.create_fps_camera = function(p_x, p_y, p_z, p_viewport_width, p_viewport_height)

	local camera_entity=Entity()
	camera_entity:add_aspect(TRANSFORM_ASPECT)
	camera_entity:add_aspect(CAMERA_ASPECT)

	camera_entity:configure_camera(p_viewport_width,p_viewport_height, 1.0, 1000000.0)

	local fps_transfo=FPSTransform()
	fps_transfo:configure(camera_entity,0,0,p_x,p_y,p_z,TRUE)

	return camera_entity, fps_transfo
end


commons.create_skybox_with_mirror = function(p_passid, p_mirrorpassid, p_rendergraph, p_module, p_front_tx, p_rear_tx, p_left_tx, p_right_tx, p_top_tx, p_bottom_tx, p_scale)

	local skybox_entity=Entity()
	skybox_entity:add_aspect(RENDERING_ASPECT)
	skybox_entity:add_aspect(TRANSFORM_ASPECT)


	local skybox_texture_front = TexturesSet()
	skybox_texture_front:set_texturefiletostage(p_front_tx, 0)
	local skybox_texture_rear = TexturesSet()
	skybox_texture_rear:set_texturefiletostage(p_rear_tx, 0)
	local skybox_texture_left = TexturesSet()
	skybox_texture_left:set_texturefiletostage(p_left_tx, 0)
	local skybox_texture_right = TexturesSet()
	skybox_texture_right:set_texturefiletostage(p_right_tx, 0)
	local skybox_texture_top = TexturesSet()
	skybox_texture_top:set_texturefiletostage(p_top_tx, 0)
	local skybox_texture_bottom = TexturesSet()
	skybox_texture_bottom:set_texturefiletostage(p_bottom_tx, 0)

	local skybox_fxparams = FxParams()
	skybox_fxparams:add_shaderfile('texture.vso',SHADER_COMPILED)
	skybox_fxparams:add_shaderfile('texture.pso',SHADER_COMPILED)
	skybox_rendercontext = RenderContext(p_passid)
	skybox_rendercontext:add_fxparams(skybox_fxparams)
	skybox_rendercontext:add_texturesset(skybox_texture_front)
	skybox_rendercontext:add_texturesset(skybox_texture_rear)
	skybox_rendercontext:add_texturesset(skybox_texture_left)
	skybox_rendercontext:add_texturesset(skybox_texture_right)
	skybox_rendercontext:add_texturesset(skybox_texture_top)
	skybox_rendercontext:add_texturesset(skybox_texture_bottom)
	skybox_rendercontext:set_renderingorder(-1000)


	if p_mirrorpassid ~= "" then
		local mirror_rss=RenderStatesSet()

		mirror_rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "false")
		mirror_rss:add_renderstate_in(RENDERSTATE_OPE_SETCULLING, "ccw")
		mirror_rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")
		mirror_rss:add_renderstate_out(RENDERSTATE_OPE_SETCULLING, "cw")

		local skybox_fxparams_mirror = FxParams()
		skybox_fxparams_mirror:add_shaderfile('texture_mirror.vso',SHADER_COMPILED)
		skybox_fxparams_mirror:add_shaderfile('texture_mirror.pso',SHADER_COMPILED)
		skybox_fxparams_mirror:set_renderstatesset(mirror_rss)
	
		skybox_rendercontext_mirror = RenderContext(p_mirrorpassid)
		skybox_rendercontext_mirror:add_fxparams(skybox_fxparams_mirror)
		skybox_rendercontext_mirror:add_texturesset(skybox_texture_front)
		skybox_rendercontext_mirror:add_texturesset(skybox_texture_rear)
		skybox_rendercontext_mirror:add_texturesset(skybox_texture_left)
		skybox_rendercontext_mirror:add_texturesset(skybox_texture_right)
		skybox_rendercontext_mirror:add_texturesset(skybox_texture_top)
		skybox_rendercontext_mirror:add_texturesset(skybox_texture_bottom)
		skybox_rendercontext_mirror:set_renderingorder(-1000)

		skybox_rendercontext_mirror:add_shaderparam("reflector_pos", 0, 24)
		skybox_rendercontext_mirror:add_shaderparam("reflector_normale", 0, 25)
	end
	

	skybox_renderconfig=RenderConfig()
	skybox_renderconfig:add_rendercontext(skybox_rendercontext)

	if p_mirrorpassid ~= "" then
		skybox_renderconfig:add_rendercontext(skybox_rendercontext_mirror)
	end


	local skybox_renderer = SkyboxRendering()
	skybox_renderer:instanciate_renderingimpl(p_module)
	skybox_renderer:attach_toentity(skybox_entity)
	skybox_renderer:configure(skybox_renderconfig)

	skybox_renderer:register_to_rendering(p_rendergraph)

	sb_scale = Matrix();
	sb_scale:scale(p_scale, p_scale, p_scale)

	local sb_transform = RawTransform()
	sb_transform:configure(skybox_entity)
	sb_transform:add_matrix("sb_scaling",sb_scale)

	return skybox_entity, skybox_renderer, sb_transform
end

commons.create_rendered_meshe = function(p_rendergraph, p_config, p_meshefile, p_meshe_index)

	local meshe_entity=Entity()
	meshe_entity:add_aspect(RENDERING_ASPECT)
	meshe_entity:add_aspect(TRANSFORM_ASPECT)

	local renderconfig=RenderConfig()

	local renderer=MesheRendering()
	renderer:attach_toentity(meshe_entity)

	for k, v in pairs(p_config) do
		--g:print(k)
		local rendercontext = RenderContext(k)

		local fxparams = FxParams()
		local fx_config = v['fx']

		local rss=RenderStatesSet()

		local rs_in_config = fx_config['rs_in']
		for k2, v2 in pairs(rs_in_config) do
			local curr_rs_in = v2
			--g:print(curr_rs_in['ope']..'->'..curr_rs_in['value'])
			rss:add_renderstate_in(curr_rs_in['ope'], curr_rs_in['value'])
		end

		local rs_out_config = fx_config['rs_out']
		for k2, v2 in pairs(rs_out_config) do
			local curr_rs_out = v2
			--g:print(curr_rs_out['ope']..'->'..curr_rs_out['value'])
			rss:add_renderstate_out(curr_rs_out['ope'], curr_rs_out['value'])
		end
		fxparams:set_renderstatesset(rss)

		local shaders_config = fx_config['shaders']
		for k2, v2 in pairs(shaders_config) do
			local curr_shader = v2
			--g:print(curr_shader['path']..'->'..curr_shader['mode'])
			fxparams:add_shaderfile(curr_shader['path'],curr_shader['mode'])
		end

		local textures = TexturesSet()
		local tx_config = v['textures']
		for k2, v2 in pairs(tx_config) do
			local tx = v2
			--g:print(tx['path']..'->'..tx['stage'])
			textures:set_texturefiletostage(tx['path'], tx['stage'])
		end

		local vtextures = TexturesSet()
		local vtx_config = v['vertex_textures']
		for k2, v2 in pairs(vtx_config) do
			local vtx = v2
			--g:print(vtx['path']..'->'..vtx['stage'])
			vtextures:set_texturefiletostage(vtx['path'], vtx['stage'])
		end

		local shaderparams_config = v['shaders_params']
		for k2, v2 in pairs(shaderparams_config) do
			local param = v2
			--g:print(param['param_name']..'->'..param['shader_index']..','..param['register'])
			rendercontext:add_shaderparam(param['param_name'], param['shader_index'], param['register'])
		end

		rendercontext:add_fxparams(fxparams)
		rendercontext:add_texturesset(textures)
		rendercontext:add_vertextexturesset(vtextures)

		renderconfig:add_rendercontext(rendercontext)
	end

	renderer:configure(renderconfig, p_meshefile, p_meshe_index)

	renderer:register_to_rendering(p_rendergraph)

	return meshe_entity, renderer
end

-- create a table with all required parameters for lit vertex and pixel shaders (lit.vso, lit.pso)
commons.setup_lit_shader_params = function()

	shaders_params_table = {}
	
	shaders_params_table[0] = { param_name = "lights_enabled_v", shader_index = 0, register = 24 }
	shaders_params_table[1] = { param_name = "light0_dir_v", shader_index = 0, register = 25 }
	shaders_params_table[2] = { param_name = "flags_v", shader_index = 0, register = 26 }
	shaders_params_table[3] = { param_name = "reflectorPos", shader_index = 0, register = 27 }
	shaders_params_table[4] = { param_name = "reflectorNormale", shader_index = 0, register = 28 }
	shaders_params_table[5] = { param_name = "ambient_color", shader_index = 1, register = 0 }
	shaders_params_table[6] = { param_name = "lights_enabled", shader_index = 1, register = 1 }
	shaders_params_table[7] = { param_name = "light0_color", shader_index = 1, register = 2 }
	shaders_params_table[8] = { param_name = "light0_dir", shader_index = 1, register = 3 }
	shaders_params_table[9] = { param_name = "flags", shader_index = 1, register = 7 }
	shaders_params_table[10] = { param_name = "self_emissive", shader_index = 1, register = 8 }
	shaders_params_table[11] = { param_name = "absorption", shader_index = 1, register = 9 }
	shaders_params_table[12] = { param_name = "color", shader_index = 1, register = 10 }
	shaders_params_table[13] = { param_name = "color_source", shader_index = 1, register = 11 }
	shaders_params_table[14] = { param_name = "fog_color", shader_index = 1, register = 12 }

	return shaders_params_table
end



commons.update_lights = function( p_pass_id, p_lights_table, p_renderer_tables )

	for k, v in pairs(p_renderer_tables) do
		
		local renderer = v

		renderer:set_shaderrealvector( p_pass_id, 'ambient_color', p_lights_table.ambient_light.r, p_lights_table.ambient_light.g, p_lights_table.ambient_light.b, p_lights_table.ambient_light.a )
		renderer:set_shaderrealvector( p_pass_id, 'lights_enabled', p_lights_table.lights_enabled.x, p_lights_table.lights_enabled.y, p_lights_table.lights_enabled.z, lights.lights_enabled.w )
		renderer:set_shaderrealvector( p_pass_id, 'light0_color', p_lights_table.light0.color.r, p_lights_table.light0.color.g, p_lights_table.light0.color.b, lights.light0.color.a )
		renderer:set_shaderrealvector( p_pass_id, 'light0_dir', p_lights_table.light0.direction.x, p_lights_table.light0.direction.y, p_lights_table.light0.direction.z, lights.light0.direction.w )

		renderer:set_shaderrealvector( p_pass_id, 'light0_dir_v', p_lights_table.light0.direction.x, p_lights_table.light0.direction.y, p_lights_table.light0.direction.z, lights.light0.direction.w )
		renderer:set_shaderrealvector( p_pass_id, 'lights_enabled_v', p_lights_table.lights_enabled.x, p_lights_table.lights_enabled.y, p_lights_table.lights_enabled.z, lights.lights_enabled.w )
	end
end


commons.setup_lit_flags = function( p_pass_id, p_renderer_tables, p_mirror, p_reflector_pos, p_reflector_normale, p_fog_intensity, p_fog_color )
	for k, v in pairs(p_renderer_tables) do
		local renderer = v
		renderer:set_shaderrealvector( p_pass_id, 'flags_v', p_mirror, p_fog_intensity, 0.0, 0.0 )
		renderer:set_shaderrealvector( p_pass_id, 'fog_color', p_fog_color.r, p_fog_color.g, p_fog_color.b, 1.0 )
		
		renderer:set_shaderrealvector( p_pass_id, 'reflectorPos', p_reflector_pos.x, p_reflector_pos.y, p_reflector_pos.z, 0.0 )
		renderer:set_shaderrealvector( p_pass_id, 'reflectorNormale', p_reflector_normale.x, p_reflector_normale.y, p_reflector_normale.z, 0.0 )
	end
end

commons.setup_lit_flags_simple = function( p_pass_id, p_renderer_tables, p_fog_intensity, p_fog_color )
	for k, v in pairs(p_renderer_tables) do
		local renderer = v
		renderer:set_shaderrealvector( p_pass_id, 'flags_v', 0.0, p_fog_intensity, 0.0, 0.0 )
		renderer:set_shaderrealvector( p_pass_id, 'fog_color', p_fog_color.r, p_fog_color.g, p_fog_color.b, 1.0 )
	end
end

commons.apply_material = function(p_material, p_renderer, p_pass_id)
	
	local texture_size = 512.0
	local bump_bias = -1.0
	local specular_power = -1.0
	local mask_mode = 0


	if p_material['bump_mapping'] ~= nil then
	  bump_bias = p_material['bump_mapping']['bias']
	  texture_size = p_material['bump_mapping']['texture_size']
	end

	if p_material['specular_power'] ~= nil then
		specular_power = p_material['specular_power']
	end

	if p_material['mask_mode'] ~= nil then
		mask_mode = p_material['mask_mode']
	end

	p_renderer:set_shaderrealvector( p_pass_id, 'flags', specular_power, bump_bias, texture_size, mask_mode )

	if p_material['color_source'] ~= nil then
		p_renderer:set_shaderrealvector( p_pass_id, 'color_source', p_material['color_source']['r'], p_material['color_source']['g'], p_material['color_source']['b'], p_material['color_source']['a'] )
	end

	if p_material['simple_color'] ~= nil then
		p_renderer:set_shaderrealvector( p_pass_id, 'color', p_material['simple_color']['r'], p_material['simple_color']['g'], p_material['simple_color']['b'], p_material['simple_color']['a'] )
	end

	if p_material['self_emissive'] ~= nil then
		p_renderer:set_shaderrealvector( p_pass_id, 'self_emissive', p_material['self_emissive']['r'], p_material['self_emissive']['g'], p_material['self_emissive']['b'], p_material['self_emissive']['a'] )
	end

	if p_material['light_absorption'] ~= nil then
		p_renderer:set_shaderrealvector( p_pass_id, 'absorption', p_material['light_absorption']['r'], p_material['light_absorption']['g'], p_material['light_absorption']['b'], p_material['light_absorption']['a'] )
	end
end


commons.trash.skybox = function(p_rendergraph, p_module, p_entity, p_renderer, p_transform)

  p_transform:release()

  p_renderer:unregister_from_rendering(p_rendergraph)
  p_renderer:release()
  p_renderer:detach_fromentity()
  p_renderer:trash_renderingimpl(p_module)
  
  p_entity:remove_aspect(TRANSFORM_ASPECT)
  p_entity:remove_aspect(RENDERING_ASPECT)

end

commons.trash.meshe = function(p_rendergraph, p_entity, p_renderer)

	p_renderer:unregister_from_rendering(p_rendergraph)
	p_renderer:release()
	p_renderer:detach_fromentity()

	p_entity:remove_aspect(TRANSFORM_ASPECT)
	p_entity:remove_aspect(RENDERING_ASPECT)

end

commons.trash.fps_camera = function(p_camera_entity, p_fps_transfo)
	p_fps_transfo:release()

	p_camera_entity:release_camera()
	p_camera_entity:remove_aspect(TRANSFORM_ASPECT)
	p_camera_entity:remove_aspect(CAMERA_ASPECT)
end

commons.trash.final_pass = function(p_rendergraph, p_passid)
	p_rendergraph:release_pass_viewportquad_resources(p_passid)
	p_rendergraph:remove_pass_viewportquad(p_passid)
end

-- ready-to-use materials

commons.unlit_texture_material =
{
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	self_emissive = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
}

commons.unlit_color_material =
{
	color_source = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	simple_color = { r = 0.0, g = 1.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	self_emissive = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
}
