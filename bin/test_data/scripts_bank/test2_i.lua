


lights = 
{
	ambient_light = {r = 0.33, g = 0.33, b = 0.33, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = 1.0, y = -0.50, z = 0.0, w = 1.0 },
	}
}

renderers =
{
}
nb_renderers = 0;


ctrl_key = FALSE
last_key = 0

renderer_infos = {renderer:descr()}
g:print('Current renderer is '..renderer_infos[1]..', '..renderer_infos[2]..'x'..renderer_infos[3])



commons.init_final_pass(rg, 'final_pass')
rg:create_child('final_pass', 'texture_pass', 0)


text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 10, 40, 255, 0, 255, "??? fps")


root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_ENABLED, 0.0, -9.81, 0.0)



camera_entity, fps_transfo=commons.create_fps_camera(0.0, 3.0, 20.0, renderer_infos[5],renderer_infos[6])
eg:add_child('root','camera_entity',camera_entity)


ground_entity_config = 
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
			{ path='bloc1.jpg', stage=0 }
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	}
}

ground_entity, ground_renderer = commons.create_rendered_meshe(rg, ground_entity_config, 'water.ac', 0)
eg:add_child('root','ground_entity',ground_entity)

ground_entity:add_aspect(BODY_ASPECT)
ground_body=Body()

ground_body:attach_toentity(ground_entity)

ground_body:configure_shape( SHAPE_BOX, 100, 0.0, 100.0)


ground_body:configure_mode(COLLIDER_MODE)

ground_body:configure_state(TRUE)

ground_material =
{
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
}

commons.apply_material( ground_material, ground_renderer, 'texture_pass')

renderers[nb_renderers] = ground_renderer
nb_renderers = nb_renderers + 1

clothbox_entity_config = 
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
			{ path='clothbox.jpg', stage=0},
			{ path='box_bump.bmp', stage=1}
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	}
}



clothbox_entity,clothbox_renderer = commons.create_rendered_meshe(rg, clothbox_entity_config, 'mythcloth.ac', 0)
eg:add_child('root','clothbox_entity',clothbox_entity)

clothbox_material =
{
	specular_power = 180.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 2800, bias = 0.1 }
}

commons.apply_material( clothbox_material, clothbox_renderer, 'texture_pass')

renderers[nb_renderers] = clothbox_renderer
nb_renderers = nb_renderers + 1


cube_roty_mat = Matrix()

cube_rotx_mat = Matrix()

cube_pos_mat = Matrix()
cube_pos_mat:translation( 0.0, 2.0, -15.0 )



clothbox_transform = RawTransform()
clothbox_transform:configure(clothbox_entity)
clothbox_transform:add_matrix( "roty", cube_roty_mat )
clothbox_transform:add_matrix( "rotx", cube_rotx_mat )
clothbox_transform:add_matrix( "pos", cube_pos_mat )


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
	}
}
sphere_entity,sphere_renderer = commons.create_rendered_meshe(rg, sphere_entity_config, 'sphere.ac', 0)
eg:add_child('root','sphere_entity',sphere_entity)


sphere_entity:add_aspect(BODY_ASPECT)
sphere_body=Body()

sphere_body:attach_toentity(sphere_entity)

sphere_body:configure_shape( SHAPE_SPHERE, 1.0)


sphere_pos_mat = Matrix()

sphere_pos_mat:translation( 0.0, 7.0, 0.0 )

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

commons.apply_material( sphere_material, sphere_renderer, 'texture_pass')

renderers[nb_renderers] = sphere_renderer
nb_renderers = nb_renderers + 1

rock_entity_config = 
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
			{ path='rock08.bmp', stage=0}
		},
		vertex_textures =
		{
		},

		shaders_params = commons.setup_lit_shader_params()
	}
}
rock_entity,rock_renderer = commons.create_rendered_meshe(rg, rock_entity_config, 'rock.ac', 0)
eg:add_child('root','rock_entity',rock_entity)


rock_entity:add_aspect(BODY_ASPECT)
rock_body=Body()

rock_body:attach_toentity(rock_entity)

rock_body:configure_shape(SHAPE_SPHERE, 3.0)


rock_pos_mat = Matrix()

rock_pos_mat:translation( 1.0, 0.0, 0.0 )

rock_body:configure_attitude(rock_pos_mat)


rock_body:configure_mode(COLLIDER_MODE)

rock_body:configure_state(TRUE)


rock_material =
{
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
}

commons.apply_material( rock_material, rock_renderer, 'texture_pass')


renderers[nb_renderers] = rock_renderer
nb_renderers = nb_renderers + 1


--[[
rock_pos_mat = Matrix()
rock_pos_mat:translation( -9.0, 6.0, -20.0 )

rock_transform = RawTransform()
rock_transform:configure(rock_entity)
rock_transform:add_matrix( "pos", rock_pos_mat )

]]


commons.update_lights( 'texture_pass', lights, renderers )


-- ///////////////////////////////

eg:set_camera(camera_entity)

rg:update_renderingqueues()


cube_rot_y=SyncAngle()
cube_rot_x=SyncAngle()



fps_yaw=SyncAngle()
fps_pitch=SyncAngle()


fps_yaw:init_fromtimeaspectof(root_entity, 0.0)
fps_pitch:init_fromtimeaspectof(root_entity,0.0)

cube_rot_x:init_fromtimeaspectof(root_entity,0.0)
cube_rot_y:init_fromtimeaspectof(root_entity,0.0)



g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if ctrl_key == TRUE then

    
    cube_rot_y:inc(20.0 * dx)
	cube_rot_x:inc(20.0 * dy)

  else
  
    fps_yaw:inc(-dx / 1.0)

    fps_pitch:inc(-dy / 1.0)

    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], fps_infos[9]) 

  end

end)



g:add_keydowncb( "keydown",
function( key )



  --Q key
  if key == 81 then 
    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 12.0) 

  
      
  --W key
  elseif key == 87 then

    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], -12.0) 

  elseif key == 17 then
    ctrl_key = TRUE


  end

end)

g:add_keyupcb( "keyup",
function( key )  

  last_key = key

  --Q key
  if key == 81 then
    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 0.0) 

    
  --W key
  elseif key == 87 then
    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 0.0) 
 
  elseif key == 17 then
    ctrl_key = FALSE

    -- VK_F1
  elseif key == 112 then
	sphere_body:update_state(TRUE)
  end

end)


g:add_appruncb( "run",
function()  

  time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2].. " key = "..last_key

  text_renderer:update(10, 30, 255, 0, 0, output_infos)


  cube_roty_mat:rotation( 0.0, 1.0, 0.0, commons.utils.deg_to_rad( cube_rot_y:get_value() ) )
  clothbox_transform:update_matrix("roty",cube_roty_mat)

  cube_rotx_mat:rotation( 1.0, 0.0, 0.0, commons.utils.deg_to_rad( cube_rot_x:get_value() ) )
  clothbox_transform:update_matrix("rotx",cube_rotx_mat)


end)

g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)




