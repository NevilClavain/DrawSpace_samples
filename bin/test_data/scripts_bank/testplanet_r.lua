

g:signal_rendersceneend("eg")


eg:remove('skybox_entity')
sb_transform:release()
skybox_entity:remove_aspect(TRANSFORM_ASPECT)
skybox_entity:release_info("entity_name")
skybox_entity:remove_aspect(INFOS_ASPECT)
commons.trash.rendering(rg, sb_mod, skybox_entity, skybox_renderer)


--[[
eg:remove('sphere_entity')
sphere_body:release()
sphere_body:detach_fromentity(sphere_entity)
sphere_entity:remove_aspect(BODY_ASPECT)
sphere_entity:release_info("entity_name")
sphere_entity:remove_aspect(INFOS_ASPECT)
commons.trash.meshe(rg, sphere_entity, sphere_renderer)




eg:remove('camera2_entity')
commons.trash.camera( camera2_entity, camera2_pos)



eg:remove('ship_entity')
ship_body:release()
ship_body:detach_fromentity(ship_entity)
ship_entity:remove_aspect(BODY_ASPECT)
ship_entity:release_info("entity_name")
ship_entity:remove_aspect(INFOS_ASPECT)
commons.trash.meshe(rg, ship_entity, ship_renderer)
]]




eg:remove('camera_entity')
commons.trash.camera( camera_entity, camera_mvt)


planet_transform:release()

planet_specific_config:cleanup(planet_renderer)

planet_entity:remove_aspect(TRANSFORM_ASPECT)
planet_entity:release_info("entity_name")
planet_entity:remove_aspect(INFOS_ASPECT)
commons.trash.rendering(rg, pl_mod, planet_entity, planet_renderer)
eg:remove('planet_entity')






text_renderer:release()

g:remove_appruncb('run')



rg:remove_pass('texture_pass')

commons.trash.final_pass(rg, 'final_pass')


rg:update_renderingqueues()


eg:unset_camera()

g:remove_mousemovecb("onmousemove")
g:remove_keydowncb("keydown")
g:remove_keyupcb("keyup")


g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)


root_entity:release_world()
root_entity:remove_aspect(PHYSICS_ASPECT)


mvt_mod:unload()
pl_mod:unload()
sb_mod:unload()


-- classes instances cleanup


text_renderer = nil

camera_entity = nil
fps_transfo = nil


skybox_entity = nil
skybox_renderer = nil
sb_transform = nil


fps_yaw = nil
fps_pitch = nil

