
skyboxmod = {}
skyboxmod.dump = {}
skyboxmod.view = {}

skyboxmod.module = Module("skyboxmod", "skybox")
skyboxmod.module:load()
g:print(skyboxmod.module:get_descr().. ' loaded')

-- stockage des instances modeles : paire {entity, renderer, body}
skyboxmod.models = {}

skyboxmod.layers =
{
	layer_0 = 
	{
		layer0_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='texture.vso',mode=SHADER_COMPILED },
					{ path='texture.pso',mode=SHADER_COMPILED }
				},
				rs_in = 
				{
				},
				rs_out =
				{
				}		
			},
			textures =
			{
				[1] = 
				{
					{path='sb0.bmp', stage=0}
				},
				[2] = 
				{
					{path='sb2.bmp', stage=0}
				},
				[3] = 
				{
					{path='sb3.bmp', stage=0}
				},
				[4] = 
				{
					{path='sb1.bmp', stage=0}
				},
				[5] = 
				{
					{path='sb4.bmp', stage=0}
				},
				[6] = 
				{
					{path='sb4.bmp', stage=0}
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
			},
			rendering_order = 10000
		},

		layer0_mirror_rendering =	
		{
			fx =
			{
				shaders = 
				{
					{ path='texture_mirror.vso',mode=SHADER_COMPILED },
					{ path='texture_mirror.pso',mode=SHADER_COMPILED }
				},
				rs_in = 
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="ccw" },		
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" },
				}		
			},
			textures =
			{
				[1] = 
				{
					{path='sb0.bmp', stage=0}
				},
				[2] = 
				{
					{path='sb2.bmp', stage=0}
				},
				[3] = 
				{
					{path='sb3.bmp', stage=0}
				},
				[4] = 
				{
					{path='sb1.bmp', stage=0}
				},
				[5] = 
				{
					{path='sb4.bmp', stage=0}
				},
				[6] = 
				{
					{path='sb4.bmp', stage=0}
				}
			},
			vertex_textures =
			{
			},
			shaders_params = 
			{
				{ param_name = "reflector_pos", shader_index = 0, register = 24 },
				{ param_name = "reflector_normale", shader_index = 0, register = 25 },
			},
			rendering_order = 10000
		}
	}
}

skyboxmod.update_from_scene_env = function( p_pass_id, p_environment_table, p_entity_id )

end
                                                                      -- VV ici on voudrait la liste de tout les passes dispos
skyboxmod.createmodelview = function(p_rendergraph, p_entitygraph, p_entity_id, p_passes_bindings, p_parent_entity_id)

  local entity
  local renderer

  entity, renderer=commons.create_rendering_from_module(skyboxmod.layers, skyboxmod.module, "skyboxRender", p_passes_bindings)
  renderer:register_to_rendering(p_rendergraph)

                           -- VV ca ne va pas !!! nom du pere en dur ! -> passer en arg !!!
  p_entitygraph:add_child(p_parent_entity_id,p_entity_id,entity)

  entity:add_aspect(TRANSFORM_ASPECT)

  local pair = { ['entity'] = entity, ['renderer'] = renderer, ['body'] = body }
  skyboxmod.models[p_entity_id] = pair

  return entity
  
end

skyboxmod.trashmodelview = function(p_rendergraph, p_entitygraph, p_entity_id)

  local entity = skyboxmod.models[p_entity_id]['entity']
  local renderer = skyboxmod.models[p_entity_id]['renderer']

  entity:remove_aspect(TRANSFORM_ASPECT)

  commons.trash.rendering(rg, skyboxmod.module, entity, renderer)
  p_entitygraph:remove(p_entity_id)

  local pair = skyboxmod.models[p_entity_id]
  pair['entity'] = nil
  pair['renderer'] = nil

  skyboxmod.models[p_entity_id] = nil

end


skyboxmod.view.unload = function(p_entity_id)
 
  local found_id = FALSE
  for k, v in pairs(skyboxmod.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
     model.view.unload(skyboxmod.trashmodelview, p_entity_id)
  else
    g:print('Unknown entity '..p_entity_id)
  end
end

skyboxmod.view.load = function(p_entity_id, p_passes_bindings, p_parent_entity_id)

  local found_id = FALSE
  for k, v in pairs(skyboxmod.models) do

    if k == p_entity_id then
	  found_id = TRUE
	end
  end

  if found_id == TRUE then
    g:print('Entity '..p_entity_id..' already exists')
  else
    model.view.load('continent model', skyboxmod.createmodelview, skyboxmod.update_from_scene_env, nil, {x = 1000.0, y = 1000.0, z = 1000.0}, p_entity_id, p_passes_bindings, p_parent_entity_id)
  end  
end
