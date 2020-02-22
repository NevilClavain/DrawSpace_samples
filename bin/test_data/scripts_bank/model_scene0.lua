

	include('continent_model.lua')	
	include('raptor_model.lua')
	include('trex_model.lua')
	include('metalcube_model.lua')
    include('spherebump_model.lua')
	
	continent.view.load('continent')
	
	--[[
	raptor.view.load('r')
	trex.view.load('t')
	metalcube.view.load('c', {x = -4010.0, y = 15.0, z = -2740.0} )
	spherebump.view.load('s', {x = -4010.0, y = 135.0, z = -2740.0} )

	spherebump.view.load('s2', {x = -4010.0, y = 0, z = -2740.0} )

    model.move.setpos('r', -4020.0, 0, -2740)
	model.move.setpos('t', -4068.0, 0, -2740)

	model.env.setgravity(1)
	]]
	model.env.setbkcolor(0.55,0.55,0.99)

	model.camera.mvt:set_pos(-4000.0, 15.0, -2684.0)
