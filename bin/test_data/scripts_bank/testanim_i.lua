
fog_intensity = 0.0001

fog_color = 
{
	r = 0.75,
	g = 0.75,
	b = 0.99, 
}

lights = 
{
	ambient_light = {r = 0.15, g = 0.15, b = 0.15, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = 1.0, y = 0.0, z = 0.0, w = 1.0 },
	}
}

renderers =
{
}
nb_renderers = 0;


ctrl_key = FALSE
last_key = 0

mouse_right = FALSE

renderer_infos = {renderer:descr()}
g:print('Current renderer is '..renderer_infos[1]..', '..renderer_infos[2]..'x'..renderer_infos[3])


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')


commons.init_final_pass(rg, 'final_pass')
rg:create_child('final_pass', 'texture_pass', 0)

rg:set_pass_targetclearcolor('texture_pass', 192, 192, 255)


text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 10, 40, 255, 0, 255, "??? fps")

text_anim_infos=TextRendering()
text_anim_infos:configure(root_entity, "animation_info", 10, 70, 255, 0, 255, "...")


root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_ENABLED, 0.0, -9.81, 0.0)




--camera_entity, camera_mvt=commons.create_free_camera(-200.0, 125.0, 100.0, renderer_infos[5],renderer_infos[6], mvt_mod, "camera")
camera_entity, camera_mvt=commons.create_free_camera(0.0, 3.0, 20.0, renderer_infos[5],renderer_infos[6], mvt_mod, "camera")

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

ground_entity, ground_renderer = commons.create_rendered_meshe(ground_entity_config, 'land2.ac', 'wavefront obj')
ground_renderer:register_to_rendering(rg)
eg:add_child('root','ground_entity',ground_entity)

ground_entity:add_aspect(BODY_ASPECT)
ground_body=Body()

ground_body:attach_toentity(ground_entity)

ground_body:configure_shape(SHAPE_MESHE, 'land2.ac', 'wavefront obj')


ground_body:configure_mode(COLLIDER_MODE)


ground_material =
{
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 1024, bias = 0.333 }
}

commons.apply_material( ground_material, ground_renderer, 'texture_pass')

renderers[nb_renderers] = ground_renderer
nb_renderers = nb_renderers + 1




dino_entity_config = 
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

--dino_entity, dino_renderer = commons.create_rendered_meshe(dino_entity_config, 'raptor.fbx', 'raptorMesh')
dino_entity, dino_renderer = commons.create_rendered_meshe(dino_entity_config, 'Trex.fbx', 'trexMesh')

dino_renderer:set_shaderrealinvector( 'texture_pass', 'flags_v', 2, 1.0)
dino_entity:add_aspect(ANIMATION_ASPECT)


dino_entity:configure_animationbones()

dino_renderer:register_to_rendering(rg)
eg:add_child('root','dino_entity',dino_entity)

dino_material =
{
	--specular_power = 429.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	--bump_mapping = { texture_size = 1024, bias = 0.193 }
}

commons.apply_material( dino_material, dino_renderer, 'texture_pass')


renderers[nb_renderers] = dino_renderer
nb_renderers = nb_renderers + 1


dino_pos_mat = Matrix()
dino_pos_mat:translation( 0.0, 0.0, 0.0 )



dino_transform = RawTransform()
dino_transform:configure(dino_entity)
dino_transform:add_matrix( "pos", dino_pos_mat )




neck_entity_config = 
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
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true" },
				{ ope=RENDERSTATE_OPE_SETCULLING, value="none" },
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
		tb_generation_mode = TB_DISCARDED,
	},
}

neck_entity, neck_renderer = commons.create_rendered_meshe(neck_entity_config, 'head.ac', 'object')

--declare bone animation for neck
--neck_renderer:set_shaderrealinvector( 'texture_pass', 'flags_v', 2, 1.0)
--neck_entity:add_aspect(ANIMATION_ASPECT)

neck_renderer:register_to_rendering(rg)
eg:add_child('root','neck_entity',neck_entity)

neck_material =
{
	specular_power = 999.0,
	color_source = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	simple_color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 1.0 },
	--bump_mapping = { texture_size = 1024, bias = 0.193 }
}

commons.apply_material( neck_material, neck_renderer, 'texture_pass')


renderers[nb_renderers] = neck_renderer
nb_renderers = nb_renderers + 1


neck_pos_mat = Matrix()
neck_pos_mat:translation( 0.0, 2.0, 0.0 )

neck_rot1_mat = Matrix()
neck_rot2_mat = Matrix()
neck_rotlerp_mat = Matrix()

neck_rot1_quat = Quaternion()
neck_rot2_quat = Quaternion()

neck_rotlerp_quat = Quaternion()


neck_rot1_quat:rotation_axis(1.0, 0.0, 0.0, commons.utils.deg_to_rad( 127.0 ))
neck_rot1_mat:rotation_fromquaternion(neck_rot1_quat)

neck_rot2_quat:rotation_axis(0.0, 1.0, 1.0, commons.utils.deg_to_rad( 102.0 ))
neck_rot2_mat:rotation_fromquaternion(neck_rot2_quat)

neck_rotlerp_quat:store_lerp(neck_rot1_quat,neck_rot2_quat, 0.0)

neck_rotlerp_mat:rotation_fromquaternion(neck_rotlerp_quat)

neck_transform = RawTransform()
neck_transform:configure(neck_entity)
--neck_transform:add_matrix( "rot1", neck_rot1_mat )
--neck_transform:add_matrix( "rot2", neck_rot2_mat )
neck_transform:add_matrix( "rotlerp", neck_rotlerp_mat	 )
neck_transform:add_matrix( "pos", neck_pos_mat )



commons.update_lights( 'texture_pass', lights, renderers )

commons.setup_lit_flags_simple( 'texture_pass', renderers, fog_intensity, fog_color)



-- ///////////////////////////////

eg:set_camera(camera_entity)

rg:update_renderingqueues()


cube_rot_y=SyncAngle()
cube_rot_x=SyncAngle()




cube_rot_x:init_fromtimeaspectof(root_entity,0.0)
cube_rot_y:init_fromtimeaspectof(root_entity,0.0)





g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if ctrl_key == TRUE then

    
    cube_rot_y:inc(20.0 * dx)
	cube_rot_x:inc(20.0 * dy)

  else
      local mvt_info = { camera_mvt:read() }

	if mouse_right == FALSE then
	  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
	else
	  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
	end
  end

end)


g:add_mouserightbuttondowncb( "onmouserightbuttondown", 
function( xm, ym )
  mouse_right = TRUE
end)


g:add_mouserightbuttonupcb( "onmouserightbuttonup", 
function( xm, ym )
  mouse_right = FALSE
end)

g:add_keydowncb( "keydown",
function( key )



  --Q key
  if key == 81 then 
    local mvt_info = { camera_mvt:read() }

	camera_mvt:update(60.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      
  --W key
  elseif key == 87 then

    local mvt_info = { camera_mvt:read() }

	camera_mvt:update(-60.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  elseif key == 17 then
    ctrl_key = TRUE


  end

end)

g:add_keyupcb( "keyup",
function( key )  

  last_key = key

  --Q key
  if key == 81 then
    local mvt_info = { camera_mvt:read() }

	camera_mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    
  --W key
  elseif key == 87 then
    local mvt_info = { camera_mvt:read() }

	camera_mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
 
  elseif key == 17 then
    ctrl_key = FALSE

    -- VK_F1
  elseif key == 112 then

      --local jaw_mat = Matrix();

	  --jaw_mat:rotation(0.0, 0.0, 1.0, -0.75)
	  --dino_entity:update_bonelocaltransform("skeleton:jaw_$AssimpFbx$_Rotation", jaw_mat)
    
    -- VK_F2
  elseif key == 113 then
    
  end
end)


g:add_appruncb( "run",
function()  

  time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2].. " key = "..last_key

  text_renderer:update(10, 30, 255, 0, 0, output_infos)


  --retrieve current anim infos for dino model

  current_animation_name, 
  current_animation_ticks_per_seconds, 
  current_animation_ticks_duration,
  current_animation_seconds_duration, 
  current_animation_ticks_progress, 
  current_animation_seconds_progress = dino_entity:read_currentanimationinfos()
  
  
  text_anim_infos:update(10, 70, 255, 0, 0, "anim="..current_animation_name.." ticks/s = "..current_animation_ticks_per_seconds.." "..current_animation_ticks_progress.."/"..current_animation_ticks_duration.." ticks "..
													current_animation_seconds_progress.."/"..current_animation_seconds_duration.. " s" )

  local mvt_info = { camera_mvt:read() }
  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

end)

g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")



animations_list = function()

  animations_names = {dino_entity:read_animationsnames()}

  for k, v in pairs(animations_names) do
	g:print('animation '..k..' ->'..v)
  end	
end


ra = function(i)

  animations_names = {dino_entity:read_animationsnames()}

  dino_entity:set_currentanimation(animations_names[i])

end
