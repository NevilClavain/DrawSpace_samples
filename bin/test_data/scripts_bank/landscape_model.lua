

landscape = {}

landscape.dump = {}
landscape.view = {}

landscape.view.lit = {}
landscape.view.wireframe = {}

-- stockage des instances modeles : paire {entity, renderer}
landscape.models = {}


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

landscape.update_lit_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = landscape.models[p_entity_id]['renderer']

	renderer:set_shaderrealvector( p_pass_id, 'ambient_color', p_environment_table.ambient_light.r, p_environment_table.ambient_light.g, p_environment_table.ambient_light.b, p_environment_table.ambient_light.a )
	renderer:set_shaderrealvector( p_pass_id, 'lights_enabled', p_environment_table.lights_enabled.x, p_environment_table.lights_enabled.y, p_environment_table.lights_enabled.z, p_environment_table.lights_enabled.w )
	renderer:set_shaderrealvector( p_pass_id, 'light0_color', p_environment_table.light0.color.r, p_environment_table.light0.color.g, p_environment_table.light0.color.b, p_environment_table.light0.color.a )
	renderer:set_shaderrealvector( p_pass_id, 'light0_dir', p_environment_table.light0.direction.x, p_environment_table.light0.direction.y, p_environment_table.light0.direction.z, p_environment_table.light0.direction.w )

	renderer:set_shaderrealvector( p_pass_id, 'light0_dir_v', p_environment_table.light0.direction.x, p_environment_table.light0.direction.y, p_environment_table.light0.direction.z, p_environment_table.light0.direction.w )
	renderer:set_shaderrealvector( p_pass_id, 'lights_enabled_v', p_environment_table.lights_enabled.x, p_environment_table.lights_enabled.y, p_environment_table.lights_enabled.z, p_environment_table.lights_enabled.w )
	
	renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 0, p_environment_table.mirror)
	renderer:set_shaderrealinvector( p_pass_id, 'flags_v', 1, p_environment_table.fog_intensity)
	
	renderer:set_shaderrealvector( p_pass_id, 'fog_color', p_environment_table.fog_color.r, p_environment_table.fog_color.g, p_environment_table.fog_color.b, 1.0 )
		
	renderer:set_shaderrealvector( p_pass_id, 'reflectorPos', p_environment_table.reflector_pos.x, p_environment_table.reflector_pos.y, p_environment_table.reflector_pos.z, 1.0 )
	renderer:set_shaderrealvector( p_pass_id, 'reflectorNormale', p_environment_table.reflector_normale.x, p_environment_table.reflector_normale.y, p_environment_table.reflector_normale.z, 1.0 )
end

landscape.update_wireframe_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = landscape.models[p_entity_id]['renderer']

	renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 1.0, 1.0, 1.0 )
end

landscape.createlitmodelview = function(p_rendergraph, p_entitygraph, p_pass_id, p_entity_id)
  
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(landscape.rendering_config, 'land2.ac', 'wavefront obj', {lit_rendering=p_pass_id})
  renderer:register_to_rendering(p_rendergraph)

  p_entitygraph:add_child('root',p_entity_id,entity)

  commons.apply_material( landscape.lit_material, renderer, p_pass_id)

  local pair = {}
  pair['entity'] = entity
  pair['renderer'] = renderer

  landscape.models[p_entity_id] = pair

  return entity
end

landscape.createwireframemodelview = function(p_rendergraph, p_entitygraph, p_pass_id, p_entity_id)

  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(landscape.rendering_config, 'land2.ac', 'wavefront obj', {wireframe_rendering=p_pass_id})
  renderer:register_to_rendering(p_rendergraph)

  p_entitygraph:add_child('root',p_entity_id,entity)

  local pair = { ['entity'] = entity, ['renderer'] = renderer }
  
  landscape.models[p_entity_id] = pair

  return entity
end

landscape.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = landscape.models[p_entity_id]['entity']
  local renderer = landscape.models[p_entity_id]['renderer']

  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = landscape.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  landscape.models[p_entity_id] = nil
end

landscape.models.dump = function()
  for k, v in pairs(landscape.models) do
    g:print("landscape entity instance -> "..k)
  end  
end

landscape.view.unload = function(p_entity_id)
 
  found_id = FALSE
  for k, v in pairs(landscape.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(landscape.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

landscape.view.load = function(p_entity_id)

  found_id = FALSE
  for k, v in pairs(landscape.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    landscape.view.lit.load(p_entity_id)
  end  
end


landscape.view.lit.load = function(p_entity_id)
  model.view.load(landscape.createlitmodelview, landscape.update_lit_from_scene_env, { x = 30.0, y = 30.0, z = 30.0 }, p_entity_id, FALSE)
end

landscape.view.wireframe.load = function(p_entity_id)
  model.view.load(landscape.createwireframemodelview, landscape.update_wireframe_from_scene_env, { x = 30.0, y = 30.0, z = 30.0 }, p_entity_id, FALSE)
end
