
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




ground_entity, ground_renderer = commons.create_unlit_meshe( rg, 'texture_pass', 'water.ac',0, 'bloc1.jpg')
eg:add_child('root','ground_entity',ground_entity)


clothbox_entity,clothbox_renderer = commons.create_lit_meshe( rg, 'texture_pass', 'mythcloth.ac',0, 'clothbox.jpg')
eg:add_child('root','clothbox_entity',clothbox_entity)

cube_roty_mat = Matrix()

cube_rotx_mat = Matrix()

cube_pos_mat = Matrix()
cube_pos_mat:translation( 0.0, 2.0, -15.0 )



clothbox_transform = RawTransform()
clothbox_transform:configure(clothbox_entity)
clothbox_transform:add_matrix( "roty", cube_roty_mat )
clothbox_transform:add_matrix( "rotx", cube_rotx_mat )
clothbox_transform:add_matrix( "pos", cube_pos_mat )

sphere_entity,sphere_renderer = commons.create_lit_meshe( rg, 'texture_pass', 'planet.ac',0, 'marbre.jpg')
eg:add_child('root','sphere_entity',sphere_entity)


sphere_pos_mat = Matrix()
sphere_pos_mat:translation( -5.0, 2.0, -20.0 )

sphere_transform = RawTransform()
sphere_transform:configure(sphere_entity)
sphere_transform:add_matrix( "pos", sphere_pos_mat )





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




