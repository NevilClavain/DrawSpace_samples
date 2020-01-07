



model.move = {}
model.pos_mat = Matrix()
model.scale_mat = Matrix()
model.rotx_mat = Matrix()
model.roty_mat = Matrix()
model.rotz_mat = Matrix()
model.rotx_deg_angle = 0.0
model.roty_deg_angle = 0.0
model.rotz_deg_angle = 0.0


MODEL_TRANSFORMATION_INPUTMODE_NONE = -1

MODEL_TRANSFORMATION_INPUTMODE_POSX = 0
MODEL_TRANSFORMATION_INPUTMODE_POSY = 1
MODEL_TRANSFORMATION_INPUTMODE_POSZ = 2

MODEL_TRANSFORMATION_INPUTMODE_ROTX = 3
MODEL_TRANSFORMATION_INPUTMODE_ROTY = 4
MODEL_TRANSFORMATION_INPUTMODE_ROTZ = 5

MODEL_TRANSFORMATION_INPUTMODE_SCALEX = 6
MODEL_TRANSFORMATION_INPUTMODE_SCALEY = 7
MODEL_TRANSFORMATION_INPUTMODE_SCALEZ = 8

MODEL_TRANSFORMATION_INPUTMODE_SCALEXYZ = 9


transformations_update = function(p_delta)

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_POSX then

	  local pos_x = model.pos_mat:get_value(3,0)
      pos_x = pos_x + p_delta
      model.pos_mat:set_value(3,0,pos_x)
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_POSY then

	  local pos_y = model.pos_mat:get_value(3,1)
      pos_y = pos_y + p_delta
      model.pos_mat:set_value(3,1,pos_y)
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_POSZ then

	  local pos_z = model.pos_mat:get_value(3,2)
      pos_z = pos_z + p_delta
      model.pos_mat:set_value(3,2,pos_z)      
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_ROTX then
	  model.rotx_deg_angle = model.rotx_deg_angle + p_delta
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_ROTY then
	  model.roty_deg_angle = model.roty_deg_angle + p_delta
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_ROTZ then
	  model.rotz_deg_angle = model.rotz_deg_angle + p_delta
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_SCALEX then

	  local scale_x = model.scale_mat:get_value(0,0)
      scale_x = scale_x + 0.01 * p_delta
	  if scale_x < 0.001 then
	     scale_x = 0.001
	  end

      model.scale_mat:set_value(0,0,scale_x)
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_SCALEY then

	  local scale_y = model.scale_mat:get_value(1,1)
      scale_y = scale_y + 0.01 * p_delta
	  if scale_y < 0.001 then
	     scale_y = 0.001
	  end

      model.scale_mat:set_value(1,1,scale_y)
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_SCALEZ then

	  local scale_z = model.scale_mat:get_value(2,2)
      scale_z = scale_z + 0.01 * p_delta
	  if scale_z < 0.001 then
	     scale_z = 0.001
	  end

      model.scale_mat:set_value(2,2,scale_z)
	end

    if model.transformation_input_mode == MODEL_TRANSFORMATION_INPUTMODE_SCALEXYZ then

	  local scale_x = model.scale_mat:get_value(0,0)
	  local scale_y = model.scale_mat:get_value(1,1)
	  local scale_z = model.scale_mat:get_value(2,2)
      scale_x = scale_x + 0.01 * p_delta
	  scale_y = scale_y + 0.01 * p_delta
	  scale_z = scale_z + 0.01 * p_delta

	  if scale_x < 0.001 then
	     scale_x = 0.001
	  end

	  if scale_y < 0.001 then
	     scale_y = 0.001
	  end

	  if scale_z < 0.001 then
	     scale_z = 0.001
	  end

      model.scale_mat:set_value(0,0,scale_x)
	  model.scale_mat:set_value(1,1,scale_y)
	  model.scale_mat:set_value(2,2,scale_z)
	end

    model.rotx_mat:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(model.rotx_deg_angle))
    model.roty_mat:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(model.roty_deg_angle))
    model.rotz_mat:rotation(0.0, 0.0, 1.0, commons.utils.deg_to_rad(model.rotz_deg_angle))

	model.transform:update_matrix("pos",model.pos_mat)
	model.transform:update_matrix("rotx",model.rotx_mat)
	model.transform:update_matrix("roty",model.roty_mat)
	model.transform:update_matrix("rotz",model.rotz_mat)
	model.transform:update_matrix("scale",model.scale_mat)

end

model.move.posx = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_POSX
end

model.move.posy = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_POSY
end

model.move.posz = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_POSZ
end

model.move.rotx = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_ROTX
end

model.move.roty = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_ROTY
end

model.move.rotz = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_ROTZ
end

model.move.scalex = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_SCALEX
end

model.move.scaley = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_SCALEY
end

model.move.scalez = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_SCALEZ
end

model.move.scalexyz = function()
  model.transformation_input_mode = MODEL_TRANSFORMATION_INPUTMODE_SCALEXYZ
end

model.move.resetpos = function()

  local pos_x = model.pos_mat:get_value(3,0)
  local pos_y = model.pos_mat:get_value(3,1)
  local pos_z = model.pos_mat:get_value(3,2)

  pos_x = 0
  pos_y = 0
  pos_z = 0

  model.pos_mat:set_value(3,0,pos_x)
  model.pos_mat:set_value(3,1,pos_y)
  model.pos_mat:set_value(3,2,pos_z)

  model.transform:update_matrix("pos",model.pos_mat)
end

model.move.resetrot = function()

  model.rotx_deg_angle = 0
  model.roty_deg_angle = 0
  model.rotz_deg_angle = 0

  model.rotx_mat:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(model.rotx_deg_angle))
  model.roty_mat:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(model.roty_deg_angle))
  model.rotz_mat:rotation(0.0, 0.0, 1.0, commons.utils.deg_to_rad(model.rotz_deg_angle))

  model.transform:update_matrix("rotx",model.rotx_mat)
  model.transform:update_matrix("roty",model.roty_mat)
  model.transform:update_matrix("rotz",model.rotz_mat)
end

model.move.resetscale = function()

  local scale_x = model.scale_mat:get_value(0,0)
  local scale_y = model.scale_mat:get_value(1,1)
  local scale_z = model.scale_mat:get_value(2,2)

  scale_x = 1
  scale_y = 1
  scale_z = 1

  model.scale_mat:set_value(0,0,scale_x)
  model.scale_mat:set_value(1,1,scale_y)
  model.scale_mat:set_value(2,2,scale_z)

  model.transform:update_matrix("scale",model.scale_mat)

end

