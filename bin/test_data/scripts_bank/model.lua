


model = {}

model.dump = {}
model.dump.anims = {}
model.dump.meshes = {}

model.view = {}

model.entities = {}

model.env = {}
model.env.light = {}
model.env.ambientlight = {}
model.env.fog = {}
model.camera = {}

model.camera.speed = 50.0

model.target = ""

include("model_transformations.lua")
include("model_anims.lua")




environment = 
{
	ambient_light = {r = 0.35, g = 0.35, b = 0.35, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = -0.1, y = -1.0, z = 0.0, w = 1.0 },
	},


	fog_intensity = 0.00022,
	fog_color = 
	{
		r = 0.55,
		g = 0.55,
		b = 0.99, 
	},

	mirror = 0,
	reflector_pos = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	reflector_normale = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },

	gravity = { x = 0.0, y = -9.81, z = 0.0}
}

ctrl_key = FALSE
last_key = 0

mouse_right = FALSE
mouse_left = FALSE


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

move_renderer=TextRendering()
move_renderer:configure(root_entity, "move", 10, 60, 255, 0, 255, "...")

animsinfos_renderer=TextRendering()
animsinfos_renderer:configure(root_entity, "anims", 10, 80, 255, 0, 255, "...")

dbg_renderer=TextRendering()
dbg_renderer:configure(root_entity, "debug", 10, 100, 255, 0, 255, "...")
dbg_string = "debugme"

root_entity:add_aspect(PHYSICS_ASPECT)

model.camera.entity, model.camera.mvt=commons.create_free_camera(0.0, 5.0, 0, renderer_infos[5],renderer_infos[6], mvt_mod, "model.camera")

eg:add_child('root','model.camera.entity',model.camera.entity)




-- ///////////////////////////////

eg:set_camera(model.camera.entity)

rg:update_renderingqueues()


g:add_mousemovecb( "onmousemove",function( xm, ym, dx, dy )  

  local mvt_info = { model.camera.mvt:read() }


  if mouse_left == TRUE then

    transformations_update( dx )

  else

    if mouse_right == FALSE then
	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)

    else
	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)

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

g:add_mouseleftbuttondowncb( "onmouseleftbuttondown", 
function( xm, ym )
  mouse_left = TRUE
end)

g:add_mouseleftbuttonupcb( "onmouseleftbuttonup", 
function( xm, ym )
  mouse_left = FALSE
end)

g:add_keydowncb( "keydown",
function( key )



  --Q key
  if key == 81 then 
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(model.camera.speed,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      
  --W key
  elseif key == 87 then

    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(-model.camera.speed,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  elseif key == 17 then
    ctrl_key = TRUE


  end

end)

g:add_keyupcb( "keyup",
function( key )  

  last_key = key

  --Q key
  if key == 81 then
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

    
  --W key
  elseif key == 87 then
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
 
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

  local target_infos = ""
  local target_anims_infos = ""

  if model.target ~= nil then

    if model.target ~= "" then

      target_infos = "selection = ["..model.target.."]"

	  if model.entities[model.target] ~= nil then

        if model.entities[model.target].entity:has_aspect(BODY_ASPECT) == TRUE then

	      target_infos = target_infos.. " (is body)"

	    else

          local transform_entry = model.transformations[model.target]

          local pos_x = transform_entry['pos_mat']:get_value(3,0)
	      local pos_y = transform_entry['pos_mat']:get_value(3,1)
	      local pos_z = transform_entry['pos_mat']:get_value(3,2)

	      local scale_x = transform_entry['scale_mat']:get_value(0,0)
	      local scale_y = transform_entry['scale_mat']:get_value(1,1)
	      local scale_z = transform_entry['scale_mat']:get_value(2,2)

	      local rot_x = transform_entry['rotx_deg_angle']
	      local rot_y = transform_entry['roty_deg_angle']
	      local rot_z = transform_entry['rotz_deg_angle']

	      target_infos = target_infos.. " rot = "..rot_x.. " "..rot_y.." "..rot_z.." pos = "..pos_x.." "..pos_y.." "..pos_z.." scale = "..scale_x.. " "..scale_y.." "..scale_z

	    end
		
		if model.entities[model.target].entity:has_aspect(ANIMATION_ASPECT) == TRUE then
		   target_anims_infos = "ANIMATED : "

           local current_animation_name
           local current_animation_ticks_per_seconds
		   local current_animation_ticks_duration
           local current_animation_seconds_duration
           local current_animation_ticks_progress 
           local current_animation_seconds_progress 
		   
		   
		   current_animation_name, 
           current_animation_ticks_per_seconds, 
           current_animation_ticks_duration,
           current_animation_seconds_duration, 
           current_animation_ticks_progress, 
           current_animation_seconds_progress = model.entities[model.target].entity:read_currentanimationinfos()

		   target_anims_infos = target_anims_infos.."anim="..current_animation_name
												--[[
													.." ticks/s = "..current_animation_ticks_per_seconds..
													" "..current_animation_ticks_progress.."/"..current_animation_ticks_duration.." ticks "..
													current_animation_seconds_progress.."/"..current_animation_seconds_duration.. " s"
													]]
		end
		
      else
	    target_infos = target_infos.. " UNKNOWN"
	  end
    end
  else
    target_infos = target_infos.. " UNKNOWN"
  end

  move_renderer:update(15, 70, 255, 255, 255, target_infos)
  animsinfos_renderer:update(15, 90, 255, 255, 255, target_anims_infos)

  dbg_renderer:update(15, 120, 0, 255, 0, dbg_string)


  local mvt_info = { model.camera.mvt:read() }
  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

end)



g:add_animationeventcb( "onanimationevent",
function( id, event, animation_name )
  if event == ANIMATION_END then
  
	 --dbg_string = animation_name..'-> '..id..' rand anim mode '..model.entities[id]['rand_anim_mode']..' current_animation_loop '..model.entities[id]['current_animation_loop'].. ' pool size = '.. model.entities[id]['entity']:read_animationpoolsize()

	 if animation_name ~= "transition_animation" then

       if model.entities[id]['rand_anim_mode'] == TRUE then

	     local selected_index = model.anims.compute_random_anim_index(model.entities[id]['do_something_distribution'], model.entities[id]['anim_action_distribution'], animations_rand_engine, model.entities[id]['rand_anims'], model.entities[id]['main_idle_anim'])
         model.entities[id]['current_animation_loop'] = selected_index
	     model.anims.pushanim(id, model.entities[id]['current_animation_loop'])

	   else

	     if model.entities[id]['current_animation_loop'] ~= -1 then	   
	       model.anims.pushanim(id, model.entities[id]['current_animation_loop'])
	     end

	   end
     end
  end
end)


g:signal_renderscenebegin("eg")


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
      local anim_infos = { entity:read_meshesfiledescription(i, 'anim', k) }
      g:print(k.. ' - ' ..v..' name = '..anim_infos[1]..' ticks/sec = '..anim_infos[2]..' duration = '..anim_infos[3]..'s number of channels = '..anim_infos[4])
	end
  end

end


model.view.load = function(p_modelviewload_function, p_update_from_scene_env_function, p_initial_scale, p_entity_id)

  local entity = p_modelviewload_function(rg, eg, 'texture_pass', p_entity_id, p_bodyinitialpos)

  p_update_from_scene_env_function( 'texture_pass', environment, p_entity_id)


  local rotx_deg_angle = 0.0
  local roty_deg_angle = 0.0
  local rotz_deg_angle = 0.0

  local pos_mat = Matrix()
  pos_mat:translation( 0.0, 0.0, 0.0 )

  local scale_mat = Matrix()

  if p_initial_scale ~= nil then
	scale_mat:scale( p_initial_scale['x'], p_initial_scale['y'], p_initial_scale['z'] )
  else
  	scale_mat:scale( 1.0, 1.0, 1.0 )
  end

  local rotx_mat = Matrix()
  rotx_mat:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(rotx_deg_angle))

  local roty_mat = Matrix()
  roty_mat:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(roty_deg_angle))

  local rotz_mat = Matrix()
  rotz_mat:rotation(0.0, 0.0, 1.0, commons.utils.deg_to_rad(rotz_deg_angle))

  local transform = RawTransform()

  transform:configure(entity)
  
  transform:add_matrix( "scale", scale_mat )
  transform:add_matrix( "roty", roty_mat )
  transform:add_matrix( "rotx", rotx_mat )
  transform:add_matrix( "rotz", rotz_mat )
  transform:add_matrix( "pos", pos_mat )
      
  local transform_entry = 
  { 
  	['pos_mat'] = pos_mat,
  	['scale_mat'] = scale_mat,
	['rotx_mat'] = rotx_mat,
	['roty_mat'] = roty_mat,
	['rotz_mat'] = rotz_mat,
	['rotx_deg_angle'] = rotx_deg_angle,
	['roty_deg_angle'] = roty_deg_angle,
  	['rotz_deg_angle'] = rotz_deg_angle,
	['transformation_input_mode'] = MODEL_TRANSFORMATION_INPUTMODE_NONE,
	['transform'] = transform 	
  }
  
  model.transformations[p_entity_id] = transform_entry

  local entity_properties_entry =
  {
    ['entity'] = entity,
	['current_animation_loop'] = -1,
	['rand_anim_mode'] = FALSE,
	['rand_anims'] = {},
	['main_idle_anim'] = 0,
	['update_from_scene_env_function'] = p_update_from_scene_env_function
  }

  model.entities[p_entity_id] = entity_properties_entry

  rg:update_renderingqueues()
end

model.view.loadbody = function(p_modelviewload_function, p_update_from_scene_env_function, p_initial_scale, p_entity_id, p_bodyinitialpos)

  local entity = p_modelviewload_function(rg, eg, 'texture_pass', p_entity_id, p_bodyinitialpos)

  p_update_from_scene_env_function( 'texture_pass', environment, p_entity_id)

  local entity_properties_entry =
  {
    ['entity'] = entity,
	['current_animation_loop'] = -1,
	['rand_anim_mode'] = FALSE,
	['rand_anims'] = {},
	['main_idle_anim'] = 0,
	['update_from_scene_env_function'] = p_update_from_scene_env_function
  }

  model.entities[p_entity_id] = entity_properties_entry

  rg:update_renderingqueues()
end


model.view.unload = function(p_modelunload_function,p_entity_id)

  if model.transformations[p_entity_id] ~= nil then

    local transform_entry = model.transformations[p_entity_id]

    local transform = transform_entry['transform']
    transform:release()

    transform_entry['pos_mat'] = nil
    transform_entry['scale_mat'] = nil
    transform_entry['rotx_mat'] = nil
    transform_entry['roty_mat'] = nil
    transform_entry['rotz_mat'] = nil

    transform_entry['transform'] = nil

    model.transformations[p_entity_id] = nil
  end

  p_modelunload_function(rg, eg, p_entity_id)
  rg:update_renderingqueues()

  local entity_properties_entry = model.entities[p_entity_id]
  entity_properties_entry['entity'] = nil

  model.entities[p_entity_id] = nil

end

model.env.setgravity = function( p_state )
  if p_state == TRUE then
    root_entity:configure_world(GRAVITY_ENABLED, environment.gravity.x, environment.gravity.y, environment.gravity.z )
  else
    root_entity:configure_world(GRAVITY_DISABLED, environment.gravity.x, environment.gravity.y, environment.gravity.z )
  end
end

model.env.setbkcolor = function( p_r, p_g, p_b )
   rg:set_pass_targetclearcolor('texture_pass', p_r * 255, p_g * 255, p_b * 255)
end

local update_entities_shaders = function()

  for k, v in pairs(model.entities) do
     
	-- extract shader update function provided by model
    local update_entity_shaders_func = v.update_from_scene_env_function

	-- and call it with updated environnment
	update_entity_shaders_func( 'texture_pass', environment, k )
  end
end

model.env.fog.setdensity = function(p_density)
  environment.fog_intensity = p_density
  update_entities_shaders()
end

model.env.fog.setcolor = function(p_r, p_g, p_b)
  environment.fog_color.r = p_r
  environment.fog_color.g = p_g
  environment.fog_color.b = p_b
  update_entities_shaders()
end

model.env.light.setstate = function( p_state )
  if p_state == TRUE then
    environment.lights_enabled.x = 1.0
  else
    environment.lights_enabled.x = 0.0
  end
  update_entities_shaders()
end

model.env.light.setcolor = function( p_r, p_g, p_b )
  environment.light0.color.r = p_r
  environment.light0.color.g = p_g
  environment.light0.color.b = p_b

  update_entities_shaders()
end

model.env.light.setdir = function( p_x, p_y, p_z )
  environment.light0.direction.x = p_x
  environment.light0.direction.y = p_y
  environment.light0.direction.z = p_z

  update_entities_shaders()
end

model.env.ambientlight.setcolor = function( p_r, p_g, p_b )
  environment.ambient_light.r = p_r
  environment.ambient_light.g = p_g
  environment.ambient_light.b = p_b

  update_entities_shaders()
end


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")

if modelscenefile ~= "" then
  g:print('Loading scene file : '..modelscenefile)
  g:do_file(modelscenefile)
else
  g:print('No scene file')
end


