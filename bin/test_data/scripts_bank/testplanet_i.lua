

renderers = {}
nb_renderers = 0

lights = 
{
	ambient_light = {r = 0.05, g = 0.05, b = 0.05, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = 3.0, y = 1.0, z = 0.0, w = 1.0 },
	}
}

create_sphere = function()

	local entity_config = 
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
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
					
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
					{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" }
				}
			},
			textures =
			{
				[1] = 
				{
					{ path='marbre.jpg', stage=0},
					{ path='bump.bmp', stage=1}
				}
			},
			vertex_textures =
			{
			},
			rendering_order = 10000,
			shaders_params = commons.setup_lit_shader_params()
		}
	}
	local entity = nil
	local renderer = nil
	entity,renderer = commons.create_rendered_meshe(entity_config, 'sphere.ac', 0)

	entity:add_aspect(INFOS_ASPECT)
	entity:setup_info( "entity_name", "bump mapped sphere" )

	renderer:register_to_rendering(rg)
	eg:add_child('root','sphere_entity',entity)

	--[[
	entity:add_aspect(BODY_ASPECT)
	local body=Body()

	body:attach_toentity(entity)

	body:configure_shape(SHAPE_SPHERE, 20.0)


	local sphere_pos_mat = Matrix()
	
	sphere_pos_mat:translation( 0.0, 0.0, -60.0 )
	

	body:configure_attitude(sphere_pos_mat)

	body:configure_mass(80.0)

	body:configure_mode(BODY_MODE)
	]]


	local scale = Matrix();
	scale:scale(1.0, 1.0, 1.0)

	local pos_mat = Matrix()
	pos_mat:translation( 0.0, 0.0, -62.0 )


	local transform = RawTransform()
	transform:configure(entity)
	transform:add_matrix("scaling",scale)
	transform:add_matrix( "pos", pos_mat )


	local sphere_material =
	{
		specular_power = 200.0,
		color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		simple_color = { r = 1.0, g = 0.0, b = 0.0, a = 0.0 },
		light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		bump_mapping = { texture_size = 512, bias = 0.45 }
	}

	commons.apply_material( sphere_material, renderer, 'texture_pass')

	--return entity,renderer,body
	return entity,renderer,transform
end



create_ship = function()

	local entity_config = 
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
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"}
				}
			},
			textures =
			{
				[1] = 
				{
					--{ path='SurveyShip_color.jpg', stage=0},
					--{ path='SurveyShip_color.jpg', stage=1}
					{ path='bellerophon.jpg', stage=0},
					{ path='bellerophon.jpg', stage=1}
				}
			},
			vertex_textures =
			{
			},
			rendering_order = 10000,
			shaders_params = commons.setup_lit_shader_params()
		}
	}

	local entity = nil
	local renderer = nil

	--entity,renderer = commons.create_rendered_meshe(ship_entity_config, 'survey.ac', 0)
	entity,renderer = commons.create_rendered_meshe(entity_config, 'bellerophon.ac', 0)
	entity:add_aspect(INFOS_ASPECT)
	entity:setup_info( "entity_name", "Space Ship" )

	renderer:register_to_rendering(rg)
	eg:add_child('root','ship_entity',entity)


	entity:add_aspect(BODY_ASPECT)
	local body=Body()

	body:attach_toentity(entity)

	body:configure_shape(SHAPE_BOX, 110.0, 80.0, 170.0)


	local ship_pos_mat = Matrix()

	ship_pos_mat:translation( -300.0, 0.0, -400.0 )

	body:configure_attitude(ship_pos_mat)

	body:configure_mass(50.0)

	body:configure_mode(BODY_MODE)


	ship_material =
	{
		specular_power = 200.0,
		color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		simple_color = { r = 1.0, g = 0.0, b = 0.0, a = 0.0 },
		light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		bump_mapping = { texture_size = 1024, bias = 0.45 }
	}

	commons.apply_material( ship_material, renderer, 'texture_pass')

	return entity,renderer,body
end

create_skybox = function()

	local entity_layers =
	{
		layer_0 = 
		{
			texture_pass =	
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
						{path='neb_front5.png', stage=0}
					},
					[2] = 
					{
						{path='neb_back6.png', stage=0}
					},
					[3] = 
					{
						{path='neb_left2.png', stage=0}
					},
					[4] = 
					{
						{path='neb_right1.png', stage=0}
					},
					[5] = 
					{
						{path='neb_top3.png', stage=0}
					},
					[6] = 
					{
						{path='neb_bottom4.png', stage=0}
					}
				},
				vertex_textures =
				{
				},
				shaders_params = 
				{
				},
				rendering_order = 1000
			}
		}
	}

	local entity = nil
	local renderer = nil

	entity,renderer=commons.create_rendering_from_module(entity_layers,sb_mod,"skyboxRender")

	entity:add_aspect(INFOS_ASPECT)
	entity:setup_info( "entity_name", "skybox" )

	renderer:register_to_rendering(rg)
	eg:add_child('root','skybox_entity',entity)


	entity:add_aspect(TRANSFORM_ASPECT)
	local sb_scale = Matrix();
	sb_scale:scale(1000.0, 1000.0, 1000.0)

	local transform = RawTransform()
	transform:configure(entity)
	transform:add_matrix("sb_scaling",sb_scale)

	return entity,renderer,transform
end

create_planet = function()

	local entity_layers =
	{ 
		surface_layer = 
		{
			texture_pass =	
			{
				fx =
				{
					shaders = 
					{
						{ path='planet_surface.vso',mode=SHADER_COMPILED },
						{ path='planet_surface.pso',mode=SHADER_COMPILED }
					},
					rs_in = 
					{
						{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }--[[,
						{ ope=RENDERSTATE_OPE_SETFILLMODE, value="line" }]]
						
					},
					rs_out =
					{
						{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"}--[[,
						{ ope=RENDERSTATE_OPE_SETFILLMODE, value="solid" }]]
					}		
				},
				textures =
				{
					[1] = 
					{
						{ path='earth_th_pixels_16.jpg', stage=0 },
						{ path='earth_th_splatting_16.jpg', stage=1 },
					}
				},
				vertex_textures =
				{
				},
				shaders_params = 
				{
				},
				rendering_order = 5000
			}
		}
		--[[ -- pour plus tard...
		,		
		atmo_layer = 
		{
			texture_pass =	
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
				},
				vertex_textures =
				{
				},
				shaders_params = 
				{
				},
				rendering_order = 10000
			}
		}
		]]
	}

	local planet_specific_config_descr =
	{
		resources_path						= "test_data/shaders_bank",
		climate_vshader						= "planet_ht.vso",
		climate_pshader						= "planet_ht.pso",
		planet_ray							= 6500.0,
		plains_amplitude					= 600.0,
		mountains_amplitude					= 16000.0,
		vertical_offset						= 20.0,
		mountains_offset					= 0.0,
		plains_seed1						= 8099.0,
		plains_seed2						= 5662.0,
		mix_seed1							= 3111.0,
		mix_seed2							= 498.0,
		terrainbump_factor					= 16.0,
		splat_transition_up_relative_alt	= 1.095,
		splat_transition_down_relative_alt	= 1.0040,
		splat_texture_resol					= 16,
		atmo_kr								= 0.0033,
		fog_alt_limit						= 30000.0,
		fog_density							= 0.000031,
		beach_limit							= 25.0,
		landplace_patch						= FALSE,

		ambient_light = 
		{
			state = FALSE,
			color = 
			{
				r = 1.0,
				g = 0.0,
				b = 0.0
			}
		},

		light0 = 
		{
			state = TRUE,
			color = 
			{
				r = 1.0,
				g = 0.9,
				b = 1.0
			},
			dir = 
			{
				x = 0.0,
				y = 1.0,
				z = 0.5
			}
		},

		light1= 
		{
			state = FALSE,
			color = 
			{
				r = 1.0,
				g = 0.0,
				b = 0.0
			},
			dir = 
			{
				x = 1.0,
				y = 0.0,
				z = 0.0
			}
		},

		light2 = 
		{
			state = FALSE,
			color = 
			{
				r = 0.0,
				g = 0.0,
				b = 0.0
			},
			dir = 
			{
				x = 0.0,
				y = 0.0,
				z = 1.0
			}
		}
	}

	local entity = nil
	local renderer = nil

	entity,renderer=commons.create_rendering_from_module(entity_layers,pl_mod,"planetsRender")

	local specific_config = PlanetSpecificConfig()
	commons.procedural.planet.setup_specific_config(planet_specific_config_descr, specific_config)
	specific_config:apply(renderer)


	entity:add_aspect(INFOS_ASPECT)
	entity:setup_info( "entity_name", "test planet" )

	renderer:register_to_rendering(rg)

	eg:add_child('root','planet_entity',entity)

	entity:add_aspect(TRANSFORM_ASPECT)

	return entity,renderer,specific_config
end

set_camera = function(camera)

  if camera == free_cam then
    eg:set_camera(camera_entity)
  elseif camera == ship_cam then
    --eg:set_camera(camera2_entity) temporaire !
  end
end

free_cam = 0
ship_cam = 1

current_cam = free_cam

fog_intensity = 0.0

fog_color = 
{
	r = 0.75,
	g = 0.75,
	b = 0.99, 
}



mouse_right = FALSE


renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

g:print('rendering infos : '..renderer_descr..', '..renderer_width..'x'..renderer_height..' fullscreen='..renderer_fullscreen..' viewport='..viewport_width..'x'..viewport_height)

sb_mod = Module("skyboxmod", "skybox")
sb_mod:load()
g:print(sb_mod:get_descr().. ' loaded')

mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

pl_mod = Module("planetsmod", "planets")
pl_mod:load()
g:print(pl_mod:get_descr().. ' loaded')


commons.init_final_pass(rg, 'final_pass')

rg:create_child('final_pass', 'texture_pass', 0)
rg:set_pass_targetclearstate( 'texture_pass', FALSE )
rg:set_pass_depthclearstate( 'texture_pass', TRUE )



text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")


root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_DISABLED)


camera_entity, camera_mvt=commons.create_free_camera(0.0, 0.0, 0.0, viewport_width,viewport_height, mvt_mod, "free_camera")
eg:add_child('root','camera_entity',camera_entity)



commons.update_lights( 'texture_pass', lights, renderers )


commons.setup_lit_flags( 'texture_pass', renderers, REFLECTIONS_OFF, reflectorPos, reflectorNormale, fog_intensity, fog_color)



planet_entity,planet_renderer,planet_specific_config = create_planet()
planet_transform = RawTransform()
planet_transform:configure(planet_entity)

planet_pos_mat = Matrix()
planet_pos_mat:translation( 0.0, 0.0, -40620000.0 )
planet_transform:add_matrix( "pos", planet_pos_mat )




skybox_entity,skybox_renderer,sb_transform = create_skybox()



--sphere_entity, sphere_renderer, sphere_transform = create_sphere()
--renderers[nb_renderers] = sphere_renderer
--nb_renderers = nb_renderers + 1

--ship_entity, ship_renderer, ship_body = create_ship()
--renderers[nb_renderers] = ship_renderer
--nb_renderers = nb_renderers + 1


--[[
camera2_entity, camera2_pos=commons.create_static_camera(0.0, 110.0, 300.0, viewport_width,viewport_height, mvt_mod, "ship_camera")
eg:add_child('ship_entity','camera2_entity',camera2_entity)
]]







-- ///////////////////////////////


commons.update_lights( 'texture_pass', lights, renderers )
commons.setup_lit_flags_simple( 'texture_pass', renderers, fog_intensity, fog_color)








set_camera(current_cam)

rg:update_renderingqueues()



g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

	local mvt_info = { camera_mvt:read() }

	if mouse_right == FALSE then
	  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
	else
	  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
	end

end)

g:add_mouseleftbuttondowncb( "onmouselbdown",
function( xm, ym )
end)

g:add_mouseleftbuttonupcb( "onmouselbup",
function( xm, ym )
end)


g:add_mouserightbuttondowncb( "onmouserbdown",
function( xm, ym )
	mouse_right = TRUE
end)

g:add_mouserightbuttonupcb( "onmouserbup",
function( xm, ym )
	mouse_right = FALSE
end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 
    
    local mvt_info = { camera_mvt:read() }
	camera_mvt:update(12.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  --W key
  elseif key == 87 then
    
    local mvt_info = { camera_mvt:read() }
	camera_mvt:update(-12.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
  
  end

end)

g:add_keyupcb( "keyup",
function( key )  

  --Q key
  if key == 81 then
    
    local mvt_info = { camera_mvt:read() }

	camera_mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)


  --W key
  elseif key == 87 then

    local mvt_info = { camera_mvt:read() }

	camera_mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)


  -- VK_F1
  elseif key == 112 then  

    current_cam = current_cam + 1
	if current_cam == 2 then
	  current_cam = 0
	end

	set_camera(current_cam)
  end

end)


g:add_appruncb( "run",
function()


  time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "

  text_renderer:update(10, 30, 255, 0, 0, output_infos)

  local mvt_info = { camera_mvt:read() }
  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)



end)





g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)


g:signal_renderscenebegin("eg")



