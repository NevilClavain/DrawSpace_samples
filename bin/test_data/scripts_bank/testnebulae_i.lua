

renderers =
{
}
nb_renderers = 0;


ctrl_key = FALSE

renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

g:print('rendering infos : '..renderer_descr..', '..renderer_width..'x'..renderer_height..' fullscreen='..renderer_fullscreen..' viewport='..viewport_width..'x'..viewport_height)

sb_mod = Module("skyboxmod", "skybox")
sb_mod:load()
g:print(sb_mod:get_descr().. ' loaded')

mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

vol_mod = Module("volumetricsmod", "volums")
vol_mod:load()
g:print(vol_mod:get_descr().. ' loaded')




commons.init_final_pass(rg, 'final_pass')

rg:create_child('final_pass', 'texture_pass', 0)



text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")


camera_entity, fps_transfo=commons.create_fps_camera(0.0, 3.0, 10.0, viewport_width, viewport_height, mvt_mod)
eg:add_child('root','camera_entity',camera_entity)





commons.update_lights( 'texture_pass', lights, renderers )


commons.setup_lit_flags( 'texture_pass', renderers, REFLECTIONS_OFF, reflectorPos, reflectorNormale, fog_intensity, fog_color)


skybox_entity, skybox_renderer, sb_transform = commons.create_skybox_with_mirror( 'texture_pass', '', rg, sb_mod, "neb_front5.png", "neb_back6.png", "neb_left2.png", "neb_right1.png", "neb_top3.png", "neb_bottom4.png", 1000.0)
eg:add_child('root','skybox_entity',skybox_entity)


-- ///////////////////////////////

neb_entity_config = 
{ 
	texture_pass = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='nebulae.vso',mode=SHADER_COMPILED },
				{ path='nebulae.pso',mode=SHADER_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false"	},
				{ ope=RENDERSTATE_OPE_SETCULLING, value="none" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="true" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDOP, value="add" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDFUNC, value="always" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDDEST, value="one" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDSRC, value="srcalpha" }
								
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" },
				{ ope=RENDERSTATE_OPE_SETCULLING, value="cw" },
				{ ope=RENDERSTATE_OPE_ALPHABLENDENABLE, value="false" },				
			}
		},
		textures =
		{
			{path='nebulae_atlas.jpg', stage=0},
			{path='nebulae_mask.jpg', stage=1},
		},
		vertex_textures =
		{
		},

		shaders_params = 
		{
			{ param_name = "color", shader_index = 1, register = 0 }
		}
	}
}

neb_entity,neb_renderer=commons.create_rendering_from_module(rg,neb_entity_config,vol_mod,"nebulaeRender")
eg:add_child('root','nebulae_entity',neb_entity)


neb_entity:add_aspect(TRANSFORM_ASPECT)

neb_scale = Matrix();
neb_scale:scale(1000.0, 1000.0, 1000.0)

neb_roty_mat = Matrix()

neb_rotx_mat = Matrix()

neb_pos_mat = Matrix()
neb_pos_mat:translation( 0.0, 0.0, -2000.0 )


neb_transform = RawTransform()
neb_transform:configure(neb_entity)
neb_transform:add_matrix("neb_scaling",neb_scale)
neb_transform:add_matrix( "roty", neb_roty_mat )
neb_transform:add_matrix( "rotx", neb_rotx_mat )
neb_transform:add_matrix( "pos", neb_pos_mat )

neb_renderer:set_shaderrealvector('texture_pass', "color", 0.45, 1.0, 0.45, 1.0 )





n0 = function(p_a)
	neb_renderer:set_shaderrealvector( 'texture_pass', 'color', 1.0, 1.0, 1.0, p_a )
end


-- ///////////////////////////////

eg:set_camera(camera_entity)

rg:update_renderingqueues()



fps_yaw=SyncAngle()
fps_pitch=SyncAngle()


fps_yaw:init_fromtimeaspectof(root_entity, 0.0)
fps_pitch:init_fromtimeaspectof(root_entity,0.0)


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

	fps_yaw:inc(-dx / 1.0)

	fps_pitch:inc(-dy / 1.0)

	fps_infos = { fps_transfo:read() }
	fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], fps_infos[9]) 
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
end)

g:add_mouserightbuttonupcb( "onmouserbup",
function( xm, ym )
end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 
    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 1200.0) 
  --W key
  elseif key == 87 then
    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], -1200.0) 
  
   
  elseif key == 17 then
    ctrl_key = TRUE
  end

end)

g:add_keyupcb( "keyup",
function( key )  

  --Q key
  if key == 81 then

    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 0.0) 
    
  --W key
  elseif key == 87 then

    fps_infos = { fps_transfo:read() }
    fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], 0.0) 
  -- VK_F1
  elseif key == 112 then  

  
  elseif key == 17 then
    ctrl_key = FALSE

  end

end)


g:add_appruncb( "run",
function()


  time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "

  text_renderer:update(10, 30, 255, 0, 0, output_infos)


  neb_roty_mat:rotation( 0.0, 1.0, 0.0, commons.utils.deg_to_rad( cube_rot_y:get_value() ) )
  neb_transform:update_matrix("roty",neb_roty_mat)

  neb_rotx_mat:rotation( 1.0, 0.0, 0.0, commons.utils.deg_to_rad( cube_rot_x:get_value() ) )
  neb_transform:update_matrix("rotx",neb_rotx_mat)


end)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)


g:signal_renderscenebegin("eg")

