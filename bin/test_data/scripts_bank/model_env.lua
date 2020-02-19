
model.env = {}
model.env.light = {}
model.env.ambientlight = {}
model.env.fog = {}


model.env.update_entities_shaders = function()

  for k, v in pairs(model.entities) do
     
	-- extract shader update function provided by model
    local update_entity_shaders_func = v.update_from_scene_env_function

	-- and call it with updated environnment
	update_entity_shaders_func( 'texture_pass', environment, k )
  end
end

model.env.setgravity = function( p_state )
 
  if p_state == TRUE then
    environment.gravity_state = GRAVITY_ENABLED
  else
    environment.gravity_state = GRAVITY_DISABLED
  end

  root_entity:update_gravitystate(environment.gravity_state)
end

model.env.setgravitydir = function( p_x, p_y, p_z )

  environment.gravity.x = p_x
  environment.gravity.y = p_y
  environment.gravity.z = p_z

  root_entity:update_gravitydirection( environment.gravity.x, environment.gravity.y, environment.gravity.z )
end

model.env.setbkcolor = function( p_r, p_g, p_b )
   rg:set_pass_targetclearcolor('texture_pass', p_r * 255, p_g * 255, p_b * 255)
end

model.env.fog.setdensity = function(p_density)
  environment.fog_intensity = p_density
  model.env.update_entities_shaders()
end

model.env.fog.setcolor = function(p_r, p_g, p_b)
  environment.fog_color.r = p_r
  environment.fog_color.g = p_g
  environment.fog_color.b = p_b
  model.env.update_entities_shaders()
end

model.env.light.setstate = function( p_state )
  if p_state == TRUE then
    environment.lights_enabled.x = 1.0
  else
    environment.lights_enabled.x = 0.0
  end
  model.env.update_entities_shaders()
end

model.env.light.setcolor = function( p_r, p_g, p_b )
  environment.light0.color.r = p_r
  environment.light0.color.g = p_g
  environment.light0.color.b = p_b

  model.env.update_entities_shaders()
end

model.env.light.setdir = function( p_x, p_y, p_z )
  environment.light0.direction.x = p_x
  environment.light0.direction.y = p_y
  environment.light0.direction.z = p_z

  model.env.update_entities_shaders()
end

model.env.ambientlight.setcolor = function( p_r, p_g, p_b )
  environment.ambient_light.r = p_r
  environment.ambient_light.g = p_g
  environment.ambient_light.b = p_b

  model.env.update_entities_shaders()
end

