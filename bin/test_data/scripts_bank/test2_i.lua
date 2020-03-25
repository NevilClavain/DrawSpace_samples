
include("model_main.lua")
include('terrain_model.lua')


g:print('Current renderer is '..model.renderer_infos[1]..', '..model.renderer_infos[2]..'x'..model.renderer_infos[3])


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

commons.init_final_pass(rg, 'final_pass')
rg:create_child('final_pass', 'texture_pass', 0)

root_entity:add_aspect(PHYSICS_ASPECT)

model.createmaincamera(0.0, 5.5, 0.0, mvt_mod)

eg:set_camera(model.camera.entity)

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


terrain.view.load('terrain', {lit_rendering='texture_pass'}, 'root')


model.env.setgravity(1)	
model.env.setbkcolor(0.55,0.55,0.99)

model.camera.mvt:set_pos(0.0, 1.4, 5.0)

model.env.fog.setdensity(0.03)
model.env.fog.setcolor(0.55,0.55,0.99)

model.env.light.setstate( TRUE )
model.env.light.setdir(1.0, -0.2, 0.0)
model.env.ambientlight.setcolor(0.1, 0.1, 0.1)
