

model = {}

model.entities = {}

model.camera = {}

model.camera.speed = 50.0

model.target = ""

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

	gravity = { x = 0.0, y = -9.81, z = 0.0},
	gravity_state = GRAVITY_DISABLED
}

include("model_dump.lua")
include("model_view.lua")
include("model_transformations.lua")
include("model_anims.lua")
include("model_env.lua")

model.printscenelist = function()

  for k, v in pairs(model.entities) do
    g:print(k..' ['..v.model_classname..']')
  end
end
