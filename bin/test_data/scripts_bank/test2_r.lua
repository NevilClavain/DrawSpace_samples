

commons.trash.skybox(rg, sb_mod, skybox_entity, skybox_renderer, sb_transform)
eg:remove('skybox_entity')



commons.trash.meshe(rg, ground_entity, ground_renderer)
eg:remove('ground_entity')


commons.trash.fps_camera( camera_entity, fps_transfo)
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