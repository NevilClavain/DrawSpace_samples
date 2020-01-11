

landscape = {}

landscape.dump = {}
landscape.view = {}

landscape.view.lit = {}
landscape.view.wireframe = {}


landscape.rendering_config = 
{ 
    lit_rendering = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='lit.vso',mode=SHADER_COMPILED },
				{ path='lit.pso',mode=SHADER_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" }
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
			}
		},
		textures =
		{
			[1] = 
			{
				{ path='012b2su2.jpg', stage=0 },
				{ path='grass_bump.bmp', stage=1 }
			}
		},
		vertex_textures =
		{
		},
		rendering_order = 10000,
		shaders_params = 
		{ 
			{ param_name = "lights_enabled_v", shader_index = 0, register = 24 },
			{ param_name = "light0_dir_v", shader_index = 0, register = 25 },
			{ param_name = "flags_v", shader_index = 0, register = 26 },
			{ param_name = "reflectorPos", shader_index = 0, register = 27 },
			{ param_name = "reflectorNormale", shader_index = 0, register = 28 },
			{ param_name = "ambient_color", shader_index = 1, register = 0 },
			{ param_name = "lights_enabled", shader_index = 1, register = 1 },
			{ param_name = "light0_color", shader_index = 1, register = 2 },
			{ param_name = "light0_dir", shader_index = 1, register = 3 },
			{ param_name = "flags", shader_index = 1, register = 7 },
			{ param_name = "self_emissive", shader_index = 1, register = 8 },
			{ param_name = "absorption", shader_index = 1, register = 9 },
			{ param_name = "color", shader_index = 1, register = 10 },
			{ param_name = "color_source", shader_index = 1, register = 11 },
			{ param_name = "fog_color", shader_index = 1, register = 12 }
		}
	},
	wireframe_rendering =
	{
		fx = 
		{
			shaders = 
			{
				{ path='color.vso',mode=SHADER_COMPILED },
				{ path='color.pso',mode=SHADER_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
				{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
				{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" }
			}
		},
		textures =
		{
		},
		vertex_textures =
		{
		},
		rendering_order = 10000,
		shaders_params = 
		{ 
		    { param_name = "color", shader_index = 1, register = 0 },
		}	
	},	
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_AUTO
	},
}

landscape.lit_material =
{
	--specular_power = 429.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	--bump_mapping = { texture_size = 1024, bias = 0.193 }
}

landscape.dump.load = function()
   landscape.dump.entity = model.dump.load('landscape.dump.entity','land2.ac')
end

landscape.dump.unload = function()
   landscape.dump.entity = model.dump.unload(landscape.dump.entity)
   landscape.dump.entity = nil;
end

landscape.dump.show = function()
   model.dump.show(landscape.dump.entity)
end

landscape.update_lit_from_scene_env = function( p_pass_id, p_environment_table )

	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'ambient_color', p_environment_table.ambient_light.r, p_environment_table.ambient_light.g, p_environment_table.ambient_light.b, p_environment_table.ambient_light.a )
	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'lights_enabled', p_environment_table.lights_enabled.x, p_environment_table.lights_enabled.y, p_environment_table.lights_enabled.z, p_environment_table.lights_enabled.w )
	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'light0_color', p_environment_table.light0.color.r, p_environment_table.light0.color.g, p_environment_table.light0.color.b, p_environment_table.light0.color.a )
	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'light0_dir', p_environment_table.light0.direction.x, p_environment_table.light0.direction.y, p_environment_table.light0.direction.z, p_environment_table.light0.direction.w )

	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'light0_dir_v', p_environment_table.light0.direction.x, p_environment_table.light0.direction.y, p_environment_table.light0.direction.z, p_environment_table.light0.direction.w )
	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'lights_enabled_v', p_environment_table.lights_enabled.x, p_environment_table.lights_enabled.y, p_environment_table.lights_enabled.z, p_environment_table.lights_enabled.w )

	
	landscape.view.renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 0, p_environment_table.mirror)
	landscape.view.renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 1, p_environment_table.fog_intensity)
	
	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'fog_color', p_environment_table.fog_color.r, p_environment_table.fog_color.g, p_environment_table.fog_color.b, 1.0 )
		
	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'reflectorPos', p_environment_table.reflector_pos.x, p_environment_table.reflector_pos.y, p_environment_table.reflector_pos.z, 1.0 )
	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'reflectorNormale', p_environment_table.reflector_normale.x, p_environment_table.reflector_normale.y, p_environment_table.reflector_normale.z, 1.0 )
end

landscape.update_wireframe_from_scene_env = function( p_pass_id, p_environment_table )

	landscape.view.renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 1.0, 1.0, 1.0 )
end

landscape.createlitmodelview = function(p_rendergraph, p_entitygraph, p_pass_id, p_entity_id)

  g:print('adding entity : '..p_entity_id)
  landscape.view.entity, landscape.view.renderer = commons.create_rendered_meshe(landscape.rendering_config, 'land2.ac', 'wavefront obj', {lit_rendering=p_pass_id})
  landscape.view.renderer:register_to_rendering(p_rendergraph)

  p_entitygraph:add_child('root','landscape.view.entity',landscape.view.entity)

  commons.apply_material( landscape.lit_material, landscape.view.renderer, p_pass_id)

  return landscape.view.entity
end

landscape.createwireframemodelview = function(p_rendergraph, p_entitygraph, p_pass_id, p_entity_id)

  g:print('adding entity : '..p_entity_id)
  landscape.view.entity, landscape.view.renderer = commons.create_rendered_meshe(landscape.rendering_config, 'land2.ac', 'wavefront obj', {wireframe_rendering=p_pass_id})
  landscape.view.renderer:register_to_rendering(p_rendergraph)

  p_entitygraph:add_child('root','landscape.view.entity',landscape.view.entity)

  return landscape.view.entity
end

landscape.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  g:print('trashing entity : '..p_entity_id)
  commons.trash.meshe(p_rendergraph, landscape.view.entity, landscape.view.renderer)
  p_entitygraph:remove('landscape.view.entity')
end


landscape.view.unload = function(p_entity_id)
  model.view.unload(landscape.trashmodelview,p_entity_id)
end

landscape.view.load = function(p_entity_id)
  landscape.view.lit.load(p_entity_id)
end


landscape.view.lit.load = function(p_entity_id)
  model.view.load(landscape.createlitmodelview, landscape.update_lit_from_scene_env, p_entity_id)
end

landscape.view.wireframe.load = function(p_entity_id)
  model.view.load(landscape.createwireframemodelview, landscape.update_wireframe_from_scene_env, p_entity_id)
end
