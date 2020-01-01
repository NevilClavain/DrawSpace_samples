

landscape = {}

landscape.dump = {}
landscape.view = {}


landscape.rendering_config = 
{ 
    landscape_rendering = 
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
		shaders_params = commons.setup_lit_shader_params()
	},

	meshes_loader_params =
	{
		normale_generation_mode = NORMALES_AUTO_SMOOTH,
		tb_generation_mode = TB_AUTO
	},
}

landscape.material =
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


landscape.createmodelview = function(p_rendergraph, p_entitygraph, p_pass_id)

  landscape.view.entity, landscape.view.renderer = commons.create_rendered_meshe(landscape.rendering_config, 'land2.ac', 'wavefront obj', {landscape_rendering=p_pass_id})
  landscape.view.renderer:register_to_rendering(p_rendergraph)

  p_entitygraph:add_child('root','landscape.view.entity',landscape.view.entity)

  commons.apply_material( landscape.material, landscape.view.renderer, p_pass_id)

  return landscape.view.renderer  
end

landscape.trashmodelview = function(p_rendergraph, p_entitygraph)
  commons.trash.meshe(p_rendergraph, landscape.view.entity, landscape.view.renderer)
  p_entitygraph:remove('landscape.view.entity')
end


landscape.view.unload = function()
  model.view.unload(landscape.trashmodelview)
end

landscape.view.load = function()
  model.view.load(landscape.createmodelview)
end
