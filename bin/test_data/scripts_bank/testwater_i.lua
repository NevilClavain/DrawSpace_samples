
reflectorPos = 
{
	x = 0.0,
	y = 0.0,
	z = 0.0, 
}

reflectorNormale = 
{
	x = 0.0,
	y = 1.0,
	z = 0.0, 
}

lights = 
{
	ambient_light = {r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = 1.0, y = -1.0, z = 0.0, w = 1.0 },
	}
}

renderers =
{
}
nb_renderers = 0;

renderers2 =
{
}
nb_renderers2 = 0;

hmi_mode=FALSE

renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

g:print('rendering infos : '..renderer_descr..', '..renderer_width..'x'..renderer_height..' fullscreen='..renderer_fullscreen..' viewport='..viewport_width..'x'..viewport_height)

sb_mod = Module("skyboxmod", "skybox")
sb_mod:load()
g:print(sb_mod:get_descr().. ' loaded')


commons.init_final_pass_water_mask(rg, 'final_pass')

rg:create_child('final_pass', 'texture_pass', 0)
rg:create_child('final_pass', 'texturemirror_pass', 1)


rg:create_child('final_pass', 'bump_pass', 2, RENDERPURPOSE_FLOATVECTOR)
rg:set_pass_depthclearstate('bump_pass', TRUE)
rg:set_pass_targetclearstate('bump_pass', TRUE)
rg:set_pass_targetclearcolor('bump_pass', 0, 0, 0, 0)



rg:create_child('final_pass', 'wave_pass', NO_TEXTURESTAGE_CONNECTION, RENDERPURPOSE_COLOR, RENDERTARGET_GPU, FALSE, 512, 512)

rg:create_pass_viewportquad('wave_pass')

wave_fxparams = FxParams()
wave_fxparams:add_shaderfile('water_waves.vso',SHADER_COMPILED)
wave_fxparams:add_shaderfile('water_waves.pso',SHADER_COMPILED)

wave_textures = TexturesSet()
wave_rendercontext = RenderContext('wave_pass')
wave_rendercontext:add_fxparams(wave_fxparams)
wave_rendercontext:add_texturesset(wave_textures)
wave_rendercontext:add_shaderparam("waves", 1, 0)

waves_renderconfig=RenderConfig()
waves_renderconfig:add_rendercontext(wave_rendercontext)
rg:configure_pass_viewportquad_resources('wave_pass',waves_renderconfig)


text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")

root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_ENABLED, 0.0, -9.81, 0.0)


camera_entity, fps_transfo=commons.create_fps_camera(0.0, 3.0, 0.0, viewport_width, viewport_height)
eg:add_child('root','camera_entity',camera_entity)


ground_entity_config = 
{ 
	texture_pass = 
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
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
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
		shaders_params =
		{
			{ param_name = "color", shader_index = 1, register = 0 }
		}
	},
	bump_pass = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='water_bump.vso',mode=SHADER_COMPILED },
				{ path='water_bump.pso',mode=SHADER_COMPILED }
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
		},
		vertex_textures =
		{
		},
		shaders_params =
		{
			{ param_name = 'bump_bias', shader_index = 1, register = 0 }
		}	
	}
}

ground_entity, ground_renderer = commons.create_rendered_meshe(rg, ground_entity_config, 'water.ac', 0)
ground_renderer:set_shaderrealvector('texture_pass', "color", 1.0, 0.0, 1.0, 1.0 )
ground_renderer:set_shaderrealvector('bump_pass', 'bump_bias', 1.0, 0.0, 0.0, 0.0)

ground_renderer:set_passnodetexturefrompass(rg, 'wave_pass', 'bump_pass', 0)
eg:add_child('root','ground_entity',ground_entity)

ground_entity:add_aspect(BODY_ASPECT)
ground_body=Body()

ground_body:attach_toentity(ground_entity)

ground_body:configure_shape( SHAPE_BOX, 100, 0.0, 100.0)


ground_body:configure_mode(COLLIDER_MODE)

ground_body:configure_state(TRUE)








cube_entity_config = 
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
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
			}
		},
		textures =
		{
			{ path='mars.jpg', stage=0 }
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	},
	texturemirror_pass = 
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
			{ path='mars.jpg', stage=0 }
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	}
}
cube_entity, cube_renderer = commons.create_rendered_meshe(rg, cube_entity_config, 'object.ac', 0)
eg:add_child('root','cube_entity',cube_entity)





cube_entity:add_aspect(BODY_ASPECT)
cube_body=Body()

cube_body:attach_toentity(cube_entity)
cube_body:configure_shape( SHAPE_BOX, 0.5, 0.5, 0.5)

cube_body:configure_mode(COLLIDER_MODE)

cube_body:configure_state(TRUE)


cube_rot_mat = Matrix()
cube_rot_mat:rotation( 0.0, 1.0, 0.0, commons.utils.deg_to_rad( 0.0 ) )



cube_pos_mat = Matrix()
cube_pos_mat:translation( 0.0, 2.0, -15.0 )


cube_final_mat = Matrix()
cube_final_mat:set_product( cube_rot_mat, cube_pos_mat)

cube_body:configure_attitude(cube_final_mat)


cube_material =
{
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 }
}

commons.apply_material(cube_material, cube_renderer, 'texture_pass')
commons.apply_material(cube_material, cube_renderer, 'texturemirror_pass')

renderers[nb_renderers] = cube_renderer
nb_renderers = nb_renderers + 1







sphere_entity_config = 
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
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
			}
		},
		textures =
		{
			{ path='marbre.jpg', stage=0},
			{ path='bump.bmp', stage=1}
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	},
	texturemirror_pass = 
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
			{ path='marbre.jpg', stage=0},
			{ path='bump.bmp', stage=1}
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	}
}
sphere_entity,sphere_renderer = commons.create_rendered_meshe(rg, sphere_entity_config, 'sphere.ac', 0)
eg:add_child('root','sphere_entity',sphere_entity)


sphere_entity:add_aspect(BODY_ASPECT)
sphere_body=Body()

sphere_body:attach_toentity(sphere_entity)

sphere_body:configure_shape(SHAPE_SPHERE, 1.0)


sphere_pos_mat = Matrix()

sphere_pos_mat:translation( -2.0, 3.0, -2.8 )

sphere_body:configure_attitude(sphere_pos_mat)

sphere_body:configure_mass(80.0)

sphere_body:configure_mode(BODY_MODE)

sphere_body:configure_state(FALSE)


sphere_material =
{
	specular_power = 200.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 1.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 512, bias = 0.45 }
}

commons.apply_material(sphere_material, sphere_renderer, 'texture_pass')
commons.apply_material(sphere_material, sphere_renderer, 'texturemirror_pass')

renderers[nb_renderers] = sphere_renderer
nb_renderers = nb_renderers + 1








land_entity_config = 
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
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
			}
		},
		textures =
		{
			{ path='012b2su2.jpg', stage=0 },
			{ path='grass_bump.bmp', stage=1 }
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	},
	texturemirror_pass = 
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
			{ path='012b2su2.jpg', stage=0 },
			{ path='grass_bump.bmp', stage=1 }
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	}
}

land_entity, land_renderer = commons.create_rendered_meshe(rg, land_entity_config, 'land2.ac', 0)
eg:add_child('root','land_entity',land_entity)

land_entity:add_aspect(BODY_ASPECT)
land_body=Body()

land_body:attach_toentity(land_entity)

land_body:configure_shape(SHAPE_MESHE, 'land2.ac', 0)


land_body:configure_mode(COLLIDER_MODE)

land_body:configure_state(TRUE)

land_material =
{
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 1024, bias = 0.333 }
}

commons.apply_material( land_material, land_renderer, 'texture_pass')
commons.apply_material(land_material, land_renderer, 'texturemirror_pass')

renderers[nb_renderers] = land_renderer
nb_renderers = nb_renderers + 1







commons.update_lights( 'texture_pass', lights, renderers )
commons.update_lights( 'texturemirror_pass', lights, renderers )


commons.setup_mirror_off( 'texture_pass', renderers )
commons.setup_mirror_on( 'texturemirror_pass', renderers, reflectorPos, reflectorNormale )




skybox_entity, skybox_renderer, sb_transform = commons.create_skybox_with_mirror( 'texture_pass', 'texturemirror_pass', rg, sb_mod, "sb0.bmp", "sb2.bmp", "sb3.bmp", "sb1.bmp", "sb4.bmp", "sb4.bmp", 1000.0)
eg:add_child('root','skybox_entity',skybox_entity)

skybox_renderer:set_shaderrealvector( 'texturemirror_pass', 'reflector_pos', 0.0, 0.0, 0.0, 1.0)
skybox_renderer:set_shaderrealvector( 'texturemirror_pass', 'reflector_normale', 0.0, 1.0, 0.0, 1.0)


-- ///////////////////////////////

eg:set_camera(camera_entity)

rg:update_renderingqueues()



fps_yaw=SyncAngle()
fps_pitch=SyncAngle()

cube_rot=SyncAngle()

waves=SyncAngle()

waves_inc = TRUE


fps_yaw:init_fromtimeaspectof(root_entity, 0.0)
fps_pitch:init_fromtimeaspectof(root_entity,0.0)

cube_rot:init_fromtimeaspectof(root_entity,0.0)

waves:init_fromtimeaspectof(root_entity,0.0)



g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

	if hmi_mode == TRUE then
		gui:on_mousemove( xm, ym, dx, dy )
	else
  
		fps_yaw:inc(-dx / 1.0)

		fps_pitch:inc(-dy / 1.0)

		fps_infos = { fps_transfo:read() }
		fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], fps_infos[9]) 

	end
end)

g:add_mouseleftbuttondowncb( "onmouselbdown",
function( xm, ym )
	if hmi_mode == TRUE then
		gui:on_mouseleftbuttondown() 
	end
end)

g:add_mouseleftbuttonupcb( "onmouselbup",
function( xm, ym )
	if hmi_mode == TRUE then
		gui:on_mouseleftbuttonup()
	end
end)


g:add_mouserightbuttondowncb( "onmouserbdown",
function( xm, ym )
	if hmi_mode == TRUE then
		gui:on_mouserightbuttondown()
	end
end)

g:add_mouserightbuttonupcb( "onmouserbup",
function( xm, ym )
	if hmi_mode == TRUE then
		gui:on_mouserightbuttonup()
	end
end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 

    if hmi_mode == FALSE then
      fps_infos = { fps_transfo:read() }
      fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 12.0) 
	end
  
      
  --W key
  elseif key == 87 then

    if hmi_mode == FALSE then
      fps_infos = { fps_transfo:read() }
      fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], -12.0) 
	end

  end

end)

g:add_keyupcb( "keyup",
function( key )  



  --Q key
  if key == 81 then

    if hmi_mode==FALSE then
      fps_infos = { fps_transfo:read() }
      fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 0.0) 
    end
    
  --W key
  elseif key == 87 then

    if hmi_mode == FALSE then
      fps_infos = { fps_transfo:read() }
      fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 0.0) 
	end
  -- VK_F1
  elseif key == 112 then

    if hmi_mode == FALSE then
      hmi_mode = TRUE
      g:show_mousecursor(TRUE)
      g:set_mousecursorcircularmode(FALSE)


    elseif hmi_mode==TRUE then
      hmi_mode=FALSE
      g:show_mousecursor(FALSE)
      g:set_mousecursorcircularmode(TRUE)
    end
  
  end

end)


g:add_appruncb( "run",
function()  

	time_infos = { root_entity:read_timemanager() }
	output_infos = renderer:descr() .." "..time_infos[3].. " fps debug = "..waves:get_value()

	text_renderer:update(520, 30, 255, 0, 0, output_infos)

	cube_rot:inc( 10.0 )
	
	cube_rot_mat:rotation( 0.0, 0.0, 1.0, commons.utils.deg_to_rad( cube_rot:get_value() ) )
	--cube_transform:update_matrix("cube_rot",cube_rot_mat)

	cube_final_mat:set_product( cube_rot_mat, cube_pos_mat)

	cube_body:update_attitude(cube_final_mat)


    if waves_inc == TRUE then

        if waves:get_value() < 200.0 then
            waves:inc( 1.0 )
        else
            waves_inc = FALSE
		end
    else

        if waves:get_value() > 0.0 then
            waves:dec( 1.0 )
        else
            waves_inc = TRUE
		end
	end

	rg:set_viewportquadshaderrealvector('wave_pass', 'waves', waves:get_value(), 0.0, 0.0, 0.0, 0.0)


end)



gui=Gui()
gui:init()
gui:set_resourcespath("./testskin")
gui:load_scheme("AlfiskoSkin.scheme")
gui:load_layout("main.layout","testskin/layouts/main_widgets.conf")
gui:set_layout("main.layout")
gui:show_gui(TRUE)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

y_cube = 1


cube_instances = {}

add_cube = function()

	local cube_name = 'cube_entity'..y_cube

	local cube_infos = {}

	local cube_entity
	local cube_renderer

	local cube_entity_config = 
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
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
				},
				rs_out =
				{
					{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
				}
			},
			textures =
			{
				{ path='Bloc1.jpg', stage=0 }
			},
			vertex_textures =
			{
			},

			shaders_params = commons.setup_lit_shader_params()
		}
		,
		texturemirror_pass = 
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
				{ path='Bloc1.jpg', stage=0 }
			},
			vertex_textures =
			{
			},

			shaders_params = commons.setup_lit_shader_params()	
		}
	}
	cube_entity, cube_renderer = commons.create_rendered_meshe(rg, cube_entity_config, 'object.ac', 0)
	eg:add_child('root',cube_name,cube_entity)


	cube_entity:add_aspect(BODY_ASPECT)
	local cube_body=Body()

	cube_body:attach_toentity(cube_entity)

	cube_body:configure_shape( SHAPE_BOX, 0.5, 0.5, 0.5)

	local cube_pos_mat = Matrix()

	cube_pos_mat:translation( 0.7, 9.0, -15.0 )

	cube_body:configure_attitude(cube_pos_mat)

	cube_body:configure_mass(7.0)

	cube_body:configure_mode(BODY_MODE)

	cube_body:configure_state(TRUE)


	local cube_material =
	{
		color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
		self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 }
	}

	commons.apply_material( cube_material, cube_renderer, 'texture_pass')
	commons.apply_material(cube_material, cube_renderer, 'texturemirror_pass')



	renderers2[nb_renderers2] = cube_renderer
	nb_renderers2 = nb_renderers2 + 1

	commons.update_lights( 'texture_pass', lights, renderers2 )
	commons.update_lights( 'texturemirror_pass', lights, renderers2 )

	commons.setup_mirror_off( 'texture_pass', renderers2 )
	commons.setup_mirror_on( 'texturemirror_pass', renderers2, reflectorPos, reflectorNormale )


	cube_infos['renderer'] = cube_renderer
	cube_infos['entity'] = cube_entity
	cube_infos['body'] = cube_body;


	cube_instances[y_cube] = cube_infos

	y_cube = y_cube + 1

end



destroy_all_cubes = function()

	for k, v in pairs(cube_instances) do
	
		local cube_infos = v

		cube_infos['body']:release()
		cube_infos['body']:detach_fromentity(cube_infos['entity'])
		cube_infos['entity']:remove_aspect(BODY_ASPECT)
		commons.trash.meshe(rg, cube_infos['entity'], cube_infos['renderer'])


		cube_infos['entity'] = nil
		cube_infos['renderer'] = nil
		cube_infos['body'] = nil;

		cube_instances[k] = nil

		local cube_name = 'cube_entity'..k
		eg:remove(cube_name)

	end	

	renderers2 = nil
	renderers2 =
	{
	}
	nb_renderers2 = 0;

end



gui:add_pushbuttonclickedcb( "onpushbutton",
function( layout, widget )
  
  if layout == 'main.layout' and widget == "Quit" then
    g:quit()
  
  elseif layout == 'main.layout' and widget == "Button_Create" then

	add_cube()	
	rg:update_renderingqueues()

  elseif layout == 'main.layout' and widget == "Button_Destroy" then
	
	destroy_all_cubes()	
	y_cube = 1
	rg:update_renderingqueues()
	
  end
end)


set_water_bump = function(bias)
	ground_renderer:set_shaderrealvector('bump_pass', 'bump_bias', bias, 0.0, 0.0, 0.0)
end

g:signal_renderscenebegin("eg")


