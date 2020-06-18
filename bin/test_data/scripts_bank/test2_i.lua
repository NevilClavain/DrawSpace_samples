
include("model_main.lua")
include('terrain_model.lua')
include('boulder_model.lua')
include('metalcube_model.lua')
include('spherebump_model.lua')
include('container_model.lua')

ctrl_key = FALSE

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
rg:create_child('transfer_pass', 'zmask_pass', 1)

rg:create_child('transfer_pass', 'noise_pass', 2, RENDERPURPOSE_COLOR, RENDERTARGET_GPU, FALSE, 128, 128)

rg:create_pass_viewportquad('noise_pass')
noisepass_rss=RenderStatesSet()
noisepass_rss:add_renderstate_in(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "point")
noisepass_rss:add_renderstate_out(RENDERSTATE_OPE_SETTEXTUREFILTERTYPE, "linear")
noise_textures = TexturesSet()
noise_fxparams = FxParams()
noise_fxparams:add_shaderfile('noiser.vso',SHADER_COMPILED)
noise_fxparams:add_shaderfile('noiser.pso',SHADER_COMPILED)
noise_fxparams:set_renderstatesset(noisepass_rss)

noise_rendercontext = RenderContext('noise_pass')
noise_rendercontext:add_fxparams(noise_fxparams)
noise_rendercontext:add_shaderparam("coords", 1, 1)

noise_rendercontext:add_texturesset(noise_textures)

noise_renderconfig=RenderConfig()
noise_renderconfig:add_rendercontext(noise_rendercontext)
rg:configure_pass_viewportquad_resources('noise_pass',noise_renderconfig)

rg:set_targettexturedepth('noise_pass', 128)
rg:set_pass_targetslice('noise_pass', 0)

root_entity:add_aspect(PHYSICS_ASPECT)

model.createmaincamera(0.0, 0.0, 0.0, mvt_mod)
eg:set_camera(model.camera.entity)


camera_width, camera_height, zn, zf = model.camera.entity:read_cameraparams()

g:print('camera params = '..camera_width..' '..camera_height..' '..zn..' '..zf )
rg:set_viewportquadshaderrealvector('transfer_pass', 'camera_params', camera_width, camera_height, zn, zf)
rg:set_viewportquadshaderrealvector('transfer_pass', 'container_half_dims', container.scale.x * 0.5, container.scale.y * 0.5, container.scale.z * 0.5, 0.0)




container_angle_deg = 0.0

rg:update_renderingqueues()

mouse_right = FALSE


g:add_mousemovecb( "onmousemove",
function( xm, ym, dx, dy )  

  if ctrl_key == TRUE then
    container_angle_deg = container_angle_deg + dx
    model.target = 'cont'
    model.move.roty()
    transformations_update(dx)
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


  -- box positionning in world
  local box_pos = Matrix()
  box_pos:translation( 0.0, 56.0, 0.0 )

  local box_rot = Matrix()
  box_rot:rotation(0.0, 1.0, 0.0, commons.utils.deg_to_rad(container_angle_deg))



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


noise_slice = 0
ready = FALSE
rg:add_renderpasseventcb("render pass event", 
function(evt, passid)

   if evt==RENDERINGQUEUE_UPDATED then
     ready = TRUE
   end

   if ready==TRUE and evt==RENDERINGQUEUE_PASS_END and passid == 'noise_pass' then
       

     noise_slice = noise_slice + 1

     if noise_slice < 42 then
       rg:set_viewportquadshaderrealvector('noise_pass', 'coords', 0.25, 0.25, 0.75, 0.75)

     elseif noise_slice < 84 then
       rg:set_viewportquadshaderrealvector('noise_pass', 'coords', 0.35, 0.35, 0.65, 0.65)

     else
       rg:set_viewportquadshaderrealvector('noise_pass', 'coords', 0.45, 0.45, 0.55, 0.55)

     end

     if noise_slice == 128 then
       rg:disable_pass("noise_pass")
     else
       rg:set_pass_targetslice('noise_pass', noise_slice)
     end

   end   
end
)


root_entity:configure_world(environment.gravity_state, environment.gravity.x, environment.gravity.y, environment.gravity.z )


terrain_passes_config = 
{
    texture_pass = 
    {
        rendering_id = 'lit_rendering',
        lit_shader_update_func = terrain.update_lit_from_scene_env
	},
    zmask_pass = 
    {
        rendering_id = 'flatcolor_rendering',
        lit_shader_update_func = nil
	}
}
terrain.view.load('terrain', terrain_passes_config, 'root')
terrain.update_flatcolor( 'zmask_pass', 0.0, 0.0, 0.0, 1.0, 'terrain')

boulder_passes_config = 
{
    texture_pass = 
    {
        rendering_id = 'lit_rendering',
        lit_shader_update_func = boulder.update_lit_from_scene_env
	},
    zmask_pass = 
    {
        rendering_id = 'flatcolor_rendering',
        lit_shader_update_func = nil
	}
}
boulder.view.load('rock', {x = 20.0, y = -3.0, z = 0.0}, boulder_passes_config, 'root' )
boulder.update_flatcolor( 'zmask_pass', 0.0, 0.0, 0.0, 1.0, 'rock')

spherebump_passes_config = 
{
    texture_pass = 
    {
        rendering_id = 'lit_rendering',
        lit_shader_update_func = spherebump.update_from_scene_env
	},
    zmask_pass = 
    {
        rendering_id = 'flatcolor_rendering',
        lit_shader_update_func = nil
	}
}
spherebump.view.load('s', {x = 10.0, y = 17.0, z = 5.8}, spherebump_passes_config, 'root' )
spherebump.update_flatcolor( 'zmask_pass', 0.0, 0.0, 0.0, 1.0, 's')

metalcube_passes_config = 
{
    texture_pass = 
    {
        rendering_id = 'lit_rendering',
        lit_shader_update_func = metalcube.update_from_scene_env
	},
    zmask_pass = 
    {
        rendering_id = 'flatcolor_rendering',
        lit_shader_update_func = nil
	}
}
metalcube.view.load('mc', {x = -18.0, y = 5.9, z = -18.8}, metalcube_passes_config, 'root' )
metalcube.update_flatcolor( 'zmask_pass', 0.0, 0.0, 0.0, 1.0, 'mc')

container_passes_config = 
{
    zmask_pass = 
    {
        rendering_id = 'flatcolor_rendering',
        lit_shader_update_func = nil
	}
}
container.view.load('cont', container_passes_config, 'root')
container.update_flatcolor( 'zmask_pass', 1.0, 1.0, 1.0, 1.0, 'cont')

model.move.setpos('cont', 0.0, 56.0, 0.0)

model.env.setgravity(1)	
model.env.setbkcolor('texture_pass', 0.55,0.55,0.99)
model.env.setbkcolor('zmask_pass', 0.0, 0.0, 0.0)
model.env.setbkcolor('noise_pass', 0.0, 0.0, 0.0)

model.camera.mvt:set_pos(0.0, 68.0, 65.0)

model.env.fog.setdensity(0.03)
model.env.fog.setcolor(0.55,0.55,0.99)

model.env.light.setstate( TRUE )
model.env.light.setdir(1.0, -0.4, 0.0)
model.env.ambientlight.setcolor(0.0, 0.0, 0.0)

