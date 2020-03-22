
raymarching = {}

raymarching.dump = {}
raymarching.view = {}

raymarching.view.lit = {}
raymarching.view.wireframe = {}

-- stockage des instances modeles : paire {entity, renderer}
raymarching.models = {}

raymarching.rendering_config = 
{ 
    main_rendering = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='raymarch.vso',mode=SHADER_COMPILED },
				{ path='raymarch.pso',mode=SHADER_COMPILED }
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
		},
		vertex_textures =
		{
		},
		rendering_order = 10000,
		shaders_params = 
		{ 
			{ param_name = "color", shader_index = 1, register = 0 }
		}
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_AUTO
	},
}

raymarching.scale = 
{
	x = 1.0, y = 1.0, z = 1.0
}

raymarching.update_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

    local renderer = raymarching.models[p_entity_id]['renderer']
	renderer:set_shaderrealvector( p_pass_id, 'color', 1.0, 0.0, 0.0, 1.0 )

end

raymarching.createmodelview = function(p_rendergraph, p_entitygraph, p_entity_id, p_passes_bindings, p_parent_entity_id)
  
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(raymarching.rendering_config, 'object.ac', 'box', p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)

  p_entitygraph:add_child(p_parent_entity_id,p_entity_id,entity)

  local pair = { ['entity'] = entity, ['renderer'] = renderer, ['body'] = body }

  raymarching.models[p_entity_id] = pair

  return entity
end

raymarching.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = raymarching.models[p_entity_id]['entity']
  local renderer = raymarching.models[p_entity_id]['renderer']

  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = raymarching.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  raymarching.models[p_entity_id] = nil
end

raymarching.view.unload = function(p_entity_id)
 
  local found_id = FALSE
  for k, v in pairs(raymarching.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(raymarching.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

raymarching.view.load = function(p_entity_id, p_passes_bindings, p_parent_entity_id)

  local found_id = FALSE
  for k, v in pairs(raymarching.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('raymarching model', raymarching.createmodelview, raymarching.update_from_scene_env, nil, raymarching.scale, p_entity_id, p_passes_bindings, p_parent_entity_id)
  end
end