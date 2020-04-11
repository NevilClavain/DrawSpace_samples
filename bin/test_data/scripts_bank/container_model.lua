
container = {}

container.dump = {}
container.view = {}

container.view.lit = {}

-- stockage des instances modeles : paire {entity, renderer}
container.models = {}

container.rendering_config = 
{ 
	flatcolor_rendering = 
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
			{ param_name = "color", shader_index = 1, register = 0 },
		}
	},	
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO,
		tb_generation_mode = TB_AUTO,
															
		normales_transform = commons.utils.init_matrix( { 
		                        1, 0,-1, 0,
								0, 1, 0, 0,
							    1, 0, 1, 0,
							    0, 0, 0, 1 } 
							 )							 							 
	},
}

container.scale = 
{
	x = 30.0, y = 4.0, z = 30.0
}

container.update_flatcolor = function( p_pass_id, p_r, p_g, p_b, p_a, p_entity_id )

    local renderer = container.models[p_entity_id]['renderer']
    renderer:set_shaderrealvector( p_pass_id, 'color', p_r, p_g, p_b, p_a )
end

container.createmodelview = function(p_rendergraph, p_entitygraph, p_entity_id, p_passes_bindings, p_parent_entity_id)
  
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(container.rendering_config, 'object.ac', 'box', p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)

 
  p_entitygraph:add_child(p_parent_entity_id,p_entity_id,entity)

  local pair = {}
  pair['entity'] = entity
  pair['renderer'] = renderer

  container.models[p_entity_id] = pair

  return entity
end

container.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = container.models[p_entity_id]['entity']
  local renderer = container.models[p_entity_id]['renderer']

  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = container.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  container.models[p_entity_id] = nil
end

container.view.unload = function(p_entity_id)
 
  local found_id = FALSE
  for k, v in pairs(container.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(container.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

container.view.load = function(p_entity_id, p_passes_config, p_parent_entity_id)

  local found_id = FALSE
  for k, v in pairs(container.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('container model', container.createmodelview, p_passes_config, nil, container.scale, p_entity_id, p_parent_entity_id)
  end
end