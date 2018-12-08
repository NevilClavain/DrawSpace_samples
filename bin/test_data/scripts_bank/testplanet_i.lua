

renderers =
{
}
nb_renderers = 0;



fog_intensity = 0.0

fog_color = 
{
	r = 0.75,
	g = 0.75,
	b = 0.99, 
}

lights = 
{
	ambient_light = {r = 0.05, g = 0.05, b = 0.05, a = 0.0 },
	lights_enabled = {x = 1.0, y = 0.0, z = 0.0, w = 0.0 },
	light0 = 
	{
		color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
		direction = { x = 0.0, y = 1.0, z = 0.0, w = 1.0 },
	}
}



mouse_right = FALSE

renderer_descr, renderer_width, renderer_height, renderer_fullscreen, viewport_width, viewport_height = renderer:descr()

g:print('rendering infos : '..renderer_descr..', '..renderer_width..'x'..renderer_height..' fullscreen='..renderer_fullscreen..' viewport='..viewport_width..'x'..viewport_height)

sb_mod = Module("skyboxmod", "skybox")
sb_mod:load()
g:print(sb_mod:get_descr().. ' loaded')

mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

pl_mod = Module("planetsmod", "planets")
pl_mod:load()
g:print(pl_mod:get_descr().. ' loaded')


commons.init_final_pass(rg, 'final_pass')

rg:create_child('final_pass', 'texture_pass', 0)



text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 320, 30, 255, 0, 255, "??? fps")


root_entity:add_aspect(PHYSICS_ASPECT)
root_entity:configure_world(GRAVITY_DISABLED)


camera_entity, camera_mvt=commons.create_free_camera(0.0, 0.0, 0.0, viewport_width,viewport_height, mvt_mod)
eg:add_child('root','camera_entity',camera_entity)





commons.update_lights( 'texture_pass', lights, renderers )


commons.setup_lit_flags( 'texture_pass', renderers, REFLECTIONS_OFF, reflectorPos, reflectorNormale, fog_intensity, fog_color)


skybox_config =
{
	texture_pass =	
	{
		fx =
		{
			shaders = 
			{
				{ path='texture.vso',mode=SHADER_COMPILED },
				{ path='texture.pso',mode=SHADER_COMPILED }
			},
			rs_in = 
			{
			},
			rs_out =
			{
			}		
		},
		textures =
		{
			[1] = 
			{
				{path='neb_front5.png', stage=0}
			},
			[2] = 
			{
				{path='neb_back6.png', stage=0}
			},
			[3] = 
			{
				{path='neb_left2.png', stage=0}
			},
			[4] = 
			{
				{path='neb_right1.png', stage=0}
			},
			[5] = 
			{
				{path='neb_top3.png', stage=0}
			},
			[6] = 
			{
				{path='neb_bottom4.png', stage=0}
			}
		},
		vertex_textures =
		{
		},
		shaders_params = 
		{
		},
		rendering_order = 10000
	}
}

skybox_entity,skybox_renderer=commons.create_rendering_from_module(skybox_config,sb_mod,"skyboxRender")
skybox_renderer:register_to_rendering(rg)
eg:add_child('root','skybox_entity',skybox_entity)


skybox_entity:add_aspect(TRANSFORM_ASPECT)
sb_scale = Matrix();
sb_scale:scale(1000.0, 1000.0, 1000.0)

sb_transform = RawTransform()
sb_transform:configure(skybox_entity)
sb_transform:add_matrix("sb_scaling",sb_scale)

-- ///////////////////////////////

sphere_entity_config = 
{ 
	texture_pass = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='lit.vso',mode=SHADER_COMPILED },
				{ path='lit.pso',mode=SHADER_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
			}
		},
		textures =
		{
			[1] = 
			{
				{ path='marbre.jpg', stage=0},
				{ path='bump.bmp', stage=1}
			}
		},
		vertex_textures =
		{
		},
		rendering_order = 10000,
		shaders_params = commons.setup_lit_shader_params()
	}
}
sphere_entity,sphere_renderer = commons.create_rendered_meshe(sphere_entity_config, 'sphere.ac', 0)
sphere_renderer:register_to_rendering(rg)
eg:add_child('root','sphere_entity',sphere_entity)


sphere_entity:add_aspect(BODY_ASPECT)
sphere_body=Body()

sphere_body:attach_toentity(sphere_entity)

sphere_body:configure_shape(SHAPE_SPHERE, 1.0)


sphere_pos_mat = Matrix()

sphere_pos_mat:translation( 0.0, 0.0, -60.0 )

sphere_body:configure_attitude(sphere_pos_mat)

sphere_body:configure_mass(80.0)

sphere_body:configure_mode(BODY_MODE)


sphere_material =
{
	specular_power = 200.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 1.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 512, bias = 0.45 }
}

commons.apply_material( sphere_material, sphere_renderer, 'texture_pass')


renderers[nb_renderers] = sphere_renderer
nb_renderers = nb_renderers + 1

-- ///////////////////////////////

ship_entity_config = 
{ 
	texture_pass = 
	{
		fx = 
		{
			shaders = 
			{
				{ path='lit.vso',mode=SHADER_COMPILED },
				{ path='lit.pso',mode=SHADER_COMPILED }
			},
			rs_in = 
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="true"	}		
			},
			rs_out =
			{
				{ ope=RENDERSTATE_OPE_ENABLEZBUFFER, value="false" }
			}
		},
		textures =
		{
			[1] = 
			{
				--{ path='SurveyShip_color.jpg', stage=0},
				--{ path='SurveyShip_color.jpg', stage=1}
				{ path='bellerophon.jpg', stage=0},
				{ path='bellerophon.jpg', stage=1}
			}
		},
		vertex_textures =
		{
		},
		rendering_order = 10000,
		shaders_params = commons.setup_lit_shader_params()
	}
}

--ship_entity,ship_renderer = commons.create_rendered_meshe(ship_entity_config, 'survey.ac', 0)
ship_entity,ship_renderer = commons.create_rendered_meshe(ship_entity_config, 'bellerophon.ac', 0)
ship_renderer:register_to_rendering(rg)
eg:add_child('root','ship_entity',ship_entity)


ship_entity:add_aspect(BODY_ASPECT)
ship_body=Body()

ship_body:attach_toentity(ship_entity)

ship_body:configure_shape(SHAPE_BOX, 110.0, 80.0, 170.0)


ship_pos_mat = Matrix()

ship_pos_mat:translation( -300.0, 0.0, -400.0 )

ship_body:configure_attitude(ship_pos_mat)

ship_body:configure_mass(50.0)

ship_body:configure_mode(BODY_MODE)


ship_material =
{
	specular_power = 200.0,
	color_source = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 },
	simple_color = { r = 1.0, g = 0.0, b = 0.0, a = 0.0 },
	light_absorption = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	self_emissive = { r = 0.0, g = 0.0, b = 0.0, a = 0.0 },
	bump_mapping = { texture_size = 1024, bias = 0.45 }
}

commons.apply_material( ship_material, ship_renderer, 'texture_pass')


renderers[nb_renderers] = ship_renderer
nb_renderers = nb_renderers + 1


-- ///////////////////////////////

planet_entity_config = 
{ 

}

planet_entity,planet_renderer=commons.create_rendering_from_module(planet_entity_config,pl_mod,"planetsRender")

planet_renderer:register_to_rendering(rg)

eg:add_child('root','planet_entity',planet_entity)

planet_entity:add_aspect(TRANSFORM_ASPECT)

-- ///////////////////////////////


commons.update_lights( 'texture_pass', lights, renderers )
commons.setup_lit_flags_simple( 'texture_pass', renderers, fog_intensity, fog_color)







eg:set_camera(camera_entity)

rg:update_renderingqueues()



g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

	local mvt_info = { camera_mvt:read() }

	if mouse_right == FALSE then
	  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
	else
	  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
	end

end)

g:add_mouseleftbuttondowncb( "onmouselbdown",
function( xm, ym )
end)

g:add_mouseleftbuttonupcb( "onmouselbup",
function( xm, ym )
end)


g:add_mouserightbuttondowncb( "onmouserbdown",
function( xm, ym )
	mouse_right = TRUE
end)

g:add_mouserightbuttonupcb( "onmouserbup",
function( xm, ym )
	mouse_right = FALSE
end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 
    
    local mvt_info = { camera_mvt:read() }
	camera_mvt:update(120.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  --W key
  elseif key == 87 then
    
    local mvt_info = { camera_mvt:read() }
	camera_mvt:update(-120.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
  
  end

end)

g:add_keyupcb( "keyup",
function( key )  

  --Q key
  if key == 81 then
    
    local mvt_info = { camera_mvt:read() }

	camera_mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)


  --W key
  elseif key == 87 then

    local mvt_info = { camera_mvt:read() }

	camera_mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)


  -- VK_F1
  elseif key == 112 then  

 

  end

end)


g:add_appruncb( "run",
function()


  time_infos = { root_entity:read_timemanager() }
  output_infos = renderer:descr() .." "..time_infos[3].. " fps "

  text_renderer:update(10, 30, 255, 0, 0, output_infos)

  local mvt_info = { camera_mvt:read() }
  camera_mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)


end)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)


g:signal_renderscenebegin("eg")



