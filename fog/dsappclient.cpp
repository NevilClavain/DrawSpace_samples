#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Gui;

dsAppClient* dsAppClient::m_instance = NULL;



dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_draw_cube2( true )
{    
    _INIT_LOGGER( "logfog.conf" )  
    m_w_title = "fog test";
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{


    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_scenenodegraph.ComputeTransformations( m_timer );

    m_texturepass->GetRenderingQueue()->Draw();
    m_fogintpass->GetRenderingQueue()->Draw();
    
    m_fogblendpass->GetRenderingQueue()->Draw();

    m_finalpass->GetRenderingQueue()->Draw();


    renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );
    renderer->DrawText( 255, 0, 0, 10, 40, "%s", m_current_camera.c_str() );

    renderer->FlipScreen();

    m_timer.Update();
    if( m_timer.IsReady() )
    {
        m_world.StepSimulation( m_timer.GetFPS(), 15 );
    }
}

bool dsAppClient::OnIdleAppInit( void )
{
    bool status;

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    //m_texturepass->SetTargetDimsFromRenderer( false );
    //m_texturepass->SetTargetDims( 64, 64 );
    m_texturepass->Initialize();
    

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 145, 230, 230 );

    

    //////////////////////////////////////////////////////////////

    m_fogintpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "fogint_pass" ) );
    //m_fogintpass->SetTargetDimsFromRenderer( false );
    //m_fogintpass->SetTargetDims( 64, 64 );
    m_fogintpass->Initialize();

    m_fogintpass->GetRenderingQueue()->EnableDepthClearing( true );
    m_fogintpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_fogintpass->GetRenderingQueue()->SetTargetClearingColor( 255, 0, 0 );
    


    //////////////////////////////////////////////////////////////

    
    m_fogblendpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "fogblend_pass" ) );
    //m_fogblendpass->SetTargetDimsFromRenderer( false );
    //m_fogblendpass->SetTargetDims( 64, 64 );
    m_fogblendpass->Initialize();
    m_fogblendpass->CreateViewportQuad();
    

    m_fogblendpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_fogblendpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogblend.vsh", false ) ) );
    m_fogblendpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogblend.psh", false ) ) );
    m_fogblendpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_fogblendpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_fogblendpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    m_fogblendpass->GetViewportQuad()->SetTexture( m_fogintpass->GetTargetTexture(), 1 );

    m_fogblendpass->GetViewportQuad()->AddShaderParameter( 1, "fog_color", 0 );
    m_fogblendpass->GetViewportQuad()->SetShaderRealVector( "fog_color", DrawSpace::Utils::Vector( 145.0 / 255.0, 230.0 / 255.0, 230.0 / 255.0, 1.0 ) );
    
    

    //////////////////////////////////////////////////////////////

    
    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_fogblendpass->GetTargetTexture(), 0 );
        

    
    ///////////////////////////////////////////////////////////////



    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_chunk->RegisterPassSlot( m_fogintpass );
    m_chunk->RegisterPassSlot( m_texturepass );
    m_chunk->SetRenderer( renderer );
    m_chunk->SetSceneName( "chunk" );

    //status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe", "ac3dmeshe_plugin" );
    //m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );
    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    
    m_chunk->GetMeshe()->SetImporter( m_meshe_import );
    m_chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_chunk->GetNodeFromPass( m_fogintpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_chunk->GetNodeFromPass( m_fogintpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.vsh", false ) ) );
    m_chunk->GetNodeFromPass( m_fogintpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.psh", false ) ) );
    m_chunk->GetNodeFromPass( m_fogintpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( m_fogintpass )->GetFx()->GetShader( 1 )->LoadFromFile();
    m_chunk->GetNodeFromPass( m_fogintpass )->AddShaderParameter( 0, "fog_intensity", 12 );
    m_chunk->GetNodeFromPass( m_fogintpass )->SetShaderReal( "fog_intensity", 0.08 );

    m_chunk->GetNodeFromPass( m_fogintpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( m_fogintpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    

    m_chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    
    m_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "chunk" ) );
    m_chunk_node->SetContent( m_chunk );

    m_scenenodegraph.RegisterNode( m_chunk_node );


    


    ///////////////////////////////////////////////////////////////

    m_ground = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ground->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_ground->RegisterPassSlot( m_fogintpass );
    m_ground->RegisterPassSlot( m_texturepass );
    m_ground->SetRenderer( renderer );
    m_ground->SetSceneName( "ground" );
    
    m_ground->GetMeshe()->SetImporter( m_meshe_import );

    m_ground->GetMeshe()->LoadFromFile( "grid.ac", 0 );

    m_ground->GetNodeFromPass( m_fogintpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_fogintpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_fogintpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.psh", false ) ) );
    m_ground->GetNodeFromPass( m_fogintpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_fogintpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ground->GetNodeFromPass( m_fogintpass )->AddShaderParameter( 0, "fog_intensity", 12 );
    m_ground->GetNodeFromPass( m_fogintpass )->SetShaderReal( "fog_intensity", 0.08 );

    m_ground->GetNodeFromPass( m_fogintpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_fogintpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    m_ground->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );


    m_ground->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "ground.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_ground_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "ground" ) );
    m_ground_node->SetContent( m_ground );

    m_scenenodegraph.RegisterNode( m_ground_node );



    /////////////////////////////////////////////////////////////////

    //status = DrawSpace::Utils::LoadFontImportPlugin( "cbfgfont", "cbfgfont_plugin" );
    //m_font_import = DrawSpace::Utils::InstanciateFontImportFromPlugin( "cbfgfont_plugin" );

    m_font_import = new CBFGFontImport();
    m_font = _DRAWSPACE_NEW_( Font, Font );
    m_font->SetImporter( m_font_import );

    status = m_font->Build( "mangalfont.bmp", "mangalfont.csv" );
    if( !status )
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////

    m_mytext = _DRAWSPACE_NEW_( DrawSpace::Text, DrawSpace::Text( m_font ) );
    status = m_mytext->Initialize();

    m_mytext->SetText( 3, 16, 8, "Fog test" );

    m_mytext->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_mytext->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_mytext->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_mytext->GetFx()->GetShader( 0 )->LoadFromFile();
    m_mytext->GetFx()->GetShader( 1 )->LoadFromFile();

    m_mytext->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_mytext->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_mytext->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_mytext->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_mytext->SetOrderNumber( 20000 );


    m_finalpass->GetRenderingQueue()->Add( m_mytext );


    ///////////////////////////////////////////////////////////////


    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera", NULL, "" ) );
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );

    m_scenenodegraph.RegisterNode( m_camera_node );




    m_scenenodegraph.SetCurrentCamera( "camera" );
    

    m_fpsmove.Init( DrawSpace::Utils::Vector( 0.0, 1.0, 10.0, 1.0 ) );
    m_fpsmove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FPSMovement>, SceneNode<DrawSpace::Core::FPSMovement>( "fps_node" ) );
    m_fpsmove_node->SetContent( &m_fpsmove );


    m_scenenodegraph.AddNode( m_fpsmove_node );
    m_scenenodegraph.RegisterNode( m_fpsmove_node );

    
    m_camera_node->LinkTo( m_fpsmove_node );


    ////////// 2nd camera //////////////

    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera2", NULL, "" ) );
    m_camera2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera2" ) );
    m_camera2_node->SetContent( m_camera2 );

    

    //m_scenenodegraph.AddNode( m_camera2_node );
    m_scenenodegraph.RegisterNode( m_camera2_node );


  

    m_circmove.Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( -10.0, 1.0, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 0.0, 0.0, 0.0 );
    m_circmove.SetAngularSpeed( 40.0 );
    m_circmove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::CircularMovement>, SceneNode<DrawSpace::Core::CircularMovement>( "circ_node" ) );
    m_circmove_node->SetContent( &m_circmove );


    m_scenenodegraph.AddNode( m_circmove_node );
    m_scenenodegraph.RegisterNode( m_circmove_node );


    m_camera2_node->LinkTo( m_circmove_node );


    ////////// 3rd camera //////////////

    m_camera3 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera3", NULL, "" ) );

    Matrix cam3_pos;
    cam3_pos.Translation( 0.0, 0.7, 6.0 );

    m_camera3->SetLocalTransform( cam3_pos );

    m_camera3_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera3" ) );
    m_camera3_node->SetContent( m_camera3 );

    
    m_scenenodegraph.RegisterNode( m_camera3_node );


 
    m_camera3_node->LinkTo( m_circmove_node );


    ////////////////////////////////////


    m_world.Initialize();
    m_world.SetGravity( DrawSpace::Utils::Vector( 0.0, -9.81, 0.0, 0.0 ) );

    DrawSpace::Dynamics::Body::Parameters cube_params;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;

    cube_params.initial_attitude.Translation( 0.0, 10.5, 0.0 );


    m_cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, /*m_chunk*/ NULL, cube_params ) );

    m_cube_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "cube_body" ) );
    m_cube_body_node->SetContent( m_cube_body );

    m_scenenodegraph.AddNode( m_cube_body_node );
    m_scenenodegraph.RegisterNode( m_cube_body_node );

    m_chunk_node->LinkTo( m_cube_body_node );



    DrawSpace::Dynamics::Body::Parameters ground_params;
    ground_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 100.0, 0.0, 100.0, 1.0 );
    ground_params.mass = 0.0;
    ground_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;


    ground_params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    m_ground_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, /*m_ground*/NULL, ground_params ) );
    m_ground_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "ground_body" ) );
    m_ground_body_node->SetContent( m_ground_body );

    m_scenenodegraph.AddNode( m_ground_body_node );
    m_scenenodegraph.RegisterNode( m_ground_body_node );

    m_ground_node->LinkTo( m_ground_body_node );



    m_camera2->LockOnBody( "cube_body", m_cube_body );


    ////////////////////////////////////////////////////////////////////////////////////////////

    m_cube2 = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_cube2->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_cube2->RegisterPassSlot( m_fogintpass );
    m_cube2->RegisterPassSlot( m_texturepass );
    m_cube2->SetRenderer( renderer );
    m_cube2->SetSceneName( "cube2" );

        
    m_cube2->GetMeshe()->SetImporter( m_meshe_import );
    m_cube2->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_cube2->GetNodeFromPass( m_fogintpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_cube2->GetNodeFromPass( m_fogintpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.vsh", false ) ) );
    m_cube2->GetNodeFromPass( m_fogintpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.psh", false ) ) );
    m_cube2->GetNodeFromPass( m_fogintpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_cube2->GetNodeFromPass( m_fogintpass )->GetFx()->GetShader( 1 )->LoadFromFile();
    m_cube2->GetNodeFromPass( m_fogintpass )->AddShaderParameter( 0, "fog_intensity", 12 );
    m_cube2->GetNodeFromPass( m_fogintpass )->SetShaderReal( "fog_intensity", 0.08 );

    m_cube2->GetNodeFromPass( m_fogintpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_cube2->GetNodeFromPass( m_fogintpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    

    m_cube2->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_cube2->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_cube2->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "saturnmap.jpg" ) ), 0 );
    m_cube2->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    
    m_cube2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "cube2" ) );
    m_cube2_node->SetContent( m_cube2 );

    m_scenenodegraph.RegisterNode( m_cube2_node );




    DrawSpace::Dynamics::Body::Parameters cube2_params;
    cube2_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube2_params.mass = 0.0;
    cube2_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
   

    m_cube2_collider = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Collider, DrawSpace::Dynamics::Collider( NULL ) );

    m_cube2_collider->SetKinematic( cube2_params );
    m_cube2_collider->AddToWorld( &m_world );


    m_cube2_colider_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Collider>, SceneNode<DrawSpace::Dynamics::Collider>( "cube2_body" ) );
    m_cube2_colider_node->SetContent( m_cube2_collider );
    
    m_scenenodegraph.RegisterNode( m_cube2_colider_node );

    m_cube2_node->LinkTo( m_cube2_colider_node );



    m_freemove.Init( Vector( 0.8, 2.0, 0.3, 1.0 ) );
    m_freemove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FreeMovement>, SceneNode<DrawSpace::Core::FreeMovement>( "freemvt_node" ) );
    m_freemove_node->SetContent( &m_freemove );


    m_scenenodegraph.AddNode( m_freemove_node );
    m_scenenodegraph.RegisterNode( m_freemove_node );

    
    m_cube2_colider_node->LinkTo( m_freemove_node );



    m_camera3_node->LinkTo( m_cube2_node );




    /////////////////////////////////////////////////////////////////////////////////////////////


    m_current_camera = "camera2";
    m_scenenodegraph.SetCurrentCamera( m_current_camera );

    m_mouse_circularmode = true;



    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_fogintpass->GetRenderingQueue()->UpdateOutputQueue();
    m_fogblendpass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();


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

            if( "camera" == m_current_camera )
            {
                m_fpsmove.SetSpeed( 6.0 );
            }
            else if( "camera3" == m_current_camera )
            {
                m_freemove.SetSpeed( 6.0 );
            }
            break;

        case 'W':

            if( "camera" == m_current_camera )
            {
                m_fpsmove.SetSpeed( -6.0 );
            }
            else
            {
                m_freemove.SetSpeed( -6.0 );
            }
            break;

    }

}

void dsAppClient::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':
        case 'W':

            if( "camera" == m_current_camera )
            {
                m_fpsmove.SetSpeed( 0.0 );
            }
            else if( "camera3" == m_current_camera )
            {
                m_freemove.SetSpeed( 0.0 );
            }
            break;      
    }

}

void dsAppClient::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:

            if( "camera" == m_current_camera )
            {
                m_current_camera = "camera2";
            }
            else if( "camera2" == m_current_camera )
            {
                m_current_camera = "camera3";
            }
            else if( "camera3" == m_current_camera )
            {
                m_current_camera = "camera";
            }

            m_scenenodegraph.SetCurrentCamera( m_current_camera );

            break;

        case VK_F2:

            if( m_draw_cube2 )
            {
                m_draw_cube2 = false;


            }
            else
            {
                m_draw_cube2 = true;
            }

            m_cube2->SetDrawingState( m_texturepass, m_draw_cube2 );
            break;
    }
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( m_mouserb )
    {
        if( "camera" == m_current_camera )
        {
        }
        else if( "camera3" == m_current_camera )
        {
            m_freemove.RotateRoll( - p_dx / 4.0, m_timer );            
        }
    }

    else
    {
        if( "camera" == m_current_camera )
        {
	        m_fpsmove.RotateYaw( - p_dx / 4.0, m_timer );
	        m_fpsmove.RotatePitch( - p_dy / 4.0, m_timer );
        }
        else if( "camera3" == m_current_camera )
        {
            m_freemove.RotateYaw( - p_dx / 4.0, m_timer );
            m_freemove.RotatePitch( - p_dy / 4.0, m_timer );
        }
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
