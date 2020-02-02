
metalcube = {}
metalcube.dump = {}
metalcube.view = {}

-- stockage des instances modeles : paire {entity, renderer, body}
metalcube.models = {}


metalcube.rendering_config = 
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
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
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
				{ path='tech_color.jpg', stage=0},
				{ path='tech_bump.jpg', stage=1},
				{ path='tech_mask.bmp', stage=2}
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
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO,
		tb_generation_mode = TB_AUTO
	}
}

metalcube.lit_material =
{
    mask_mode = 1,
	specular_power = 13.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 2800, bias = 0.1 }
}

metalcube.dump.load = function()
   metalcube.dump.entity = model.dump.load('metalcube.dump.entity','tech.ac')
end

metalcube.dump.unload = function()
   metalcube.dump.entity = model.dump.unload(metalcube.dump.entity)
   metalcube.dump.entity = nil;
end

metalcube.dump.show = function()
   model.dump.show(metalcube.dump.entity)
end

metalcube.update_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = metalcube.models[p_entity_id]['renderer']

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

metalcube.createlitmodelview = function(p_rendergraph, p_entitygraph, p_pass_id, p_entity_id, p_initialpos)
  
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(metalcube.rendering_config, 'tech.ac', 'box', {lit_rendering=p_pass_id})
  renderer:register_to_rendering(p_rendergraph)

  entity:add_aspect(BODY_ASPECT)

  local cube_body=Body()

  cube_body:attach_toentity(entity)
  cube_body:configure_shape( SHAPE_BOX, 0.5, 0.5, 0.5)

  local cube_pos_mat = Matrix()
  cube_pos_mat:translation( p_initialpos.x, p_initialpos.y, p_initialpos.z )
  cube_body:configure_attitude(cube_pos_mat)
  cube_body:configure_mass(7.0)
  cube_body:configure_mode(BODY_MODE)

  p_entitygraph:add_child('root',p_entity_id,entity)

  commons.apply_material( metalcube.lit_material, renderer, p_pass_id)

  local pair = {}
  pair['entity'] = entity
  pair['renderer'] = renderer
  pair['body'] = cube_body

  metalcube.models[p_entity_id] = pair

  return entity
end

metalcube.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = metalcube.models[p_entity_id]['entity']
  local renderer = metalcube.models[p_entity_id]['renderer']
  local body = metalcube.models[p_entity_id]['body']

  body:release()
  body:detach_fromentity(entity)

  entity:remove_aspect(BODY_ASPECT)

  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = metalcube.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil
  pair['body'] = nil

  metalcube.models[p_entity_id] = nil
end

metalcube.models.dump = function()
  for k, v in pairs(metalcube.models) do
    g:print("metalcube entity instance -> "..k)
  end  
end

metalcube.view.unload = function(p_entity_id)
 
  found_id = FALSE
  for k, v in pairs(metalcube.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(metalcube.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

metalcube.view.load = function(p_entity_id, p_initialpos)

  found_id = FALSE
  for k, v in pairs(metalcube.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.loadbody(metalcube.createlitmodelview, metalcube.update_from_scene_env, nil, p_entity_id, p_initialpos)
  end  
end