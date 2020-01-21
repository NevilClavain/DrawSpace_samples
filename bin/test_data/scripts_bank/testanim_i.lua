
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
		direction = { x = -1.0, y = 0.0, z = 0.0, w = 1.0 },
	}
}

renderers =
{
}
nb_renderers = 0;


ctrl_key = FALSE
last_key = 0

mouse_right = FALSE


current_animation_loop = -1






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
camera_entity, camera_mvt=commons.create_free_camera(0.0, 1.70, 6.0, renderer_infos[5],renderer_infos[6], mvt_mod, "camera")

eg:add_child('root','camera_entity',camera_entity)


ground_entity_config = 
{ 
    ground_rendering = 
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

ground_material =
{
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 1024, bias = 0.333 }
}



ground_entity, ground_renderer = commons.create_rendered_meshe(ground_entity_config, 'land2.ac', 'wavefront obj', {ground_rendering='texture_pass'})
ground_renderer:register_to_rendering(rg)
eg:add_child('root','ground_entity',ground_entity)

ground_entity:add_aspect(BODY_ASPECT)
ground_body=Body()
ground_body:attach_toentity(ground_entity)
ground_body:configure_shape(SHAPE_MESHE, 'land2.ac', 'wavefront obj')
ground_body:configure_mode(COLLIDER_MODE)


commons.apply_material( ground_material, ground_renderer, 'texture_pass')

renderers[nb_renderers] = ground_renderer
nb_renderers = nb_renderers + 1




dino_entity_config = 
{ 
	main_rendering = 
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

--dino_entity, dino_renderer = commons.create_rendered_meshe(dino_entity_config, 'raptor.fbx', 'raptorMesh', {main_rendering='texture_pass'})
dino_entity, dino_renderer = commons.create_rendered_meshe(dino_entity_config, 'Trex.fbx', 'trexMesh', {main_rendering='texture_pass'})

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
dino_pos_mat:translation( -18.0, 0.0, 0.0 )

dino_scale_mat = Matrix()
dino_scale_mat:scale( 0.05, 0.05, 0.05 )



dino_transform = RawTransform()
dino_transform:configure(dino_entity)
dino_transform:add_matrix( "scale", dino_scale_mat )
dino_transform:add_matrix( "pos", dino_pos_mat )





commons.update_lights( 'texture_pass', lights, renderers )

commons.setup_lit_flags_simple( 'texture_pass', renderers, fog_intensity, fog_color)



-- ///////////////////////////////

eg:set_camera(camera_entity)

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

  local timescale = commons.print_timescale(time_infos[1])

  local output_infos = renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2].." timescale = "..timescale.." key = "..last_key

  text_renderer:update(10, 30, 255, 0, 0, output_infos)


  --retrieve current anim infos for dino model

  current_animation_name, 
  current_animation_ticks_per_seconds, 
  current_animation_ticks_duration,
  current_animation_seconds_duration, 
  current_animation_ticks_progress, 
  current_animation_seconds_progress = dino_entity:read_currentanimationinfos()
  
  
  text_anim_infos:update(10, 70, 255, 0, 0, "anim="..current_animation_name.." ticks/s = "..current_animation_ticks_per_seconds.." "..current_animation_ticks_progress.."/"..current_animation_ticks_duration.." ticks "..
													current_animation_seconds_progress.."/"..current_animation_seconds_duration.. " s")

  local mvt_info = { camera_mvt:read() }
  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

end)



-- random animations stuff

rand_engine=RandomEngine()
rand_engine:set_seedfromtime()


rand_anim_mode = FALSE;
main_idle_anim = 9
rand_anims = { 1, 2, 3, 4, 5, 8, 10, 11, 12, 13, 14 }

do_something=Distribution("uniform_int_distribution", 1, 2)
dino_action=Distribution("uniform_int_distribution", 1, 11)








g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")


rg:add_renderpasseventcb("render pass event", 
function()
  --run_anim_loop(main_idle_anim)
  run_random_anim_loop()

  --dino_entity:set_animationlastkeypose("die long")

end
)

g:add_animationeventcb( "onanimationevent",
function( event, animation_name )
  if event == 1 then

    if rand_anim_mode == TRUE then
	  current_animation_loop = compute_random_anim_index(do_something, dino_action, rand_engine)
	end

    if current_animation_loop ~= -1 then
	  run_anim(current_animation_loop)
	end
  end
end)


animations_list = function()

  animations_names = {dino_entity:read_animationsnames()}

  for k, v in pairs(animations_names) do
	g:print('animation '..k..' ->'..v)
  end	
end


run_anim = function(i)

  animations_names = {dino_entity:read_animationsnames()}
  dino_entity:push_animation(animations_names[i])

end

run_anim_loop = function(i)
  current_animation_loop = i
  run_anim(current_animation_loop)
end

run_random_anim_loop = function()
  rand_anim_mode = TRUE
  current_animation_loop = compute_random_anim_index(do_something, dino_action, rand_engine)
  run_anim(current_animation_loop)
end


stop_anim_loop = function()
  rand_anim_mode = FALSE
  current_animation_loop = -1
end


pause = function()
  root_entity:update_timescale(FREEZE)
end


time1x = function()
  root_entity:update_timescale(NORMAL_TIME)
end

timediv2 = function()
  root_entity:update_timescale(DIV2_TIME)
end

timediv4 = function()
  root_entity:update_timescale(DIV4_TIME)
end

timediv10 = function()
  root_entity:update_timescale(DIV10_TIME)
end

compute_random_anim_index = function(p_do_something_generator, p_action_generator, p_random_engine)
  
   index = p_do_something_generator:generate(p_random_engine)

   if index == 2 then      
	  return rand_anims[p_action_generator:generate(p_random_engine)]
   else

      return main_idle_anim 
   end
end