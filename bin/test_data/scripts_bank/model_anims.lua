
model.anims = {}

animations_rand_engine=RandomEngine()
animations_rand_engine:set_seedfromtime()


model.anims.compute_random_anim_index = function(p_do_something_generator, p_action_generator, p_random_engine, p_rand_anims, p_main_idle_anim)
  
   local index = p_do_something_generator:generate(p_random_engine)

   if index == 2 then
      local action_index = p_action_generator:generate(p_random_engine)
	  return p_rand_anims[action_index]
   else

      return p_main_idle_anim 
   end   
end

model.anims.run = function(p_entity_id, p_index)
 
  local entity_properties_entry = model.entities[p_entity_id]
  local entity = entity_properties_entry['entity']
  local animations_names = {entity:read_animationsnames()}
  entity:push_animation(animations_names[p_index])
end

model.anims.runloop = function(p_entity_id, p_index)
  model.entities[p_entity_id]['current_animation_loop'] = p_index
  model.anims.run(p_entity_id, p_index)
end

model.anims.rand = function(p_entity_id, p_main_idle_anim, p_rand_anims, p_dosomething_distrib, p_action_distrib)

  model.entities[p_entity_id]['rand_anim_mode'] = TRUE
  local selected_index = model.anims.compute_random_anim_index(p_dosomething_distrib, p_action_distrib, animations_rand_engine,p_rand_anims,p_main_idle_anim)
  
  model.entities[p_entity_id]['current_animation_loop'] = selected_index

  model.entities[p_entity_id]['rand_anims'] = p_rand_anims
  model.entities[p_entity_id]['main_idle_anim'] = p_main_idle_anim

  model.entities[p_entity_id]['do_something_distribution'] = p_dosomething_distrib
  model.entities[p_entity_id]['anim_action_distribution'] = p_action_distrib


  model.anims.run(p_entity_id, selected_index)
end

model.anims.stop = function(p_entity_id)
    model.entities[p_entity_id]['rand_anim_mode'] = FALSE
	model.entities[p_entity_id]['current_animation_loop'] = -1
end

model.anims.dump = function(p_entity_id)

  local entity_properties_entry = model.entities[p_entity_id]
  local entity = entity_properties_entry['entity']
  animations_names = {entity:read_animationsnames()}

  for k, v in pairs(animations_names) do
	g:print('animation '..k..' ->'..v)
  end	
end
