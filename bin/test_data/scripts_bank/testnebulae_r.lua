

g:signal_rendersceneend("eg")




sb_transform:release()
skybox_entity:remove_aspect(TRANSFORM_ASPECT)
commons.trash.rendering(rg, sb_mod, skybox_entity, skybox_renderer)
eg:remove('skybox_entity')


neb_transform:release()
neb_entity:remove_aspect(TRANSFORM_ASPECT)
nebulae_specific_config:cleanup(neb_renderer)
commons.trash.rendering(rg, vol_mod, neb_entity, neb_renderer)
eg:remove('nebulae_entity')



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

vol_mod:unload()
sb_mod:unload()
mvt_mod:unload()


-- classes instances cleanup

sb_mod = nil
text_renderer = nil

camera_entity = nil
fps_transfo = nil


skybox_entity = nil
skybox_renderer = nil
sb_transform = nil


fps_yaw = nil
fps_pitch = nil

