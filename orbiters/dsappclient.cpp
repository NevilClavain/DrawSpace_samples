
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
    _INIT_LOGGER( "orbiters.conf" )  
    m_w_title = "orbiters test";

    for( long i = 0; i < 12; i++ )
    {
        m_orbiters_revol_angle[i] = 0.0;
    }
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{

    //m_fpsmove.Compute( m_timer, true );
    m_freemove.Compute( m_timer );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    DrawSpace::Utils::Matrix sbtrans;

    sbtrans.Scale( 20.0, 20.0, 20.0 );
    m_scenegraph.SetNodeLocalTransformation( "spacebox", sbtrans );



    m_orbiters[0]->Update( m_orbiters_revol_angle[0], Vector( 0.0, 0.0, 0.0, 1.0 ) );

     
    m_scenegraph.ComputeTransformations();


    DrawSpace::Utils::Matrix camera_pos;
    m_scenegraph.GetCurrentCameraTranform( camera_pos );


    static long last_fps;

    long current_fps = m_timer.GetFPS();
    char fps[256];
    sprintf( fps, "%d fps", m_timer.GetFPS() );
    if( last_fps != current_fps )
    {
        m_fpstext_widget->SetText( 0, 0, 70, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
        last_fps = current_fps;		
    }

    m_fpstext_widget->SetVirtualTranslation( 10, 5 );
    m_fpstext_widget->Transform();


    m_wireframepass->GetRenderingQueue()->Draw();

    m_fpstext_widget->Draw();

    m_finalpass->GetRenderingQueue()->Draw();

    renderer->FlipScreen();

    m_timer.Update();
    if( m_timer.IsReady() )
    {
        m_world.StepSimulation( m_timer.GetFPS() ); 


        m_timer.AngleSpeedInc( &m_orbiters_revol_angle[0], 70.0 );
    }

    m_freemove.SetSpeed( m_speed );
}

bool dsAppClient::OnIdleAppInit( void )
{
    bool status;

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_wireframepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "wireframe_pass" ) );

    m_wireframepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_wireframepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_wireframepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0 );


    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_wireframepass->GetTargetTexture(), 0 );
    

    //////////////////////////////////////////////////////////////

    m_scenegraph.RegisterPass( m_wireframepass );


    //////////////////////////////////////////////////////////////


    status = DrawSpace::Utils::LoadDrawablePlugin( "spacebox.dll", "spacebox_plugin" );

    m_spacebox = DrawSpace::Utils::InstanciateDrawableFromPlugin( "spacebox_plugin" );
    m_spacebox->RegisterPassSlot( "wireframe_pass" );
    m_spacebox->SetRenderer( renderer );
    m_spacebox->SetName( "spacebox" );


    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "wireframe_pass", "front" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "wireframe_pass", "rear" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "wireframe_pass", "top" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "wireframe_pass", "bottom" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "wireframe_pass", "left" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "wireframe_pass", "right" );

    m_spacebox->GetNodeFromPass( "wireframe_pass", "front" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_front5.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "front" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "wireframe_pass", "rear" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_back6.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "rear" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "wireframe_pass", "top" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_top3.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "top" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "wireframe_pass", "bottom" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_bottom4.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "bottom" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "wireframe_pass", "left" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_left2.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "left" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "wireframe_pass", "right" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_right1.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "right" )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "wireframe_pass", "front" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "rear" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "top" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "bottom" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "left" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", "right" )->SetOrderNumber( 200 );


    m_scenegraph.RegisterNode( m_spacebox );

    //////////////////////////////////////////////////////////////


    m_world.Initialize();
 



    //////////////////////////////////////////////////////////////

    status = DrawSpace::Utils::LoadDrawablePlugin( "chunk.dll", "chunk_plugin" );

    Drawable* drawable = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    drawable->RegisterPassSlot( "wireframe_pass" );
    drawable->SetRenderer( renderer );
    drawable->SetName( "orbiter_0" );

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );
    drawable->GetMeshe( "" )->SetImporter( m_meshe_import );

    drawable->GetMeshe( "" )->LoadFromFile( "planet.ac", 0 );


    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    drawable->GetNodeFromPass( "wireframe_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "mars.jpg" ) ), 0 );

    drawable->GetNodeFromPass( "wireframe_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( drawable );

    DrawSpace::Dynamics::Orbiter::Parameters cube_params;
    cube_params.shape_descr.sphere_radius = 0.5;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::SPHERE_SHAPE;
    cube_params.initial_pos = DrawSpace::Utils::Vector( 0.0, 0.0, 0.0, 1.0 );
    cube_params.inital_rot.Identity();

    m_orbiters[0] = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Orbiter, DrawSpace::Dynamics::Orbiter( &m_world, drawable ) );

    m_orbiters[0]->SetKinematic( cube_params );

    Orbiter::Orbit orbit0;
    orbit0.m_excentricity = 0.2;
    orbit0.m_ray = 25.0;

    m_orbiters[0]->SetOrbit1( orbit0 );

    //////////////////////////////////////////////////////////////


    status = DrawSpace::Utils::LoadFontImportPlugin( "cbfgfont.dll", "cbfgfont_plugin" );
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

    m_camera = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera" ) );
    m_scenegraph.RegisterNode( m_camera );

    m_scenegraph.SetCurrentCamera( "camera" );

    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_wireframepass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_freemove.SetTransformNode( m_camera );
    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 0.0, 20.0, 1.0 ) );


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
