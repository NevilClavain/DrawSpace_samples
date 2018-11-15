
commons = {}

commons.utils = {}
commons.trash = {}

commons.procedural = {}

commons.procedural.nebulae = {}


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

commons.create_fps_camera = function(p_x, p_y, p_z, p_viewport_width, p_viewport_height, p_module)

	local camera_entity=Entity()
	camera_entity:add_aspect(TRANSFORM_ASPECT)
	camera_entity:add_aspect(CAMERA_ASPECT)

	camera_entity:configure_camera(p_viewport_width,p_viewport_height, 1.0, 1000000.0)

	local fps_transfo=FPSTransform()
	fps_transfo:instanciate_transformimpl(p_module)
	fps_transfo:configure(camera_entity,0,0,p_x,p_y,p_z,TRUE)

	return camera_entity, fps_transfo
end

commons.create_free_camera = function(p_x, p_y, p_z, p_viewport_width, p_viewport_height, p_module)
	local camera_entity=Entity()
	camera_entity:add_aspect(TRANSFORM_ASPECT)
	camera_entity:add_aspect(CAMERA_ASPECT)

	camera_entity:configure_camera(p_viewport_width,p_viewport_height, 1.0, 1000000.0)

	local free_transfo=FreeTransform()
	free_transfo:instanciate_transformimpl(p_module)
	free_transfo:configure(camera_entity,0,p_x, p_y, p_z)

	return camera_entity, free_transfo
end

commons.create_rendering_from_module = function(p_config, p_module, p_rendering_impl_id, p_specific_config)

	local renderconfig=RenderConfig()

	local entity=Entity()
	entity:add_aspect(RENDERING_ASPECT)
	
	for k, v in pairs(p_config) do
		g:print(k)
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

		
		local tx_config = v['textures']		
		for k2, v2 in ipairs(tx_config) do
			--g:print(k2)
			local textures = TexturesSet()
			for k3, v3 in pairs(v2) do
				local tx = v3
				--g:print(tx['path']..'->'..tx['stage'])
				textures:set_texturefiletostage(tx['path'], tx['stage'])
			end
			rendercontext:add_texturesset(textures)
		end
		
		
		local vtx_config = v['vertex_textures']
		for k2, v2 in ipairs(vtx_config) do

			local vtextures = TexturesSet()
			for k3, v3 in pairs(v2) do
				local tx = v3
				--g:print(tx['path']..'->'..tx['stage'])
				vtextures:set_texturefiletostage(tx['path'], tx['stage'])
			end
			rendercontext:add_vertextexturesset(vtextures)
		end

		local shaderparams_config = v['shaders_params']
		for k2, v2 in pairs(shaderparams_config) do
			local param = v2
			--g:print(param['param_name']..'->'..param['shader_index']..','..param['register'])
			rendercontext:add_shaderparam(param['param_name'], param['shader_index'], param['register'])
		end

		local ro = v['rendering_order']
		--g:print( 'ro ='..ro )
		rendercontext:set_renderingorder(ro)

		rendercontext:add_fxparams(fxparams)
		renderconfig:add_rendercontext(rendercontext)
	end

	local rendering=Rendering()
	rendering:instanciate_renderingimpl(p_module,p_rendering_impl_id)
	rendering:attach_toentity(entity)
	rendering:configure(renderconfig)

	return entity,rendering
end

commons.create_rendered_meshe = function(p_config, p_meshefile, p_meshe_index)

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

		local tx_config = v['textures']		
		for k2, v2 in ipairs(tx_config) do
			--g:print(k2)
			local textures = TexturesSet()
			for k3, v3 in pairs(v2) do
				local tx = v3
				--g:print(tx['path']..'->'..tx['stage'])
				textures:set_texturefiletostage(tx['path'], tx['stage'])
			end
			rendercontext:add_texturesset(textures)
		end
		
		
		local vtx_config = v['vertex_textures']
		for k2, v2 in ipairs(vtx_config) do

			local vtextures = TexturesSet()
			for k3, v3 in pairs(v2) do
				local tx = v3
				--g:print(tx['path']..'->'..tx['stage'])
				vtextures:set_texturefiletostage(tx['path'], tx['stage'])
			end
			rendercontext:add_vertextexturesset(vtextures)
		end

		local shaderparams_config = v['shaders_params']
		for k2, v2 in pairs(shaderparams_config) do
			local param = v2
			--g:print(param['param_name']..'->'..param['shader_index']..','..param['register'])
			rendercontext:add_shaderparam(param['param_name'], param['shader_index'], param['register'])
		end

		local ro = v['rendering_order']
		--g:print( 'ro ='..ro )
		rendercontext:set_renderingorder(ro)

		rendercontext:add_fxparams(fxparams)

		renderconfig:add_rendercontext(rendercontext)
	end

	renderer:configure(renderconfig, p_meshefile, p_meshe_index)

	

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





commons.procedural.nebulae.generate_texture_uv_coords = function(nebulae_specific_configuration, bloc_index, random_engine, list_size, min, max)
  
  local distr=Distribution("uniform_int_distribution", min, max)

  for i = 0, list_size - 1, 1 do
    local u = distr:generate(random_engine)
	local v = distr:generate(random_engine)
	nebulae_specific_configuration:add_bloctextureuvpair(bloc_index, u, v)
  end  
end

commons.procedural.nebulae.generate_mask_uv_coords = function(nebulae_specific_configuration, bloc_index, random_engine, list_size, min, max)
  
  local distr=Distribution("uniform_int_distribution", min, max)

  for i = 0, list_size - 1, 1 do
    local u = distr:generate(random_engine)
	local v = distr:generate(random_engine)
	nebulae_specific_configuration:add_blocmaskuvpair(bloc_index, u, v)
  end  
end







commons.trash.rendering = function(p_rendergraph, p_module, p_entity, p_renderer)

	p_renderer:unregister_from_rendering(p_rendergraph)
	p_renderer:release()
	p_renderer:detach_fromentity()
	p_renderer:trash_renderingimpl(p_module)

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


