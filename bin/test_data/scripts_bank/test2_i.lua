


lights = 
{
	ambient_light = {r = 0.15, g = 0.0, b = 0.0, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = 1.0, y = -1.0, z = -6.0, w = 1.0 },
	}
}

renderers =
{
}
nb_renderers = 0;

update_lights = function( p_pass_id, p_lights_table, p_renderer_tables )

	for k, v in pairs(p_renderer_tables) do
		
		local renderer = v

		renderer:set_shaderrealvector( p_pass_id, 'ambient_color', p_lights_table.ambient_light.r, p_lights_table.ambient_light.g, p_lights_table.ambient_light.b, p_lights_table.ambient_light.a )
		renderer:set_shaderrealvector( p_pass_id, 'lights_enabled', p_lights_table.lights_enabled.x, p_lights_table.lights_enabled.y, p_lights_table.lights_enabled.z, lights.lights_enabled.w )
		renderer:set_shaderrealvector( p_pass_id, 'light0_color', p_lights_table.light0.color.r, p_lights_table.light0.color.g, p_lights_table.light0.color.b, lights.light0.color.a )
		renderer:set_shaderrealvector( p_pass_id, 'light0_dir', p_lights_table.light0.direction.x, p_lights_table.light0.direction.y, p_lights_table.light0.direction.z, lights.light0.direction.w )

		renderer:set_shaderrealvector( p_pass_id, 'light0_dir_v', p_lights_table.light0.direction.x, p_lights_table.light0.direction.y, p_lights_table.light0.direction.z, lights.light0.direction.w )
		renderer:set_shaderrealvector( p_pass_id, 'lights_enabled_v', p_lights_table.lights_enabled.x, p_lights_table.lights_enabled.y, p_lights_table.lights_enabled.z, lights.lights_enabled.w )


	end
end



ctrl_key = FALSE
last_key = 0

renderer_infos = {renderer:descr()}
g:print('Current renderer is '..renderer_infos[1]..', '..renderer_infos[2]..'x'..renderer_infos[3])



commons.init_final_pass(rg, 'final_pass')
rg:create_child('final_pass', 'texture_pass', 0)


text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 10, 40, 255, 0, 255, "??? fps")


camera_entity, fps_transfo=commons.create_fps_camera(0.0, 3.0, 0.0, renderer_infos[5],renderer_infos[6])
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
		shaders_params =
		{
		    { param_name = "lights_enabled_v", shader_index = 0, register = 24 },
			{ param_name = "light0_dir_v", shader_index = 0, register = 25 },
			{ param_name = "ambient_color", shader_index = 1, register = 0 },
			{ param_name = "lights_enabled", shader_index = 1, register = 1 },
			{ param_name = "light0_color", shader_index = 1, register = 2 },
			{ param_name = "light0_dir", shader_index = 1, register = 3 },
			{ param_name = "specular_flags", shader_index = 1, register = 7 }
		}
	}
}

ground_entity, ground_renderer = commons.create_rendered_meshe(rg, ground_entity_config, 'water.ac', 0, FALSE)
eg:add_child('root','ground_entity',ground_entity)

ground_renderer:set_shaderrealvector( 'texture_pass', 'specular_flags', 0.0, 1050.0, 0.0, 0.0 )

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
			{ path='clothbox.jpg', stage=0}
		},
		vertex_textures =
		{
		},
		shaders_params =
		{
		    { param_name = "lights_enabled_v", shader_index = 0, register = 24 },
			{ param_name = "light0_dir_v", shader_index = 0, register = 25 },
			{ param_name = "ambient_color", shader_index = 1, register = 0 },
			{ param_name = "lights_enabled", shader_index = 1, register = 1 },
			{ param_name = "light0_color", shader_index = 1, register = 2 },
			{ param_name = "light0_dir", shader_index = 1, register = 3 },
			{ param_name = "specular_flags", shader_index = 1, register = 7 }
		}
	}
}

clothbox_entity,clothbox_renderer = commons.create_rendered_meshe(rg, clothbox_entity_config, 'mythcloth.ac', 0, FALSE)
eg:add_child('root','clothbox_entity',clothbox_entity)

clothbox_renderer:set_shaderrealvector( 'texture_pass', 'specular_flags', 1.0, 35.0, 0.0, 0.0 )

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
			{ path='marbre.jpg', stage=0}
		},
		vertex_textures =
		{
		},
		shaders_params =
		{
		    { param_name = "lights_enabled_v", shader_index = 0, register = 24 },
			{ param_name = "light0_dir_v", shader_index = 0, register = 25 },
			{ param_name = "ambient_color", shader_index = 1, register = 0 },
			{ param_name = "lights_enabled", shader_index = 1, register = 1 },
			{ param_name = "light0_color", shader_index = 1, register = 2 },
			{ param_name = "light0_dir", shader_index = 1, register = 3 },
			{ param_name = "specular_flags", shader_index = 1, register = 7 }
		}
	}
}
sphere_entity,sphere_renderer = commons.create_rendered_meshe(rg, sphere_entity_config, 'sphere.ac', 0, TRUE)
eg:add_child('root','sphere_entity',sphere_entity)

sphere_renderer:set_shaderrealvector( 'texture_pass', 'specular_flags', 1.0, 100.0, 0.0, 0.0 )

renderers[nb_renderers] = sphere_renderer
nb_renderers = nb_renderers + 1

sphere_pos_mat = Matrix()
sphere_pos_mat:translation( -5.0, 2.0, -20.0 )

sphere_transform = RawTransform()
sphere_transform:configure(sphere_entity)
sphere_transform:add_matrix( "pos", sphere_pos_mat )


update_lights( 'texture_pass', lights, renderers )


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




