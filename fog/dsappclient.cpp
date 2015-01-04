
#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Gui;

dsAppClient* dsAppClient::m_instance = NULL;

_DECLARE_DS_LOGGER( logger, "AppClient" )


dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false )
{    
    _INIT_LOGGER( "fog.conf" )  
    m_w_title = "fog test";
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    //m_scenegraph.ComputeTransformations( m_timer );  
    m_scenenodegraph.ComputeTransformations( m_timer );

    m_texturepass->GetRenderingQueue()->Draw();
    m_fogintpass->GetRenderingQueue()->Draw();
    
    m_fogblendpass->GetRenderingQueue()->Draw();

    m_finalpass->GetRenderingQueue()->Draw();


    renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );

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

    //m_scenegraph.RegisterPass( m_texturepass );
    //m_scenegraph.RegisterPass( m_fogintpass );

    m_scenenodegraph.RegisterPass( m_texturepass );
    m_scenenodegraph.RegisterPass( m_fogintpass );


    ///////////////////////////////////////////////////////////////



    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_chunk->RegisterPassSlot( "fogint_pass" );
    m_chunk->RegisterPassSlot( "texture_pass" );
    m_chunk->SetRenderer( renderer );
    m_chunk->SetSceneName( "chunk" );

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );
    
    m_chunk->GetMeshe()->SetImporter( m_meshe_import );
    m_chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_chunk->GetNodeFromPass( "fogint_pass" )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_chunk->GetNodeFromPass( "fogint_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.vsh", false ) ) );
    m_chunk->GetNodeFromPass( "fogint_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.psh", false ) ) );
    m_chunk->GetNodeFromPass( "fogint_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( "fogint_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();
    m_chunk->GetNodeFromPass( "fogint_pass" )->AddShaderParameter( 0, "fog_intensity", 12 );
    m_chunk->GetNodeFromPass( "fogint_pass" )->SetShaderReal( "fog_intensity", 0.08 );

    m_chunk->GetNodeFromPass( "fogint_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( "fogint_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    

    m_chunk->GetNodeFromPass( "texture_pass" )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_chunk->GetNodeFromPass( "texture_pass" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetTexture( 0 )->LoadFromFile();

    



    //m_scenegraph.RegisterNode( m_chunk );
    // temporaire
    //m_chunk->SetScenegraph( &m_scenegraph );

    m_chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "chunk" ) );
    m_chunk_node->SetContent( m_chunk );

    m_scenenodegraph.RegisterNode( m_chunk_node );


    


    ///////////////////////////////////////////////////////////////

    m_ground = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ground->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_ground->RegisterPassSlot( "fogint_pass" );
    m_ground->RegisterPassSlot( "texture_pass" );
    m_ground->SetRenderer( renderer );
    m_ground->SetSceneName( "ground" );
    
    m_ground->GetMeshe()->SetImporter( m_meshe_import );

    m_ground->GetMeshe()->LoadFromFile( "grid.ac", 0 );

    m_ground->GetNodeFromPass( "fogint_pass" )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( "fogint_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.vsh", false ) ) );
    m_ground->GetNodeFromPass( "fogint_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.psh", false ) ) );
    m_ground->GetNodeFromPass( "fogint_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( "fogint_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ground->GetNodeFromPass( "fogint_pass" )->AddShaderParameter( 0, "fog_intensity", 12 );
    m_ground->GetNodeFromPass( "fogint_pass" )->SetShaderReal( "fog_intensity", 0.08 );

    m_ground->GetNodeFromPass( "fogint_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( "fogint_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    m_ground->GetNodeFromPass( "texture_pass" )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ground->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ground->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ground->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );


    m_ground->GetNodeFromPass( "texture_pass" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "ground.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( "texture_pass" )->GetTexture( 0 )->LoadFromFile();

    //m_scenegraph.RegisterNode( m_ground );
    // temporaire
    //m_ground->SetScenegraph( &m_scenegraph );

    m_ground_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "ground" ) );
    m_ground_node->SetContent( m_ground );

    m_scenenodegraph.RegisterNode( m_ground_node );



    /////////////////////////////////////////////////////////////////

    status = DrawSpace::Utils::LoadFontImportPlugin( "cbfgfont.dll", "cbfgfont_plugin" );
    m_font_import = DrawSpace::Utils::InstanciateFontImportFromPlugin( "cbfgfont_plugin" );
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




    //m_scenegraph.RegisterNode( m_camera );
    //m_scenegraph.SetCurrentCamera( "camera" );

    m_scenenodegraph.SetCurrentCamera( "camera" );
    

    m_fpsmove.Init( DrawSpace::Utils::Vector( 0.0, 1.0, 10.0, 1.0 ) );
    m_fpsmove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FPSMovement>, SceneNode<DrawSpace::Core::FPSMovement>( "fps_node" ) );
    m_fpsmove_node->SetContent( &m_fpsmove );

    //m_camera->RegisterMovement( "fps", &m_fpsmove );

    m_scenenodegraph.AddNode( m_fpsmove_node );
    m_scenenodegraph.RegisterNode( m_fpsmove_node );

    
    m_camera_node->LinkTo( m_fpsmove_node );


    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_fogintpass->GetRenderingQueue()->UpdateOutputQueue();
    m_fogblendpass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();


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
    ground_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 100.0, 0.0, 100., 1.0 );
    ground_params.mass = 0.0;
    ground_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;


    ground_params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    m_ground_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, /*m_ground*/NULL, ground_params ) );
    m_ground_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "ground_body" ) );
    m_ground_body_node->SetContent( m_ground_body );

    m_scenenodegraph.AddNode( m_ground_body_node );
    m_scenenodegraph.RegisterNode( m_ground_body_node );

    m_ground_node->LinkTo( m_ground_body_node );



    m_mouse_circularmode = true;

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

            m_fpsmove.SetSpeed( 6.0 );
            break;

        case 'W':

            m_fpsmove.SetSpeed( -6.0 );
            break;

    }

}

void dsAppClient::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':

            m_fpsmove.SetSpeed( 0.0 );
            break;


        case 'W':

            m_fpsmove.SetSpeed( 0.0 );
            break;

    }

}

void dsAppClient::OnKeyPulse( long p_key )
{
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
	m_fpsmove.RotateYaw( - p_dx / 4.0, m_timer );

	m_fpsmove.RotatePitch( - p_dy / 4.0, m_timer );
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
