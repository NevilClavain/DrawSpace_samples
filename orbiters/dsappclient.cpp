#include "dsappclient.h"



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_speed( 0.0 ), m_speed_speed( 5.0 )
{    
    _INIT_LOGGER( "logorbiters.conf" )  
    m_w_title = "orbiters test";

    m_nodesevent_cb = _DRAWSPACE_NEW_( NodesEventCallback, NodesEventCallback( this, &dsAppClient::on_nodes_event ) );


}

dsAppClient::~dsAppClient( void )
{

}

Chunk* dsAppClient::build_planet_chunk( char* p_name, char* p_texture )
{
    Chunk* chunk;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    chunk->RegisterPassSlot( m_texturepass );
    chunk->SetRenderer( renderer );
    chunk->SetSceneName( p_name );

    chunk->GetMeshe()->SetImporter( m_meshe_import );

    chunk->GetMeshe()->LoadFromFile( "planet.ac", 0 );


    chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( p_texture ) ), 0 );

    chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    return chunk;
}

Chunk* dsAppClient::build_orbit_drawable( char* p_name, Orbit* p_orbit )
{
    Chunk* chunk;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    chunk->RegisterPassSlot( m_texturepass );
    chunk->SetRenderer( renderer );
    chunk->SetSceneName( p_name );

    Meshe* orb0_meshe = chunk->GetMeshe();

    p_orbit->BuildMeshe( 10.0, orb0_meshe );

    chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "line.vsh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "line.psh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "none" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    chunk->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "color", 0 );
    chunk->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "color", Vector( 1.0, 0.0, 0.0, 1.0 ) );

    chunk->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "thickness", 1 );
    chunk->GetNodeFromPass( m_texturepass )->SetShaderReal( "thickness", 0.005 );
    

    return chunk;
}

void dsAppClient::on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node )
{
    _asm nop
}

void dsAppClient::OnRenderFrame( void )
{
    Matrix cam2_pos;
    cam2_pos.Translation( 0.0, 1.0, 0.0 );

    

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    
    DrawSpace::Utils::Matrix sbtrans;
    sbtrans.Scale( 20.0, 20.0, 20.0 );
    
    m_spacebox->SetLocalTransform( sbtrans );
    


    //m_cube_body->Update( m_timer );
    //m_cube_body_2->Update( m_timer );


    
    
    Matrix origin;
    origin.Identity();

    m_scenenodegraph.ComputeTransformations( m_timer );



    static long last_fps;

    long current_fps = m_timer.GetFPS();
    char fps[256];
    sprintf( fps, "%d fps", m_timer.GetFPS() );
    if( last_fps != current_fps )
    {
        m_fpstext_widget->SetText( 0, 0, 70, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
        last_fps = current_fps;		
    }

    dsstring date;
    m_calendar->GetFormatedDate( date );


    m_fpstext_widget->SetVirtualTranslation( 10, 5 );
    m_fpstext_widget->Transform();


    m_texturepass->GetRenderingQueue()->Draw();

    m_fpstext_widget->Draw();

    m_finalpass->GetRenderingQueue()->Draw();


    renderer->DrawText( 0, 255, 0, 10, 55, "%s", date.c_str() );

    renderer->DrawText( 0, 255, 0, 10, 75, "%d", m_calendar->GetCurrentInstant() );

    renderer->DrawText( 0, 255, 0, 10, 95, "%d", m_calendar->GetSubSecCount() );


    renderer->FlipScreen();



    m_calendar->Run();

    m_freemove.SetSpeed( m_speed );
}

bool dsAppClient::OnIdleAppInit( void )
{

    /////////////////////////////////////
    

    bool status;
    
    Chunk* chunk;

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );
    m_texturepass->Initialize();

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0 );


    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    

    //////////////////////////////////////////////////////////////


    

    m_spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
    m_spacebox->RegisterPassSlot( m_texturepass );

    m_spacebox->SetSceneName( "spacebox" );


    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, m_texturepass );

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_front5.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_back6.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_top3.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_bottom4.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_left2.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_right1.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetOrderNumber( 200 );


    

    m_spacebox_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Spacebox>, SceneNode<DrawSpace::Spacebox>( "spacebox" ) );
    m_spacebox_node->SetContent( m_spacebox );

    m_scenenodegraph.AddNode( m_spacebox_node );
    m_scenenodegraph.RegisterNode( m_spacebox_node );

    

    //////////////////////////////////////////////////////////////


    m_world.Initialize();
    
 

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe", "ac3dmeshe_plugin" );
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );





    //////////////////////////////////////////////

    m_sun_chunk = build_planet_chunk( "sun_chunk", "texture_sun.jpg" );
    m_sun_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "sun_chunk" ) );
    m_sun_chunk_node->SetContent( m_sun_chunk );


    m_scenenodegraph.AddNode( m_sun_chunk_node );
    m_scenenodegraph.RegisterNode( m_sun_chunk_node );



    //////////////////////////////////////////////


    m_mars_chunk = build_planet_chunk( "mars_chunk", "mars.jpg" );
    m_mars_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "mars_chunk" ) );
    m_mars_chunk_node->SetContent( m_mars_chunk );

    m_scenenodegraph.RegisterNode( m_mars_chunk_node );


    m_mars_orbiter = _DRAWSPACE_NEW_( Orbiter, Orbiter( &m_world, NULL ) );
    m_mars_orbiter->SetOrbitDuration( 1.0 );
    m_mars_orbiter->SetRevolutionTiltAngle( 33.0 );
    m_mars_orbiter->SetRevolutionDuration( 1.0 );


    DrawSpace::Dynamics::Body::Parameters sphere_params;
    sphere_params.shape_descr.sphere_radius = 0.5;
    sphere_params.shape_descr.shape = DrawSpace::Dynamics::Body::SPHERE_SHAPE;

    sphere_params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    m_mars_orbiter->SetKinematic( sphere_params );
    m_mars_orbiter->AddToWorld();



    m_mars_orbiter_node = _DRAWSPACE_NEW_( SceneNode<Orbiter>, SceneNode<Orbiter>( "mars_orbiter" ) );
    m_mars_orbiter_node->SetContent( m_mars_orbiter );

    m_scenenodegraph.RegisterNode( m_mars_orbiter_node );

    m_mars_chunk_node->LinkTo( m_mars_orbiter_node );


    //////////////////////////////////////////////


    m_mars_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 16.0, 0.99, 0.0, 9.0, 0.0, 0.0, 1.0, 33.0, 1.0, NULL ) );
    m_mars_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "mars_orbit" ) );
    m_mars_orbit_node->SetContent( m_mars_orbit );

    
    m_scenenodegraph.AddNode( m_mars_orbit_node );
    m_scenenodegraph.RegisterNode( m_mars_orbit_node );

    m_mars_orbiter_node->LinkTo( m_mars_orbit_node );


    ////////////////////////////////////////////////


    m_mars_orbit_chunk = build_orbit_drawable( "mars_orbit_chunk", m_mars_orbit );
    m_mars_orbit_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "mars_orbit_chunk" ) );
    m_mars_orbit_chunk_node->SetContent( m_mars_orbit_chunk );

    
    m_scenenodegraph.AddNode( m_mars_orbit_chunk_node );
    m_scenenodegraph.RegisterNode( m_mars_orbit_chunk_node );


    ////////////////////////////////////////////




    m_moon_chunk = build_planet_chunk( "moon_chunk", "mars.jpg" );
    m_moon_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "moon_chunk" ) );
    m_moon_chunk_node->SetContent( m_moon_chunk );

    m_scenenodegraph.RegisterNode( m_moon_chunk_node );



    m_moon_orbiter = _DRAWSPACE_NEW_( Orbiter, Orbiter( &m_world, NULL ) );
    m_moon_orbiter->SetOrbitDuration( 0.5 );
    m_moon_orbiter->SetRevolutionTiltAngle( 0.0 );
    m_moon_orbiter->SetRevolutionDuration( 0.3 );


    sphere_params.shape_descr.sphere_radius = 0.5;
    sphere_params.shape_descr.shape = DrawSpace::Dynamics::Body::SPHERE_SHAPE;
    sphere_params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    m_moon_orbiter->SetKinematic( sphere_params );
    m_moon_orbiter->AddToWorld();



    m_moon_orbiter_node = _DRAWSPACE_NEW_( SceneNode<Orbiter>, SceneNode<Orbiter>( "moon_orbiter" ) );
    m_moon_orbiter_node->SetContent( m_moon_orbiter );

    m_scenenodegraph.RegisterNode( m_moon_orbiter_node );

    m_moon_chunk_node->LinkTo( m_moon_orbiter_node );


    //////////////////////////////////////////////


    m_moon_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 5.0, 0.45, 0.0, 10.0, 3.0, 0.0, 0.5, 0.0, 0.3, NULL ) );
    m_moon_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "moon_orbit" ) );
    m_moon_orbit_node->SetContent( m_moon_orbit );

    
    m_scenenodegraph.RegisterNode( m_moon_orbit_node );

    m_moon_orbiter_node->LinkTo( m_moon_orbit_node );


    m_moon_orbit_node->LinkTo( m_mars_orbit_node );


    ///////////////////////////////////////////////

    m_moon_orbit_chunk = build_orbit_drawable( "moon_orbit_chunk", m_moon_orbit );
    m_moon_orbit_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "moon_orbit_chunk" ) );
    m_moon_orbit_chunk_node->SetContent( m_moon_orbit_chunk );

    
    
    m_scenenodegraph.RegisterNode( m_moon_orbit_chunk_node );


    m_moon_orbit_chunk_node->LinkTo( m_mars_orbit_node );


    //////////////////////////////////////////////



    m_saturn_root_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 60.0, 0.9999, 8.0, 1.0, 0.0, 0.0, 20.0, 0.0, 1.0, NULL ) );
    m_saturn_root_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "saturn_root_orbit" ) );
    m_saturn_root_orbit_node->SetContent( m_saturn_root_orbit );

    
    m_scenenodegraph.AddNode( m_saturn_root_orbit_node );
    m_scenenodegraph.RegisterNode( m_saturn_root_orbit_node );


    //////////////////////////////////////////////


    m_saturn_root_orbit_chunk = build_orbit_drawable( "saturn_root_orbit_chunk", m_saturn_root_orbit );
    m_saturn_root_orbit_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "saturn_root_orbit_chunk" ) );
    m_saturn_root_orbit_chunk_node->SetContent( m_saturn_root_orbit_chunk );

    
    m_scenenodegraph.AddNode( m_saturn_root_orbit_chunk_node );
    m_scenenodegraph.RegisterNode( m_saturn_root_orbit_chunk_node );


    ///////////////////////////////////////////////


    m_saturn1_chunk = build_planet_chunk( "saturn1_chunk", "saturnmap.jpg" );
    m_saturn1_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "saturn1_chunk" ) );
    m_saturn1_chunk_node->SetContent( m_saturn1_chunk );

    m_scenenodegraph.RegisterNode( m_saturn1_chunk_node );


    m_saturn1_orbiter = _DRAWSPACE_NEW_( Orbiter, Orbiter( &m_world, NULL ) );
    m_saturn1_orbiter->SetOrbitDuration( 0.45 );
    m_saturn1_orbiter->SetRevolutionTiltAngle( 0.0 );
    m_saturn1_orbiter->SetRevolutionDuration( 0.0 );



    sphere_params.shape_descr.sphere_radius = 0.5;
    sphere_params.shape_descr.shape = DrawSpace::Dynamics::Body::SPHERE_SHAPE;

    sphere_params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    m_saturn1_orbiter->SetKinematic( sphere_params );
    m_saturn1_orbiter->AddToWorld();



    m_saturn1_orbiter_node = _DRAWSPACE_NEW_( SceneNode<Orbiter>, SceneNode<Orbiter>( "saturn1_orbiter" ) );
    m_saturn1_orbiter_node->SetContent( m_saturn1_orbiter );

    m_scenenodegraph.RegisterNode( m_saturn1_orbiter_node );

    m_saturn1_chunk_node->LinkTo( m_saturn1_orbiter_node );


    //////////////////////////////////////////////////


    m_saturn1_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 9.6, 0.7, 8.0, 1.0, 0.0, 0.0, 0.45, 0.0, 0.0, NULL ) );
    m_saturn1_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "saturn1_orbit" ) );
    m_saturn1_orbit_node->SetContent( m_saturn1_orbit );

    
    m_scenenodegraph.RegisterNode( m_saturn1_orbit_node );


    m_saturn1_orbiter_node->LinkTo( m_saturn1_orbit_node );

    m_saturn1_orbit_node->LinkTo( m_saturn_root_orbit_node );

    


    ///////////////////////////////////////////////////


    m_saturn1_orbit_chunk = build_orbit_drawable( "saturn1_orbit_chunk", m_saturn1_orbit );
    m_saturn1_orbit_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "saturn1_orbit_chunk" ) );
    m_saturn1_orbit_chunk_node->SetContent( m_saturn1_orbit_chunk );

    
    m_scenenodegraph.RegisterNode( m_saturn1_orbit_chunk_node );


    m_saturn1_orbit_chunk_node->LinkTo( m_saturn_root_orbit_node );


    ////////////////////////////////////////////////////


    m_saturn2_chunk = build_planet_chunk( "saturn2_chunk", "saturnmap.jpg" );
    m_saturn2_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "saturn2_chunk" ) );
    m_saturn2_chunk_node->SetContent( m_saturn2_chunk );

    m_scenenodegraph.RegisterNode( m_saturn2_chunk_node );


    m_saturn2_orbiter = _DRAWSPACE_NEW_( Orbiter, Orbiter( &m_world, NULL ) );
    m_saturn2_orbiter->SetOrbitDuration( 0.45 );
    m_saturn2_orbiter->SetRevolutionTiltAngle( 0.0 );
    m_saturn2_orbiter->SetRevolutionDuration( 0.0 );



    sphere_params.shape_descr.sphere_radius = 0.5;
    sphere_params.shape_descr.shape = DrawSpace::Dynamics::Body::SPHERE_SHAPE;

    sphere_params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    m_saturn2_orbiter->SetKinematic( sphere_params );
    m_saturn2_orbiter->AddToWorld();



    m_saturn2_orbiter_node = _DRAWSPACE_NEW_( SceneNode<Orbiter>, SceneNode<Orbiter>( "saturn2_orbiter" ) );
    m_saturn2_orbiter_node->SetContent( m_saturn1_orbiter );

    m_scenenodegraph.RegisterNode( m_saturn2_orbiter_node );

    m_saturn2_chunk_node->LinkTo( m_saturn2_orbiter_node );

    //////////////////////////////////////////////////


    m_saturn2_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 9.6, 0.7, 188.0, 1.0, 0.0, 0.0, 0.45, 0.0, 0.0, NULL ) );
    m_saturn2_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "saturn2_orbit" ) );
    m_saturn2_orbit_node->SetContent( m_saturn2_orbit );

    
    m_scenenodegraph.RegisterNode( m_saturn2_orbit_node );


    m_saturn2_orbiter_node->LinkTo( m_saturn2_orbit_node );

    m_saturn2_orbit_node->LinkTo( m_saturn_root_orbit_node );

    


    ///////////////////////////////////////////////////


    m_saturn2_orbit_chunk = build_orbit_drawable( "saturn2_orbit_chunk", m_saturn2_orbit );
    m_saturn2_orbit_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "saturn2_orbit_chunk" ) );
    m_saturn2_orbit_chunk_node->SetContent( m_saturn1_orbit_chunk );

    
    m_scenenodegraph.RegisterNode( m_saturn2_orbit_chunk_node );


    m_saturn2_orbit_chunk_node->LinkTo( m_saturn_root_orbit_node );


    ////////////////////////////////////////////////////


    m_scenenodegraph.RegisterNodesEvtHandler( m_nodesevent_cb );
  

    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );
    m_calendar->RegisterWorld( &m_world );

    m_calendar->RegisterOrbit( m_mars_orbit );
    m_calendar->RegisterOrbiter( m_mars_orbiter );

    m_calendar->RegisterOrbit( m_moon_orbit );
    m_calendar->RegisterOrbiter( m_moon_orbiter );


    m_calendar->RegisterOrbit( m_saturn_root_orbit );

    m_calendar->RegisterOrbit( m_saturn1_orbit );
    m_calendar->RegisterOrbiter( m_saturn1_orbiter );

    m_calendar->RegisterOrbit( m_saturn2_orbit );
    m_calendar->RegisterOrbiter( m_saturn2_orbiter );

    //////////////////////////////////////////////////////////////


    chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    chunk->RegisterPassSlot( m_texturepass );
    chunk->SetRenderer( renderer );

    chunk->SetSceneName( "box" );
    
    chunk->GetMeshe()->SetImporter( m_meshe_import );

    chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );    

    chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();
   
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );



    chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    
    m_cube = chunk;

    m_cube_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "box" ) );
    m_cube_node->SetContent( m_cube );

    m_scenenodegraph.RegisterNode( m_cube_node );




    DrawSpace::Dynamics::Body::Parameters cube_params;
    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );



    cube_params.initial_attitude.Translation( 50.0, 0.0, -10.0 );

    m_cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, chunk, cube_params ) );


    m_cube_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "cube_body" ) );
    m_cube_body_node->SetContent( m_cube_body );

    m_scenenodegraph.AddNode( m_cube_body_node );
    m_scenenodegraph.RegisterNode( m_cube_body_node );

    m_cube_node->LinkTo( m_cube_body_node );








    chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    chunk->RegisterPassSlot( m_texturepass );
    chunk->SetRenderer( renderer );

    chunk->SetSceneName( "box2" );
    
    chunk->GetMeshe()->SetImporter( m_meshe_import );

    chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );    

    chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );



    chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    

    m_cube2 = chunk;

    m_cube2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "box2" ) );
    m_cube2_node->SetContent( m_cube2 );

    m_scenenodegraph.RegisterNode( m_cube2_node );



    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );


    cube_params.initial_attitude.Translation( 5.0, 0.0, -100.0 );

    m_cube_body_2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, chunk, cube_params ) );


    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>* m_cube_body_2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "cube_body_2" ) );
    m_cube_body_2_node->SetContent( m_cube_body_2 );

    m_scenenodegraph.AddNode( m_cube_body_2_node );
    m_scenenodegraph.RegisterNode( m_cube_body_2_node );

    m_cube2_node->LinkTo( m_cube_body_2_node );





    //////////////////////////////////////////////////////////////


    status = DrawSpace::Utils::LoadFontImportPlugin( "cbfgfont", "cbfgfont_plugin" );
    m_font_import = DrawSpace::Utils::InstanciateFontImportFromPlugin( "cbfgfont_plugin" );
    m_font = _DRAWSPACE_NEW_( Font, Font );
    m_font->SetImporter( m_font_import );

    status = m_font->Build( "mangalfont.bmp", "mangalfont.csv" );
    if( !status )
    {
        return false;
    }

    m_fpstext_widget = DrawSpace::Utils::BuildText( m_font, 15, 10, DrawSpace::Utils::Vector( 1.0, 1.0, 1.0, 0.0 ), "fps" );

    m_fpstext_widget->GetImage()->SetOrderNumber( 20000 );
    m_fpstext_widget->RegisterToPass( m_finalpass );


    //////////////////////////////////////////////////////////////

    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera", NULL, "" ) );

    
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );
    m_scenenodegraph.RegisterNode( m_camera_node );


    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 0.0, 20.0, 1.0 ) );

    m_freemove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FreeMovement>, SceneNode<DrawSpace::Core::FreeMovement>( "free_node" ) );
    m_freemove_node->SetContent( &m_freemove );

    m_scenenodegraph.AddNode( m_freemove_node );
    m_scenenodegraph.RegisterNode( m_freemove_node );
    
    m_camera_node->LinkTo( m_freemove_node );






    m_scenenodegraph.SetCurrentCamera( "camera" );

    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    


    m_mouse_circularmode = true;



    //m_calendar->Startup( 162682566 );
    m_calendar->Startup( 0 );

        
    return true;
}

void dsAppClient::OnAppInit( void )
{

}

void dsAppClient::OnClose( void )
{
}

void dsAppClient::OnKeyPress( long p_key ) 
{
    switch( p_key )
    {
        case 'Q':

            /*
            if( 0.0 == m_speed )
            {
                m_speed = 1.0;
            }
            else
            {
                m_speed *= 1.081;
            }
            */

            m_timer.TranslationSpeedInc( &m_speed, m_speed_speed );
            m_speed_speed *= 1.03;
          
            break;

        case 'W':

            /*
            if( m_speed < 1.0 )
            {
                m_speed = 0.0;
            }
            */

            m_timer.TranslationSpeedDec( &m_speed, m_speed_speed );
            m_speed_speed *= 1.06;
 
            break;

    }
}

void dsAppClient::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':

            m_speed_speed = 5.0;
            break;


        case 'W':

            m_speed_speed = 5.0;                        
            break;

        case VK_SPACE:

            m_speed = 0.0;
            break;
    }
}

void dsAppClient::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            break;

        case VK_F2:

            m_calendar->SetTimeFactor( Calendar::SEC_1HOUR_TIME );
            break;

        case VK_F3:

            m_calendar->SetTimeFactor( Calendar::SEC_30DAYS_TIME );
            break;

        case VK_F4:

            m_calendar->SetTimeFactor( Calendar::DIV2_TIME );
            break;

        case VK_F5:

            //m_cube_body->Attach( m_mars );
            break;

        case VK_F6:

            //m_cube_body->Detach();
            break;
    }
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( !m_mouserb )
    {  
	    m_freemove.RotateYaw( - p_dx / 4.0, m_timer );
	    m_freemove.RotatePitch( - p_dy / 4.0, m_timer );
    }
    else
    {
        m_freemove.RotateRoll( -p_dx / 4.0, m_timer );
    }

}

void dsAppClient::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_mouselb = true;
}

void dsAppClient::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_mouselb = false;
}

void dsAppClient::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_mouserb = true;
}

void dsAppClient::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_mouserb = false;
}

void dsAppClient::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{

}
