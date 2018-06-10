
commons = {}

commons.utils = {}
commons.trash = {}

commons.utils.PI = 3.1415927

commons.utils.deg_to_rad = function(angle)
	return ( ( angle * commons.utils.PI ) / 180.0 );
end

commons.init_final_pass = function(p_rendergraph, p_passid)

	p_rendergraph:create_pass_viewportquad(p_passid)

	local finalpass_rss=RenderStatesSet()

	finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
	finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")

	local textures = TexturesSet()

	local fxparams = FxParams()
	fxparams:add_shaderfile('texture.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('texture.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(finalpass_rss)


	rendercontext = RenderContext(p_passid)
	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)

	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)
	p_rendergraph:configure_pass_viewportquad_resources(p_passid,renderconfig)

end

commons.init_final_pass_water_mask = function(p_rendergraph, p_passid)

	p_rendergraph:create_pass_viewportquad(p_passid)

	local finalpass_rss=RenderStatesSet()

	finalpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
	finalpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")

	local textures = TexturesSet()

	local fxparams = FxParams()
	fxparams:add_shaderfile('water_mask.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('water_mask.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(finalpass_rss)


	rendercontext = RenderContext(p_passid)
	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)

	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)
	p_rendergraph:configure_pass_viewportquad_resources(p_passid,renderconfig)

end

commons.create_fps_camera = function(p_x, p_y, p_z, p_viewport_width, p_viewport_height)

	local camera_entity=Entity()
	camera_entity:add_aspect(TRANSFORM_ASPECT)
	camera_entity:add_aspect(CAMERA_ASPECT)

	camera_entity:configure_camera(p_viewport_width,p_viewport_height, 1.0, 1000000.0)

	local fps_transfo=FPSTransform()
	fps_transfo:configure(camera_entity,0,0,p_x,p_y,p_z,TRUE)

	return camera_entity, fps_transfo
end


commons.create_skybox = function(p_passid, p_rendergraph, p_module, p_front_tx, p_rear_tx, p_left_tx, p_right_tx, p_top_tx, p_bottom_tx, p_scale)

	local skybox_entity=Entity()
	skybox_entity:add_aspect(RENDERING_ASPECT)
	skybox_entity:add_aspect(TRANSFORM_ASPECT)


	local skybox_texture_front = TexturesSet()
	skybox_texture_front:set_texturefiletostage(p_front_tx, 0)
	local skybox_texture_rear = TexturesSet()
	skybox_texture_rear:set_texturefiletostage(p_rear_tx, 0)
	local skybox_texture_left = TexturesSet()
	skybox_texture_left:set_texturefiletostage(p_left_tx, 0)
	local skybox_texture_right = TexturesSet()
	skybox_texture_right:set_texturefiletostage(p_right_tx, 0)
	local skybox_texture_top = TexturesSet()
	skybox_texture_top:set_texturefiletostage(p_top_tx, 0)
	local skybox_texture_bottom = TexturesSet()
	skybox_texture_bottom:set_texturefiletostage(p_bottom_tx, 0)
	local skybox_fxparams = FxParams()
	skybox_fxparams:add_shaderfile('texture.vso',SHADER_COMPILED)
	skybox_fxparams:add_shaderfile('texture.pso',SHADER_COMPILED)
	skybox_rendercontext = RenderContext(p_passid)
	skybox_rendercontext:add_fxparams(skybox_fxparams)
	skybox_rendercontext:add_texturesset(skybox_texture_front)
	skybox_rendercontext:add_texturesset(skybox_texture_rear)
	skybox_rendercontext:add_texturesset(skybox_texture_left)
	skybox_rendercontext:add_texturesset(skybox_texture_right)
	skybox_rendercontext:add_texturesset(skybox_texture_top)
	skybox_rendercontext:add_texturesset(skybox_texture_bottom)
	skybox_rendercontext:set_renderingorder(-1000)
	skybox_renderconfig=RenderConfig()
	skybox_renderconfig:add_rendercontext(skybox_rendercontext)


	local skybox_renderer = SkyboxRendering()
	skybox_renderer:instanciate_renderingimpl(p_module)
	skybox_renderer:attach_toentity(skybox_entity)
	skybox_renderer:configure(skybox_renderconfig)

	skybox_renderer:register_to_rendering(p_rendergraph)

	sb_scale = Matrix();
	sb_scale:scale(p_scale, p_scale, p_scale)

	local sb_transform = RawTransform()
	sb_transform:configure(skybox_entity)
	sb_transform:add_matrix("sb_scaling",sb_scale)

	return skybox_entity, skybox_renderer, sb_transform
end

commons.create_skybox_with_mirror = function(p_passid, p_mirrorpassid, p_rendergraph, p_module, p_front_tx, p_rear_tx, p_left_tx, p_right_tx, p_top_tx, p_bottom_tx, p_scale)

	local skybox_entity=Entity()
	skybox_entity:add_aspect(RENDERING_ASPECT)
	skybox_entity:add_aspect(TRANSFORM_ASPECT)


	local skybox_texture_front = TexturesSet()
	skybox_texture_front:set_texturefiletostage(p_front_tx, 0)
	local skybox_texture_rear = TexturesSet()
	skybox_texture_rear:set_texturefiletostage(p_rear_tx, 0)
	local skybox_texture_left = TexturesSet()
	skybox_texture_left:set_texturefiletostage(p_left_tx, 0)
	local skybox_texture_right = TexturesSet()
	skybox_texture_right:set_texturefiletostage(p_right_tx, 0)
	local skybox_texture_top = TexturesSet()
	skybox_texture_top:set_texturefiletostage(p_top_tx, 0)
	local skybox_texture_bottom = TexturesSet()
	skybox_texture_bottom:set_texturefiletostage(p_bottom_tx, 0)

	local skybox_fxparams = FxParams()
	skybox_fxparams:add_shaderfile('texture.vso',SHADER_COMPILED)
	skybox_fxparams:add_shaderfile('texture.pso',SHADER_COMPILED)
	skybox_rendercontext = RenderContext(p_passid)
	skybox_rendercontext:add_fxparams(skybox_fxparams)
	skybox_rendercontext:add_texturesset(skybox_texture_front)
	skybox_rendercontext:add_texturesset(skybox_texture_rear)
	skybox_rendercontext:add_texturesset(skybox_texture_left)
	skybox_rendercontext:add_texturesset(skybox_texture_right)
	skybox_rendercontext:add_texturesset(skybox_texture_top)
	skybox_rendercontext:add_texturesset(skybox_texture_bottom)
	skybox_rendercontext:set_renderingorder(-1000)



	local mirror_rss=RenderStatesSet()

	mirror_rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "false")
	mirror_rss:add_renderstate_in(RENDERSTATE_OPE_SETCULLING, "ccw")
	mirror_rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")
	mirror_rss:add_renderstate_out(RENDERSTATE_OPE_SETCULLING, "cw")

	local skybox_fxparams_mirror = FxParams()

	skybox_fxparams_mirror:add_shaderfile('texture_mirror.vso',SHADER_COMPILED)
	skybox_fxparams_mirror:add_shaderfile('texture_mirror.pso',SHADER_COMPILED)


	skybox_rendercontext_mirror = RenderContext(p_mirrorpassid)
	skybox_fxparams_mirror:set_renderstatesset(mirror_rss)
	
	skybox_rendercontext_mirror:add_fxparams(skybox_fxparams_mirror)
	skybox_rendercontext_mirror:add_texturesset(skybox_texture_front)
	skybox_rendercontext_mirror:add_texturesset(skybox_texture_rear)
	skybox_rendercontext_mirror:add_texturesset(skybox_texture_left)
	skybox_rendercontext_mirror:add_texturesset(skybox_texture_right)
	skybox_rendercontext_mirror:add_texturesset(skybox_texture_top)
	skybox_rendercontext_mirror:add_texturesset(skybox_texture_bottom)
	skybox_rendercontext_mirror:set_renderingorder(-1000)


	skybox_rendercontext_mirror:add_shaderparam("reflector_pos", 0, 24)
	skybox_rendercontext_mirror:add_shaderparam("reflector_normale", 0, 25)

	


	skybox_renderconfig=RenderConfig()
	skybox_renderconfig:add_rendercontext(skybox_rendercontext)
	skybox_renderconfig:add_rendercontext(skybox_rendercontext_mirror)



	local skybox_renderer = SkyboxRendering()
	skybox_renderer:instanciate_renderingimpl(p_module)
	skybox_renderer:attach_toentity(skybox_entity)
	skybox_renderer:configure(skybox_renderconfig)

	skybox_renderer:register_to_rendering(p_rendergraph)

	sb_scale = Matrix();
	sb_scale:scale(p_scale, p_scale, p_scale)

	local sb_transform = RawTransform()
	sb_transform:configure(skybox_entity)
	sb_transform:add_matrix("sb_scaling",sb_scale)

	return skybox_entity, skybox_renderer, sb_transform
end


commons.create_unlit_meshe = function( p_rendergraph, p_passname, p_meshefile, p_meshe_index, p_texturefile)
	
	local meshe_entity=Entity()
	meshe_entity:add_aspect(RENDERING_ASPECT)
	meshe_entity:add_aspect(TRANSFORM_ASPECT)

	local rss=RenderStatesSet()
	rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "true")
	rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")


	local textures = TexturesSet()
	textures:set_texturefiletostage(p_texturefile, 0)

	local fxparams = FxParams()
	fxparams:add_shaderfile('texture.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('texture.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(rss)

	rendercontext = RenderContext(p_passname)

	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)

	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)

	local renderer=MesheRendering()
	renderer:attach_toentity(meshe_entity)


	renderer:configure(renderconfig,p_meshefile,p_meshe_index)

	renderer:register_to_rendering(p_rendergraph)

	return meshe_entity, renderer

end

commons.create_lit_meshe = function( p_rendergraph, p_passname, p_meshefile, p_meshe_index, p_texturefile, p_spherical_normales)
	
	local meshe_entity=Entity()
	meshe_entity:add_aspect(RENDERING_ASPECT)
	meshe_entity:add_aspect(TRANSFORM_ASPECT)

	local rss=RenderStatesSet()
	rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "true")
	rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")


	local textures = TexturesSet()
	textures:set_texturefiletostage(p_texturefile, 0)

	local fxparams = FxParams()
	fxparams:add_shaderfile('lit.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('lit.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(rss)

	rendercontext = RenderContext(p_passname)

	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)

	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)

	local renderer=MesheRendering()
	renderer:attach_toentity(meshe_entity)


	renderer:configure(renderconfig,p_meshefile,p_meshe_index,p_spherical_normales)

	renderer:register_to_rendering(p_rendergraph)

	return meshe_entity, renderer

end



commons.create_colored_meshe = function( p_rendergraph, p_passname, p_meshefile, p_meshe_index)
	
	local meshe_entity=Entity()
	meshe_entity:add_aspect(RENDERING_ASPECT)
	meshe_entity:add_aspect(TRANSFORM_ASPECT)

	local rss=RenderStatesSet()
	rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "true")
	rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")


	local textures = TexturesSet()

	local fxparams = FxParams()
	fxparams:add_shaderfile('color.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('color.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(rss)

	rendercontext = RenderContext(p_passname)

	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)

	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)


	rendercontext:add_shaderparam( "color", 1, 0 )

	local renderer=MesheRendering()
	renderer:attach_toentity(meshe_entity)


	renderer:configure(renderconfig,p_meshefile,p_meshe_index)

	renderer:register_to_rendering(p_rendergraph)


	renderer:set_shaderrealvector(p_passname, "color", 1.0, 0.0, 1.0, 1.0 )

	return meshe_entity, renderer

end




commons.create_unlit_meshe_mirror = function( p_rendergraph, p_passname, p_mirrorpassname, p_meshefile, p_meshe_index, p_texturefile)
	
	local meshe_entity=Entity()
	meshe_entity:add_aspect(RENDERING_ASPECT)
	meshe_entity:add_aspect(TRANSFORM_ASPECT)


	local textures = TexturesSet()
	textures:set_texturefiletostage(p_texturefile, 0)





	local rss=RenderStatesSet()
	rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "true")
	rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")




	local fxparams = FxParams()
	fxparams:add_shaderfile('texture.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('texture.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(rss)

	rendercontext = RenderContext(p_passname)

	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)





	local mirror_rss=RenderStatesSet()

	mirror_rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "true")
	mirror_rss:add_renderstate_in(RENDERSTATE_OPE_SETCULLING, "ccw")
	mirror_rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")
	mirror_rss:add_renderstate_out(RENDERSTATE_OPE_SETCULLING, "cw")


	local fxparams_mirror = FxParams()
	fxparams_mirror:add_shaderfile('texture_mirror.vso',SHADER_COMPILED)
	fxparams_mirror:add_shaderfile('texture_mirror.pso',SHADER_COMPILED)
	fxparams_mirror:set_renderstatesset(mirror_rss)

	rendercontext_mirror = RenderContext(p_mirrorpassname)
	rendercontext_mirror:add_fxparams(fxparams_mirror)
	rendercontext_mirror:add_texturesset(textures)

	rendercontext_mirror:add_shaderparam("reflector_pos", 0, 24)
	rendercontext_mirror:add_shaderparam("reflector_normale", 0, 25)



	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)
	renderconfig:add_rendercontext(rendercontext_mirror)

	local renderer=MesheRendering()
	renderer:attach_toentity(meshe_entity)


	renderer:configure(renderconfig,p_meshefile,p_meshe_index)

	renderer:register_to_rendering(p_rendergraph)

	return meshe_entity, renderer

end


commons.create_unlit_landmeshe_mirror = function( p_rendergraph, p_passname, p_mirrorpassname, p_meshefile, p_meshe_index, p_texturefile, p_vtexturefile)
	
	local meshe_entity=Entity()
	meshe_entity:add_aspect(RENDERING_ASPECT)
	meshe_entity:add_aspect(TRANSFORM_ASPECT)


	local textures = TexturesSet()
	textures:set_texturefiletostage(p_texturefile, 0)


	local vertex_textures = TexturesSet()
	vertex_textures:set_texturefiletostage(p_vtexturefile, 0)


	local rss=RenderStatesSet()
	rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "true")
	rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")




	local fxparams = FxParams()
	fxparams:add_shaderfile('heightmap.vso',SHADER_COMPILED)
	fxparams:add_shaderfile('heightmap.pso',SHADER_COMPILED)
	fxparams:set_renderstatesset(rss)

	rendercontext = RenderContext(p_passname)

	rendercontext:add_fxparams(fxparams)
	rendercontext:add_texturesset(textures)
	rendercontext:add_vertextexturesset(vertex_textures)




	
	local mirror_rss=RenderStatesSet()

	mirror_rss:add_renderstate_in(RENDERSTATE_OPE_ENABLEZBUFFER, "true")
	mirror_rss:add_renderstate_in(RENDERSTATE_OPE_SETCULLING, "ccw")
	mirror_rss:add_renderstate_out(RENDERSTATE_OPE_ENABLEZBUFFER, "false")
	mirror_rss:add_renderstate_out(RENDERSTATE_OPE_SETCULLING, "cw")


	local fxparams_mirror = FxParams()
	fxparams_mirror:add_shaderfile('heightmap_mirror.vso',SHADER_COMPILED)
	fxparams_mirror:add_shaderfile('heightmap_mirror.pso',SHADER_COMPILED)
	fxparams_mirror:set_renderstatesset(mirror_rss)

	rendercontext_mirror = RenderContext(p_mirrorpassname)
	rendercontext_mirror:add_fxparams(fxparams_mirror)
	rendercontext_mirror:add_texturesset(textures)
	rendercontext_mirror:add_vertextexturesset(vertex_textures)

	rendercontext_mirror:add_shaderparam("reflector_pos", 0, 24)
	rendercontext_mirror:add_shaderparam("reflector_normale", 0, 25)



	renderconfig=RenderConfig()
	renderconfig:add_rendercontext(rendercontext)
	renderconfig:add_rendercontext(rendercontext_mirror)

	local renderer=MesheRendering()
	renderer:attach_toentity(meshe_entity)


	renderer:configure(renderconfig,p_meshefile,p_meshe_index)

	renderer:register_to_rendering(p_rendergraph)

	return meshe_entity, renderer

end


commons.trash.skybox = function(p_rendergraph, p_module, p_entity, p_renderer, p_transform)

  p_transform:release()

  p_renderer:unregister_from_rendering(p_rendergraph)
  p_renderer:release()
  p_renderer:detach_fromentity()
  p_renderer:trash_renderingimpl(p_module)
  
  p_entity:remove_aspect(TRANSFORM_ASPECT)
  p_entity:remove_aspect(RENDERING_ASPECT)

end

commons.trash.meshe = function(p_rendergraph, p_entity, p_renderer)

	p_renderer:unregister_from_rendering(p_rendergraph)
	p_renderer:release()
	p_renderer:detach_fromentity()

	p_entity:remove_aspect(TRANSFORM_ASPECT)
	p_entity:remove_aspect(RENDERING_ASPECT)

end

commons.trash.fps_camera = function(p_camera_entity, p_fps_transfo)
	p_fps_transfo:release()

	p_camera_entity:release_camera()
	p_camera_entity:remove_aspect(TRANSFORM_ASPECT)
	p_camera_entity:remove_aspect(CAMERA_ASPECT)
end

commons.trash.final_pass = function(p_rendergraph, p_passid)
	p_rendergraph:release_pass_viewportquad_resources(p_passid)
	p_rendergraph:remove_pass_viewportquad(p_passid)
end

