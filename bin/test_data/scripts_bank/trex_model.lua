
g:print('hello from trex model!')

trex = {}

trex.inspect = {}

trex.inspect.load = function()
  trex.inspect.entity = model.load('t_rex_model_inspect_entity','Trex.fbx')
end

trex.inspect.unload = function()
  model.unload(t_rex.model_inspect.entity)
  trex.inspect.entity = nil
end


trex.entity_config = 
{ 
	texture_pass = 
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
				--{ path='raptorDif2.png', stage=0 },
				{ path='Trex_difuse.png', stage=0 },
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

trex.material =
{
	--specular_power = 429.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	--bump_mapping = { texture_size = 1024, bias = 0.193 }
}