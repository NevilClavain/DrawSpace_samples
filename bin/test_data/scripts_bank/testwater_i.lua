




hmi_mode=FALSE

renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

g:print('rendering infos : '..renderer_descr..', '..renderer_width..'x'..renderer_height..' fullscreen='..renderer_fullscreen..' viewport='..viewport_width..'x'..viewport_height)

sb_mod = Module("skyboxmod", "skybox")
sb_mod:load()
g:print(sb_mod:get_descr().. ' loaded')


commons.init_final_pass(rg, 'final_pass')
rg:create_child('final_pass', 'texture_pass', 0)


text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")



root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_ENABLED, 0.0, -9.81, 0.0)





camera_entity, fps_transfo=commons.create_fps_camera(0.0, 3.0, 0.0, viewport_width, viewport_height)
eg:add_child('root','camera_entity',camera_entity)



ground_entity, ground_renderer = commons.rawtransform.create_unlit_meshe( rg, 'texture_pass', 'water.ac',0, '002b2su2.jpg')
eg:add_child('root','ground_entity',ground_entity)

cube_entity, cube_renderer = commons.rawtransform.create_unlit_meshe( rg, 'texture_pass', 'object.ac',0, 'mars.jpg')
eg:add_child('root','cube_entity',cube_entity)


cube_transform = RawTransform()
cube_transform:configure(cube_entity)

cube_rot_mat = Matrix()
cube_rot_mat:rotation( 0.0, 1.0, 0.0, commons.utils.deg_to_rad( 0.0 ) )
cube_transform:add_matrix("cube_rot",cube_rot_mat)

cube_pos_mat = Matrix()
cube_pos_mat:translation( 0.0, 4.0, -15.0 )
cube_transform:add_matrix("cube_pos",cube_pos_mat)









skybox_entity, skybox_renderer, sb_transform = commons.create_skybox( rg, sb_mod, "sb0.bmp", "sb2.bmp", "sb3.bmp", "sb1.bmp", "sb4.bmp", "sb4.bmp", 1000.0)
eg:add_child('root','skybox_entity',skybox_entity)

-- ///////////////////////////////

eg:set_camera(camera_entity)

rg:update_renderingqueues()



fps_yaw=SyncAngle()
fps_pitch=SyncAngle()

cube_rot=SyncAngle()


fps_yaw:init_fromtimeaspectof(root_entity, 0.0)
fps_pitch:init_fromtimeaspectof(root_entity,0.0)

cube_rot:init_fromtimeaspectof(root_entity,0.0)



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
	output_infos = renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2]

	text_renderer:update(320, 30, 255, 0, 0, output_infos)

	cube_rot:inc( 10.0 )
	
	cube_rot_mat:rotation( 0.0, 1.0, 0.0, commons.utils.deg_to_rad( cube_rot:get_value() ) )
	cube_transform:update_matrix("cube_rot",cube_rot_mat)

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

	local cube_infos = {}
	local cube_name = 'cube_entity'..y_cube

	local cube_entity
	local cube_renderer 
	
	
	cube_entity, cube_render = commons.rawtransform.create_unlit_meshe( rg, 'texture_pass', 'object.ac',0, 'bloc1.jpg')
	eg:add_child('root',cube_name,cube_entity)

	local cube_transform = RawTransform()
	cube_transform:configure(cube_entity)

	local cube_pos_mat = Matrix()
	cube_pos_mat:translation( 0.0, y_cube * 2.0, -20.0 )
	cube_transform:add_matrix("cube_pos",cube_pos_mat)

	cube_infos.entity = cube_entity
	cube_infos.renderer = cube_renderer
	cube_infos.transform = cube_transform

	cube_instances[y_cube] = cube_infos

	y_cube = y_cube + 1

end



destroy_all_cube = function()

end



gui:add_pushbuttonclickedcb( "onpushbutton",
function( layout, widget )
  
  if layout == 'main.layout' and widget == "Quit" then
    g:quit()
  
  elseif layout == 'main.layout' and widget == "Button_Create" then
    g:print('create !')
	add_cube()	
	rg:update_renderingqueues()

  elseif layout == 'main.layout' and widget == "Button_Destroy" then
    g:print('destroy !')
  end
end)






