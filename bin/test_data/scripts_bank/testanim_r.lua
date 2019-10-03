

g:signal_rendersceneend("eg")


ground_body:release()
ground_body:detach_fromentity(ground_entity)
ground_entity:remove_aspect(BODY_ASPECT)
commons.trash.meshe(rg, ground_entity, ground_renderer)
eg:remove('ground_entity')


dino_transform:release()
dino_entity:release_animationbones()
dino_entity:remove_aspect(ANIMATION_ASPECT)
commons.trash.meshe(rg, dino_entity, dino_renderer)
eg:remove('dino_entity')



neck_transform:release()
--neck_entity:remove_aspect(ANIMATION_ASPECT)
commons.trash.meshe(rg, neck_entity, neck_renderer)
eg:remove('neck_entity')


commons.trash.camera(camera_entity, camera_mvt)
eg:remove('camera_entity')

text_renderer:release()
text_anim_infos:release()

g:remove_appruncb('run')


rg:remove_pass('texture_pass')

commons.trash.final_pass(rg, 'final_pass')

rg:update_renderingqueues()
g:release_assets()

g:remove_mousemovecb("onmousemove")
g:remove_keydowncb("keydown")
g:remove_keyupcb("keyup")

g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)


root_entity:release_world()

root_entity:remove_aspect(PHYSICS_ASPECT)

mvt_mod:unload()