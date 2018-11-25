

renderers =
{
}
nb_renderers = 0;


mouse_right = FALSE

renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

g:print('rendering infos : '..renderer_descr..', '..renderer_width..'x'..renderer_height..' fullscreen='..renderer_fullscreen..' viewport='..viewport_width..'x'..viewport_height)

sb_mod = Module("skyboxmod", "skybox")
sb_mod:load()
g:print(sb_mod:get_descr().. ' loaded')

mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')



commons.init_final_pass(rg, 'final_pass')

rg:create_child('final_pass', 'texture_pass', 0)



text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")


camera_entity, camera_mvt=commons.create_free_camera(0.0, 5.5, 28.0, viewport_width,viewport_height, mvt_mod)
eg:add_child('root','camera_entity',camera_entity)





commons.update_lights( 'texture_pass', lights, renderers )


commons.setup_lit_flags( 'texture_pass', renderers, REFLECTIONS_OFF, reflectorPos, reflectorNormale, fog_intensity, fog_color)


skybox_config =
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
		rendering_order = 10000
	}
}

skybox_entity,skybox_renderer=commons.create_rendering_from_module(skybox_config,sb_mod,"skyboxRender")
skybox_renderer:register_to_rendering(rg)
eg:add_child('root','skybox_entity',skybox_entity)


skybox_entity:add_aspect(TRANSFORM_ASPECT)
sb_scale = Matrix();
sb_scale:scale(1000.0, 1000.0, 1000.0)

sb_transform = RawTransform()
sb_transform:configure(skybox_entity)
sb_transform:add_matrix("sb_scaling",sb_scale)

-- ///////////////////////////////



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


