

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


sb_mod = Module("skyboxmod", "skybox")
sb_mod:load()
g:print(sb_mod:get_descr().. ' loaded')



skybox_entity, skybox_renderer, sb_transform = commons.create_skybox( 'texture_pass', rg, sb_mod, "sb0.bmp", "sb2.bmp", "sb3.bmp", "sb1.bmp", "sb4.bmp", "sb4.bmp", 1000.0)
eg:add_child('root','skybox_entity',skybox_entity)

-- ///////////////////////////////

eg:set_camera(camera_entity)

rg:update_renderingqueues()



fps_yaw=SyncAngle()
fps_pitch=SyncAngle()


fps_yaw:init_fromtimeaspectof(root_entity, 0.0)
fps_pitch:init_fromtimeaspectof(root_entity,0.0)




g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  


  
  fps_yaw:inc(-dx / 1.0)

  fps_pitch:inc(-dy / 1.0)

  fps_infos = { fps_transfo:read() }
  fps_transfo:update(fps_yaw:get_value(),fps_pitch:get_value(),fps_infos[3],fps_infos[4],fps_infos[5],fps_infos[6], fps_infos[7], fps_infos[8], fps_infos[9]) 

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

  end

end)


g:add_appruncb( "run",
function()  

  time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2]

  text_renderer:update(10, 30, 255, 0, 0, output_infos)

end)

g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)




