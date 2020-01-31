

	g:do_file('landscape_model.lua')
	g:do_file('raptor_model.lua')
	g:do_file('trex_model.lua')
	g:do_file('metalcube_model.lua')

	landscape.view.load('landscape')

	raptor.view.load('r')
	trex.view.load('t')
	metalcube.view.load('c')

    model.move.setpos('r', -45, 0, 0)
	model.move.setpos('landscape', 0, 4.0, 0)
	model.move.setpos('c', 0, 20.0, -60.0)
	