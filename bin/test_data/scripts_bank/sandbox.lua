
text_renderer=TextRendering()
text_renderer:configure(root_entity, "fps", 10, 40, 255, 0, 255, "??? fps")

rg:update_renderingqueues()


g:add_appruncb( "run",
function()  

  time_infos = { root_entity:read_timemanager() }

  local timescale = commons.print_timescale(time_infos[1])

  local output_infos = "[SANDBOX]    " ..renderer:descr() .." "..time_infos[3].. " fps "..time_infos[2].." timescale = "..timescale

  text_renderer:update(10, 30, 255, 0, 0, output_infos)


end)


g:signal_renderscenebegin("eg")


inspect_model = function(entity_name, model_file_path)

  local meshe_entity=Entity()

  meshe_entity:add_aspect(RESOURCES_ASPECT)
  meshe_entity:configure_mesheresource(model_file_path)

  
  eg:add_child('root', entity_name, meshe_entity)
  return meshe_entity
end

h = function()
  e = inspect_model('e','Trex.fbx')
end

h2 = function()
  local model_root_infos = { e:read_meshesfiledescription('Trex.fbx', 'root') }
  g:print( 'model file = '..model_root_infos[1]..' num_meshes = '..model_root_infos[3]..' num_animations = '..model_root_infos[5] )
end

h3 = function()
  local meshes_id_list = { e:read_meshesfiledescription('Trex.fbx', 'meshes_list') }
  for k, v in pairs(meshes_id_list) do
    g:print(k.. ' - ' ..v)
  end
end

h4 = function()
  local anims_id_list = { e:read_meshesfiledescription('Trex.fbx', 'anims_list') }
  for k, v in pairs(anims_id_list) do
    g:print(k.. ' - ' ..v)
  end
end

h5 = function(anim_index)
  local anim_infos = { e:read_meshesfiledescription('Trex.fbx', 'anim', anim_index) }
   g:print( 'name = '..anim_infos[1]..' ticks/sec = '..anim_infos[2]..' duration = '..anim_infos[3]..'s number of channels = '..anim_infos[4])
end

h6 = function(meshe_index)
  local meshe_infos = { e:read_meshesfiledescription('Trex.fbx', 'meshe', meshe_index) }
   g:print('node id = '..meshe_infos[1].. ' name = ['..meshe_infos[2]..']' )
   g:print('has positions = '..meshe_infos[3]..' has faces = '..meshe_infos[4]..' has normales = '..meshe_infos[5]..' has tbn = '..meshe_infos[6] )
   g:print('num vertices = '..meshe_infos[7]..' num faces = '..meshe_infos[8]..' num uvchannels = '..meshe_infos[9]..' num bones = '..meshe_infos[10])

end