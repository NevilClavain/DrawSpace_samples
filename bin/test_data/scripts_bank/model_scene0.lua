

	g:do_file('landscape_model.lua')
	g:do_file('raptor_model.lua')
	g:do_file('trex_model.lua')

	landscape.view.load('landscape')

	raptor.view.load('r')
	trex.view.load('t')

    model.move.setpos('r', -45, 0, 0)
	model.move.setpos('landscape', 0, 4.0, 0)
	