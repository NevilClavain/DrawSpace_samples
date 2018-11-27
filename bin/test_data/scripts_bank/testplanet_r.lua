

g:signal_rendersceneend("eg")




sb_transform:release()
skybox_entity:remove_aspect(TRANSFORM_ASPECT)
commons.trash.rendering(rg, sb_mod, skybox_entity, skybox_renderer)
eg:remove('skybox_entity')




planet_entity:remove_aspect(TRANSFORM_ASPECT)
commons.trash.rendering(rg, pl_mod, planet_entity, planet_renderer)
eg:remove('planet_entity')


commons.trash.camera( camera_entity, camera_mvt)
eg:remove('camera_entity')




text_renderer:release()

g:remove_appruncb('run')



rg:remove_pass('texture_pass')

commons.trash.final_pass(rg, 'final_pass')


rg:update_renderingqueues()

g:remove_mousemovecb("onmousemove")
g:remove_keydowncb("keydown")
g:remove_keyupcb("keyup")


g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)



mvt_mod:unload()
pl_mod:unload()


-- classes instances cleanup


text_renderer = nil

camera_entity = nil
fps_transfo = nil


skybox_entity = nil
skybox_renderer = nil
sb_transform = nil


fps_yaw = nil
fps_pitch = nil

