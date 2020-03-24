
include("model_main.lua")
include('terrain_model.lua')


renderer_infos = {renderer:descr()}
g:print('Current renderer is '..renderer_infos[1]..', '..renderer_infos[2]..'x'..renderer_infos[3])


mvt_mod = Module("mvtmod", "mvts")
mvt_mod:load()
g:print(mvt_mod:get_descr().. ' loaded')

commons.init_final_pass(rg, 'final_pass')
rg:create_child('final_pass', 'texture_pass', 0)

root_entity:add_aspect(PHYSICS_ASPECT)

model.camera.entity, model.camera.mvt=commons.create_free_camera(0.0, 5.0, 0, renderer_infos[5],renderer_infos[6], mvt_mod, "model.camera")

eg:add_child('root','model.camera.entity',model.camera.entity)

eg:set_camera(model.camera.entity)

rg:update_renderingqueues()

g:add_appruncb( "run",
function()  
end)

g:show_mousecursor(FALSE)
g:set_mousecursorcircularmode(TRUE)

g:signal_renderscenebegin("eg")

root_entity:configure_world(environment.gravity_state, environment.gravity.x, environment.gravity.y, environment.gravity.z )


terrain.view.load('terrain', {lit_rendering='texture_pass'}, 'root')


model.env.setgravity(1)	
model.env.setbkcolor(0.55,0.55,0.99)

model.camera.mvt:set_pos(0.0, 5.0, 50.0)

model.env.fog.setdensity(0.03)
model.env.fog.setcolor(0.55,0.55,0.99)
