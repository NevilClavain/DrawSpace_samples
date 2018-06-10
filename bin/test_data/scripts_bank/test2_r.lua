

clothbox_transform:release()

commons.trash.meshe(rg, clothbox_entity, clothbox_renderer)
eg:remove('clothbox_entity')

commons.trash.meshe(rg, sphere_entity, sphere_renderer)
eg:remove('sphere_entity')

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