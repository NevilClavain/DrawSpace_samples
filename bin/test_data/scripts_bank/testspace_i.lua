
include("model_main.lua")
include('spacebox_model.lua')

ctrl_key = FALSE

g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])
renderer_infos = {renderer:descr()}
g:print('Current resolution is '..renderer_infos[2].." "..renderer_infos[3])


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
transfer_fxparams:add_shaderfile('transfer2.vso',SHADER_COMPILED)
transfer_fxparams:add_shaderfile('transfer2.pso',SHADER_COMPILED)
transfer_fxparams:set_renderstatesset(transferpass_rss)

transfer_rendercontext = RenderContext('transfer_pass')
transfer_rendercontext:add_fxparams(transfer_fxparams)
transfer_rendercontext:add_shaderparam("camera_params", 1, 0)
transfer_rendercontext:add_shaderparam("view_matrix", 1, 1)
transfer_rendercontext:add_shaderparam("pos_matrix", 1, 5)
transfer_rendercontext:add_shaderparam("resol", 1, 10)


transfer_rendercontext:add_texturesset(transfer_textures)

transfer_renderconfig=RenderConfig()
transfer_renderconfig:add_rendercontext(transfer_rendercontext)
rg:configure_pass_viewportquad_resources('transfer_pass',transfer_renderconfig)

rg:create_child('transfer_pass', 'texture_pass', 0)


model.createmaincamera(0.0, 0.0, 0.0, mvt_mod)
eg:set_camera(model.camera.entity)


camera_width, camera_height, zn, zf = model.camera.entity:read_cameraparams()

g:print('camera params = '..camera_width..' '..camera_height..' '..zn..' '..zf )
rg:set_viewportquadshaderrealvector('transfer_pass', 'camera_params', camera_width, camera_height, zn, zf)
rg:set_viewportquadshaderrealvector('transfer_pass', 'resol', renderer_infos[2], renderer_infos[3], 0.0, 0.0)


container_angle_y_deg = 0.0
container_angle_x_deg = 0.0

rg:update_renderingqueues()

mouse_right = FALSE


g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if ctrl_key == TRUE then

    container_angle_y_deg = container_angle_y_deg + dx
    container_angle_x_deg = container_angle_x_deg + dy

  else
    local mvt_info = { model.camera.mvt:read() }
    if mouse_right == FALSE then
  	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],-dy / 4.0,-dx / 4.0, 0)
    else
	  model.camera.mvt:update(mvt_info[4],mvt_info[1],mvt_info[2],mvt_info[3],0,0,-dx)
    end
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


  -- sphere positionning in world
  local sphere_pos = Matrix()
  sphere_pos:translation( 0.0, 0.0, 0.0 )

  local sphere_scale = Matrix()
  sphere_scale:scale(1.0, 1.0, 1.0)

  local sphere_roty = Matrix()
  sphere_roty:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(container_angle_y_deg))

  local sphere_rotx = Matrix()
  sphere_rotx:rotation(1.0, 0.0, 0.0, commons.utils.deg_to_rad(container_angle_x_deg))

  local sphere_mat_1 = Matrix()
  sphere_mat_1:set_product(sphere_rotx, sphere_roty)

  local sphere_mat_2 = Matrix()
  sphere_mat_2:set_product(sphere_scale, sphere_mat_1)


  local sphere_mat = Matrix()
  sphere_mat:set_product(sphere_mat_2, sphere_pos)

  rg:set_viewportquadshaderrealmatrix('transfer_pass', 'view_matrix', view_mat)
  rg:set_viewportquadshaderrealmatrix('transfer_pass', 'pos_matrix', sphere_mat)


end)

g:add_keydowncb( "keydown",
function( key )

  --Q key
  if key == 81 then 
    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(5.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)
      
  --W key
  elseif key == 87 then

    local mvt_info = { model.camera.mvt:read() }

	model.camera.mvt:update(-5.0,mvt_info[1],mvt_info[2],mvt_info[3],0,0,0)

  elseif key == 17 then
    ctrl_key = TRUE

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

  elseif key == 17 then
    ctrl_key = FALSE
     
  end
end)


g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")

spacebox_passes_config = 
{
	texture_pass = 
	{
		rendering_id = 'layer0_rendering',
		lit_shader_update_func = nil
	}
}
spaceboxmod.view.load('spacebox0', spacebox_passes_config, 'root')


model.env.setbkcolor('texture_pass', 0.0,0.0,0.0)

model.camera.mvt:set_pos(0.0, 0.0, 10.0)


model.env.light.setstate( TRUE )
model.env.light.setdir(1.0, -0.4, 0.0)
model.env.ambientlight.setcolor(0.1, 0.1, 0.1)

