

g:signal_rendersceneend("eg")


spaceboxmod.view.unload('spacebox0')

model.releasemaincamera()

g:remove_appruncb('run')


rg:remove_pass('texture_pass')


commons.trash.final_pass(rg, 'final_pass')

rg:update_renderingqueues()
g:release_assets()


g:show_mousecursor(TRUE)
g:set_mousecursorcircularmode(FALSE)



mvt_mod:unload()