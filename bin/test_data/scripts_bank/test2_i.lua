
include("model_main.lua")
include('terrain_model.lua')
include('boulder_model.lua')
include('metalcube_model.lua')
include('spherebump_model.lua')
include('container_model.lua')


g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

commons.init_final_pass(rg, 'final_pass')

rg:create_child('final_pass', 'transfer_pass', 0)

rg:create_pass_viewportquad('transfer_pass')
transferpass_rss=RenderStatesSet()
transferpass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
transferpass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")
transfer_textures = TexturesSet()
transfer_fxparams = FxParams()
transfer_fxparams:add_shaderfile('transfer.vso',SHADER_COMPILED)
transfer_fxparams:add_shaderfile('transfer.pso',SHADER_COMPILED)
transfer_fxparams:set_renderstatesset(transferpass_rss)

transfer_rendercontext = RenderContext('transfer_pass')
transfer_rendercontext:add_fxparams(transfer_fxparams)
transfer_rendercontext:add_shaderparam("camera_params", 1, 0)
transfer_rendercontext:add_shaderparam("view_matrix", 1, 1)
transfer_rendercontext:add_shaderparam("pos_matrix", 1, 5)
transfer_rendercontext:add_shaderparam("container_half_dims", 1, 9)

transfer_rendercontext:add_texturesset(transfer_textures)

transfer_renderconfig=RenderConfig()
transfer_renderconfig:add_rendercontext(transfer_rendercontext)
rg:configure_pass_viewportquad_resources('transfer_pass',transfer_renderconfig)


rg:create_child('transfer_pass', 'texture_pass', 0)

--rg:create_child('final_pass', 'texture_pass', 0)

root_entity:add_aspect(PHYSICS_ASPECT)

model.createmaincamera(0.0, 0.0, 0.0, mvt_mod)
eg:set_camera(model.camera.entity)


camera_width, camera_height, zn, zf = model.camera.entity:read_cameraparams()

g:print('camera params = '..camera_width..' '..camera_height..' '..zn..' '..zf )
rg:set_viewportquadshaderrealvector('transfer_pass', 'camera_params', camera_width, camera_height, zn, zf)
rg:set_viewportquadshaderrealvector('transfer_pass', 'container_half_dims', 15.0, 2.0, 15.0, 0.0)


rg:update_renderingqueues()

mouse_right = FALSE


g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  local mvt_info = { model.camera.mvt:read() }
  if mouse_right == FALSE then
	model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
  else
	model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
  end
end)

g:add_mouserightbuttondowncb( "onmouserightbuttondown", 
function( xm, ym )
  mouse_right = TRUE
end)

g:add_mouserightbuttonupcb( "onmouserightbuttonup", 
function( xm, ym )
  mouse_right = FALSE
end)

g:add_appruncb( "run",
function()

  local mvt_info = { model.camera.mvt:read() }
  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  -- inject camera matrix into 
  local view_mat = Matrix()
  view_mat:store_entitytransformation(model.camera.entity, TRANSFORMATION_VIEW_MATRIX)

  -- inverse view to get camera matrix
  view_mat:inverse()


  -- box positionning in world
  local box_pos = Matrix()
  box_pos:translation( 10.0, 6.0, 0.0 )

  local box_rot = Matrix()
  box_rot:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(0))

  local box_mat = Matrix()
  box_mat:set_product(box_rot, box_pos)

  rg:set_viewportquadshaderrealmatrix('transfer_pass', 'view_matrix', view_mat)
  rg:set_viewportquadshaderrealmatrix('transfer_pass', 'pos_matrix', box_mat)



end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(12.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      
  --W key
  elseif key == 87 then

    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(-12.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
  end

end)

g:add_keyupcb( "keyup",
function( key )  

  last_key = key

  --Q key
  if key == 81 then
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
    
  --W key
  elseif key == 87 then
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(0.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
     
  end
end)

g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")

root_entity:configure_world(environment.gravity_state, environment.gravity.x, environment.gravity.y, environment.gravity.z )

--terrain.view.flatcolor.load('terrain', {flatcolor_rendering='texture_pass'}, 'root')
terrain.view.load('terrain', {lit_rendering='texture_pass'}, 'root')

boulder.view.load('rock', {x = 20.0, y = -3.0, z = 0.0}, {lit_rendering='texture_pass'}, 'root' )
--metalcube.view.load('c', {x = 0.0, y = 2.0, z = -15.0}, {lit_rendering='texture_pass'}, 'root' )
spherebump.view.load('s', {x = 10.0, y = 17.0, z = 5.8}, {lit_rendering='texture_pass'}, 'root' )

container.view.load('container', {main_rendering='texture_pass'}, 'root')
model.move.setpos('container', 10.0, 6.0, 0.0)

model.env.setgravity(1)	
model.env.setbkcolor(0.55,0.55,0.99)

model.camera.mvt:set_pos(0.0, 18.0, 25.0)

model.env.fog.setdensity(0.03)
model.env.fog.setcolor(0.55,0.55,0.99)

model.env.light.setstate( TRUE )
model.env.light.setdir(1.0, -0.2, 0.0)
model.env.ambientlight.setcolor(0.1, 0.1, 0.1)

