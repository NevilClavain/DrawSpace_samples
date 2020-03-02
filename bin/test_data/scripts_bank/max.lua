
include("model_main.lua")

text_x_position = 140
hmi_mode=FALSE

ctrl_key = FALSE
last_key = 0

mouse_right = FALSE
mouse_left = FALSE


renderer_infos = {renderer:descr()}
g:print('Current renderer is '..renderer_infos[1]..', '..renderer_infos[2]..'x'..renderer_infos[3])

mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

commons.init_final_pass(rg, 'final_pass')
rg:create_child('final_pass', 'texture_pass', 0)

rg:set_pass_targetclearcolor('texture_pass', 80, 80, 80)

text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 80, 40, 255, 0, 255, "??? fps")

move_renderer=TextRendering()
move_renderer:configure(root_entity, "move", 80, 60, 255, 0, 255, "...")

animsinfos_renderer=TextRendering()
animsinfos_renderer:configure(root_entity, "anims", 80, 80, 255, 0, 255, "...")

operation_renderer=TextRendering()
operation_renderer:configure(root_entity, "current_operation", 80, 100, 255, 0, 255, "")

dbg_renderer=TextRendering()
dbg_renderer:configure(root_entity, "debug", 80, 100, 255, 0, 255, "...")
dbg_string = ""

root_entity:add_aspect(PHYSICS_ASPECT)

model.camera.entity, model.camera.mvt=commons.create_free_camera(0.0, 5.0, 0, renderer_infos[5],renderer_infos[6], mvt_mod, "model.camera")

eg:add_child('root','model.camera.entity',model.camera.entity)




-- ///////////////////////////////

eg:set_camera(model.camera.entity)

rg:update_renderingqueues()


g:add_mousemovecb( "onmousemove",function( xm, ym, dx, dy )  

  if hmi_mode == TRUE then
    gui:on_mousemove( xm, ym, dx, dy )
  else

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
  end

end)


g:add_mouserightbuttondowncb( "onmouserightbuttondown", 
function( xm, ym )

  if hmi_mode == TRUE then
    gui:on_mouserightbuttondown()
  else
    mouse_right = TRUE
  end

end)

g:add_mouserightbuttonupcb( "onmouserightbuttonup", 
function( xm, ym )

  if hmi_mode == TRUE then
    gui:on_mouserightbuttonup()
  else
    mouse_right = FALSE
  end
end)

g:add_mouseleftbuttondowncb( "onmouseleftbuttondown", 
function( xm, ym )

  if hmi_mode == TRUE then
    gui:on_mouseleftbuttondown()
  else
    mouse_left = TRUE
  end
end)

g:add_mouseleftbuttonupcb( "onmouseleftbuttonup", 
function( xm, ym )

  if hmi_mode == TRUE then
    gui:on_mouseleftbuttonup()
  else
    mouse_left = FALSE
  end
end)

g:add_keydowncb( "keydown",
function( key )

  if hmi_mode == TRUE then
    gui:on_keydown( key )
  else

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
  end

end)

g:add_keyupcb( "keyup",
function( key )  

  if hmi_mode == TRUE then
    if key == 112 then -- VK_F1
      if hmi_mode==TRUE then
        hmi_mode=FALSE
        g:show_mousecursor(FALSE)
        g:set_mousecursorcircularmode(TRUE)
      end 
	else
	  gui:on_keyup( key )
	end    
  else

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

      if hmi_mode == FALSE then
        hmi_mode = TRUE
        g:show_mousecursor(TRUE)
        g:set_mousecursorcircularmode(FALSE)
	  end   
    end

  end
end)

g:add_oncharcb( "onchar",
function( char, scan )  
  gui:on_char( char )
end)

g:add_appruncb( "run",
function()  

  time_infos = { root_entity:read_timemanager() }

  local timescale = commons.print_timescale(time_infos[1])

  local output_infos = "[MODEL VIEWER]    " ..renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2].." timescale = "..timescale
  text_renderer:update(text_x_position, 30, 255, 0, 0, output_infos)

  local target_infos = ""
  local target_anims_infos = ""

  if model.target ~= nil then

    if model.target ~= "" then
	  if model.entities[model.target] ~= nil then

	    target_infos = "selection = ["..model.target.."] ("..model.entities[model.target].model_classname..")"

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

  move_renderer:update(text_x_position, 70, 255, 255, 255, target_infos)
  animsinfos_renderer:update(text_x_position, 90, 255, 255, 255, target_anims_infos)

  operation_renderer:update(text_x_position, 120, 255, 255, 255, model.current_operation)

  dbg_renderer:update(text_x_position, 150, 0, 255, 0, dbg_string)


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

root_entity:configure_world(environment.gravity_state, environment.gravity.x, environment.gravity.y, environment.gravity.z )

gui=Gui()
gui:init()
gui:set_resourcespath("./maxskin")
gui:load_scheme("AlfiskoSkin.scheme")
gui:load_layout("max.layout","maxskin/layouts/max_widgets.conf")
gui:set_layout("max.layout")
gui:show_gui(TRUE)

gui:add_pushbuttonclickedcb( "onpushbutton",
function( layout, widget )
  
  -- dbg_renderer:update(350, 20, 255, 0, 255, layout.. " "..widget)

  if widget == "Button_Quit" then
    g:quit()
  end

  if widget == "Button_ClearInfos" then
    model.text = ""
    gui:set_widgettext("max.layout", "Label_Text", model.text)
  end

  if widget == "Button_AnimsDump" then
    model.anims.dump()
  end

  if widget == "Button_AnimsRun" then
	local index, error = g:stoi(gui:get_widgettext("max.layout", "Editbox_AnimIndex"))
	if error == 1 then
	  --g:print('CONV ERROR')
	else
	  model.anims.run(index)
	end	
  end
end)

g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")

if modelscenefile ~= "" then
  g:print('Loading scene file : '..modelscenefile)
  g:do_file(modelscenefile)
else
  g:print('No scene file')
end


--define how to show dump infos

model.dump.showcontentfunc = function()
  g:breakpoint(model.text)
  gui:set_widgettext("max.layout", "Label_Text", model.text)
end
