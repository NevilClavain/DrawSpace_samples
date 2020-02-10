

	g:do_file('continent_model.lua')

	
	g:do_file('raptor_model.lua')
	g:do_file('trex_model.lua')
	g:do_file('metalcube_model.lua')
	
	continent.view.load('continent')
	
	raptor.view.load('r')
	trex.view.load('t')
	metalcube.view.load('c', {x = -4010.0, y = 15.0, z = -2740.0} )

    model.move.setpos('r', -4020.0, 0, -2740)
	model.move.setpos('t', -4068.0, 0, -2740)

	model.env.setgravity(1)
