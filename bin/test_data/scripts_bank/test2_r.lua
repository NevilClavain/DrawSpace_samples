

g:signal_rendersceneend("eg")


terrain.view.unload('terrain')

model.releasemaincamera()

g:remove_appruncb('run')


rg:remove_pass('texture_pass')
commons.trash.final_pass(rg, 'final_pass')

rg:update_renderingqueues()
g:release_assets()


g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)


root_entity:release_world()
root_entity:remove_aspect(PHYSICS_ASPECT)

mvt_mod:unload()