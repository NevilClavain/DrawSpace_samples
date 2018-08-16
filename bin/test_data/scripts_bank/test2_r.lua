

g:signal_rendersceneend("eg")

clothbox_transform:release()

commons.trash.meshe(rg, clothbox_entity, clothbox_renderer)
eg:remove('clothbox_entity')


sphere_body:release()
sphere_body:detach_fromentity(sphere_entity)
sphere_entity:remove_aspect(BODY_ASPECT)
commons.trash.meshe(rg, sphere_entity, sphere_renderer)
eg:remove('sphere_entity')


ground_body:release()
ground_body:detach_fromentity(ground_entity)
ground_entity:remove_aspect(BODY_ASPECT)
commons.trash.meshe(rg, ground_entity, ground_renderer)
eg:remove('ground_entity')



rock_body:release()
rock_body:detach_fromentity(rock_entity)
rock_entity:remove_aspect(BODY_ASPECT)
commons.trash.meshe(rg, rock_entity, rock_renderer)
eg:remove('rock_entity')

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


root_entity:release_world()

root_entity:remove_aspect(PHYSICS_ASPECT)