
g:signal_rendersceneend("eg")

text_renderer:release()

g:remove_appruncb('run')

rg:release_pass_viewportquad_resources('final_pass')
rg:remove_pass_viewportquad('final_pass')


g:release_assets()
--rg:update_renderingqueues()