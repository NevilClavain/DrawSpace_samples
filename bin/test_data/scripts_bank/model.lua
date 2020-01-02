


environment = 
{
	ambient_light = {r = 0.15, g = 0.15, b = 0.15, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = -1.0, y = -0.5, z = 0.0, w = 1.0 },
	},

	fog_intensity = 0.0009,
	fog_color = 
	{
		r = 0.75,
		g = 0.75,
		b = 0.99, 
	},

	mirror = 0,
	reflector_pos = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	reflector_normale = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
}

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

rg:set_pass_targetclearcolor('texture_pass', 80, 80, 80)

text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 10, 40, 255, 0, 255, "??? fps")

root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_ENABLED, 0.0, -9.81, 0.0)

camera_entity, camera_mvt=commons.create_free_camera(0.0, 1.70, 6.0, renderer_infos[5],renderer_infos[6], mvt_mod, "camera")

eg:add_child('root','camera_entity',camera_entity)




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

    -- VK_F2
  elseif key == 113 then
    
  end
end)



g:add_appruncb( "run",
function()  

  time_infos = { root_entity:read_timemanager() }

  local timescale = commons.print_timescale(time_infos[1])

  local output_infos = "[MODEL VIEWER]    " ..renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2].." timescale = "..timescale

  text_renderer:update(10, 30, 255, 0, 0, output_infos)

  local mvt_info = { camera_mvt:read() }
  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)


end)


g:signal_renderscenebegin("eg")

model = {}

model.dump = {}
model.dump.anims = {}
model.dump.meshes = {}

model.view = {}

model.dump.load = function(entity_name, model_file_path)

  local meshe_entity=Entity()

  meshe_entity:add_aspect(RESOURCES_ASPECT)
  meshe_entity:configure_mesheresource(model_file_path)
  
  eg:add_child('root', entity_name, meshe_entity)
  return meshe_entity
end

model.dump.unload = function(meshe_entity)

  local entity_name = eg:find_entityname(meshe_entity)
  meshe_entity:release_mesheresource()
  meshe_entity:remove_aspect(RESOURCES_ASPECT)

  eg:remove(entity_name)
end

model.dump.show = function(entity)

  local list_size = entity:read_meshesfiledescriptionssize()   	
  for i = 1, list_size do
    local model_root_infos = { entity:read_meshesfiledescription(i, 'root') }
	g:print( i..' - model file = '..model_root_infos[1]..' num_meshes = '..model_root_infos[3]..' num_animations = '..model_root_infos[5] )
	
	
	g:print('Meshes : ')
    local meshes_id_list = { entity:read_meshesfiledescription(i, 'meshes_list') }
	for k, v in pairs(meshes_id_list) do
	  g:print(k.. ' - ' ..v)

      local meshe_infos = { entity:read_meshesfiledescription(i, 'meshe', k) }
      g:print('node id = '..meshe_infos[1].. ' name = ['..meshe_infos[2]..']' )
      g:print('has positions = '..meshe_infos[3]..' has faces = '..meshe_infos[4]..' has normales = '..meshe_infos[5]..' has tbn = '..meshe_infos[6] )
      g:print('num vertices = '..meshe_infos[7]..' num faces = '..meshe_infos[8]..' num uvchannels = '..meshe_infos[9]..' num bones = '..meshe_infos[10])  

	end

	g:print('Animations : ')
    local anims_id_list = { entity:read_meshesfiledescription(i, 'anims_list') }
	for k, v in pairs(anims_id_list) do
	  g:print(k.. ' - ' ..v)

      local anim_infos = { entity:read_meshesfiledescription(i, 'anim', k) }
      g:print('name = '..anim_infos[1]..' ticks/sec = '..anim_infos[2]..' duration = '..anim_infos[3]..'s number of channels = '..anim_infos[4])
	end
  end

end


model.view.load = function(p_modelviewload_function, p_update_from_scene_env_function)

  p_modelviewload_function(rg, eg, 'texture_pass')
  p_update_from_scene_env_function( 'texture_pass', environment)  
  rg:update_renderingqueues()
end

model.view.unload = function(p_modelunload_function)

  p_modelunload_function(rg, eg)
  rg:update_renderingqueues()

end

g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")

