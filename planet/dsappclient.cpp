
#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_speed( 0.0 ), m_speed_speed( 5.0 ), m_update_hp( true )
{    
    _INIT_LOGGER( "planet.conf" )  
    m_w_title = "planet engine test";
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


    
    DrawSpace::Utils::Matrix planet_trans;
    DrawSpace::Utils::Matrix planet_rot;
    DrawSpace::Utils::Matrix planet_res;

    planet_trans.Translation( 10000000.0, 0.0, -20000000.0 );
    planet_rot.Rotation( DrawSpace::Utils::Vector( 0.25, 1.0, 0.0, 1.0 ), Maths::DegToRad( 110.0 ) );

    //planet_res = planet_rot * planet_trans;
    planet_res.Identity();
    m_scenegraph.SetNodeLocalTransformation( "planet01", planet_res );
    

 
    m_scenegraph.ComputeTransformations();


    DrawSpace::Utils::Matrix camera_pos;
    m_scenegraph.GetCurrentCameraTranform( camera_pos );

    TypedProperty<DrawSpace::Utils::Vector> hotpoint( "hotpoint" );

    hotpoint.m_value[0] = camera_pos( 3, 0 );
    hotpoint.m_value[1] = camera_pos( 3, 1 );
    hotpoint.m_value[2] = camera_pos( 3, 2 );

    if( m_update_hp )
    {
        m_planet->SetProperty( "hotpoint", &hotpoint );
        m_planet->ComputeSpecifics();
    }

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

/*
    TypedProperty<dsreal>* altitud = static_cast<TypedProperty<dsreal>*>( m_planet->GetProperty( "altitud" ) );

    TypedProperty<DrawSpace::Utils::Vector>* pos = static_cast<TypedProperty<DrawSpace::Utils::Vector>*>( m_planet->GetProperty( "hotpoint" ) );
    TypedProperty<DrawSpace::Utils::Vector>* relative_pos = static_cast<TypedProperty<DrawSpace::Utils::Vector>*>( m_planet->GetProperty( "relative_hotpoint" ) );

    char planetinfos[256];
    sprintf( planetinfos, "relative pos : %.2f %.2f %.2f altitud %.2f", relative_pos->m_value[0], relative_pos->m_value[1], relative_pos->m_value[2], 
                                                                                    altitud->m_value );
    m_planetinfos_widget->SetText( 5, 5, 7, planetinfos );


    m_planetinfos_widget->SetVirtualTranslation( 90, 70 );
    m_planetinfos_widget->Transform();
*/

    m_wireframepass->GetRenderingQueue()->Draw();

    m_fpstext_widget->Draw();
    m_planetinfos_widget->Draw();

    m_finalpass->GetRenderingQueue()->Draw();

    renderer->FlipScreen();

    m_timer.Update();
    if( m_timer.IsReady() )
    {
       
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

    status = DrawSpace::Utils::LoadDrawablePlugin( "planetbuild.dll", "planet_plugin" );
    m_planet = DrawSpace::Utils::InstanciateDrawableFromPlugin( "planet_plugin" );

    m_planet->SetRenderer( renderer );
    m_planet->RegisterPassSlot( "wireframe_pass" );
    m_planet->SetName( "planet01" );

    
    std::vector<dsstring> idslist;
    m_planet->GetNodesIdsList( idslist );
    for( size_t i = 0; i < idslist.size(); i++ )
    {
        m_planet->SetNodeFromPassSpecificFx( "wireframe_pass", idslist[i], "main_fx" );
    }
    

    m_scenegraph.RegisterNode( m_planet );

    DrawSpace::Core::TypedProperty<dsreal> planet_diameter( "diameter", /*12000000.0*/ 12000.0 );
    m_planet->SetProperty( "diameter", &planet_diameter );

    m_planet->Initialize();

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


    m_planetinfos_widget = DrawSpace::Utils::BuildText( m_font, 180, 120, DrawSpace::Utils::Vector( 1.0, 1.0, 1.0, 0.0 ), "planetinfos" );
    m_planetinfos_widget->RegisterToPass( m_finalpass );


    //////////////////////////////////////////////////////////////

    m_camera = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera" ) );
    m_scenegraph.RegisterNode( m_camera );

    m_scenegraph.SetCurrentCamera( "camera" );

    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_wireframepass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_freemove.SetTransformNode( m_camera );
    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 0.0, /*20000000.0*/ 40000.0, 1.0 ) );


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
        case VK_F1:

            m_update_hp = false;
            break;

        case VK_F2:

            m_update_hp = true;
            break;

        case VK_F3:
            {
                TypedProperty<dsstring> patchname( "split" );
                patchname.m_value = ".0";

                m_planet->SetProperty( "split", &patchname );
            }
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
