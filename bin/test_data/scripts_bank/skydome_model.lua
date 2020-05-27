skydome = {}

skydome.dump = {}
skydome.view = {}

-- stockage des instances modeles : paire {entity, renderer}
skydome.models = {}

skydome.rendering_config = 
{ 
	main_rendering =
	{
		fx = 
		{
			shaders = 
			{
				{ path='skydome.vso',mode=SHADER_COMPILED },
				{ path='skydome.pso',mode=SHADER_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
				--{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" },
				{ ope=RENDERSTATE_OPE_SETCULLING, value="ccw" }
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
				--{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" },
				{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
			}
		},
		textures =
		{
		},
		vertex_textures =
		{
		},
		rendering_order = 22,
		shaders_params = 
		{ 
			{ param_name = "color", shader_index = 1, register = 0 },
		}	
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_DISCARDED
	},
	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_DISCARDED
	},
}

skydome.dump.load = function()
   skydome.dump.entity = model.dump.load('skydome.dump.entity','skydome.ac')
end

skydome.dump.unload = function()
   skydome.dump.entity = model.dump.unload(skydome.dump.entity)
   skydome.dump.entity = nil;
end

skydome.dump.show = function()
   model.dump.show(skydome.dump.entity)
end

skydome.update_flatcolor = function( p_pass_id, p_r, p_g, p_b, p_a, p_entity_id )

    local renderer = skydome.models[p_entity_id]['renderer']
    renderer:set_shaderrealvector( p_pass_id, 'color', p_r, p_g, p_b, p_a )
end

skydome.createmodelview = function(p_rendergraph, p_entitygraph, p_entity_id, p_passes_bindings, p_parent_entity_id)
 
  local entity
  local renderer

  entity, renderer = commons.create_rendered_meshe(skydome.rendering_config, 'skydome.ac', 'sphere', p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)
  
 
  p_entitygraph:add_child(p_parent_entity_id,p_entity_id,entity)

  local pair = { ['entity'] = entity, ['renderer'] = renderer }

  skydome.models[p_entity_id] = pair

  return entity
end

skydome.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = skydome.models[p_entity_id]['entity']
  local renderer = skydome.models[p_entity_id]['renderer']


  commons.trash.meshe(p_rendergraph, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = skydome.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  skydome.models[p_entity_id] = nil
end

skydome.view.unload = function(p_entity_id)
 
  local found_id = FALSE
  for k, v in pairs(skydome.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(skydome.trashmodelview,p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end


skydome.scale = 
{
	x = 7000000.0, y = 7000000.0, z = 7000000.0
}

skydome.view.load = function(p_entity_id, p_passes_config, p_parent_entity_id)

  local found_id = FALSE
  for k, v in pairs(skydome.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('skydome model', skydome.createmodelview, p_passes_config, nil, skydome.scale, p_entity_id, p_parent_entity_id)
  end
end