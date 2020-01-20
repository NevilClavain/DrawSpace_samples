

raptor = {}

raptor.dump = {}
raptor.view = {}

raptor.view.lit = {}
raptor.view.wireframe = {}

-- stockage des instances modeles : paire {entity, renderer}
raptor.models = {}

raptor.rendering_config = 
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
				{ path='raptorDif2.png', stage=0 },				
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
		normale_generation_mode = NORMALES_AUTO,
		tb_generation_mode = TB_AUTO		
		,
		normales_transform = commons.utils.init_matrix( { 
		                        1, 0,-1, 0,
								0, 1, 0, 0,
							    1, 0, 1, 0,
							    0, 0, 0, 1 } 
							 )
							 
	},
}

raptor.lit_material =
{
	--specular_power = 429.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	--bump_mapping = { texture_size = 1024, bias = 0.193 }
}


raptor.scale = 
{
	x = 0.12, y = 0.12, z = 0.12
}

raptor.dump.load = function()
   raptor.dump.entity = model.dump.load('raptor.dump.entity','raptor.fbx')
end

raptor.dump.unload = function()
   raptor.dump.entity = model.dump.unload(raptor.dump.entity)
   raptor.dump.entity = nil;
end

raptor.dump.show = function()
   model.dump.show(raptor.dump.entity)
end

raptor.update_lit_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = raptor.models[p_entity_id]['renderer']

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

raptor.update_wireframe_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = raptor.models[p_entity_id]['renderer']

	renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 1.0, 1.0, 1.0 )
end

raptor.createlitmodelview = function(p_rendergraph, p_entitygraph, p_pass_id, p_entity_id)
  
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(raptor.rendering_config, 'raptor.fbx', 'raptorMesh', {lit_rendering=p_pass_id})
  renderer:register_to_rendering(p_rendergraph)

  p_entitygraph:add_child('root',p_entity_id,entity)

  commons.apply_material( raptor.lit_material, renderer, p_pass_id)

  local pair = {}
  pair['entity'] = entity
  pair['renderer'] = renderer

  raptor.models[p_entity_id] = pair

  return entity
end

raptor.createwireframemodelview = function(p_rendergraph, p_entitygraph, p_pass_id, p_entity_id)

  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(raptor.rendering_config, 'raptor.fbx', 'raptorMesh', {wireframe_rendering=p_pass_id})
  renderer:register_to_rendering(p_rendergraph)

  p_entitygraph:add_child('root',p_entity_id,entity)

  local pair = { ['entity'] = entity, ['renderer'] = renderer }
  
  raptor.models[p_entity_id] = pair

  return entity
end

raptor.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = raptor.models[p_entity_id]['entity']
  local renderer = raptor.models[p_entity_id]['renderer']

  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = raptor.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  raptor.models[p_entity_id] = nil
end

raptor.models.dump = function()
  for k, v in pairs(raptor.models) do
    g:print("raptor entity instance -> "..k)
  end  
end

raptor.view.unload = function(p_entity_id)
 
  found_id = FALSE
  for k, v in pairs(raptor.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(raptor.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

raptor.view.load = function(p_entity_id)

  found_id = FALSE
  for k, v in pairs(raptor.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    raptor.view.lit.load(p_entity_id)
  end  
end

raptor.view.lit.load = function(p_entity_id)
  model.view.load(raptor.createlitmodelview, raptor.update_lit_from_scene_env, raptor.scale, p_entity_id)
end

raptor.view.wireframe.load = function(p_entity_id)
  model.view.load(raptor.createwireframemodelview, raptor.update_wireframe_from_scene_env, raptor.scale, p_entity_id)
end
