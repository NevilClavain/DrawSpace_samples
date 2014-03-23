
#include "dsappclient.h"



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;

_DECLARE_DS_LOGGER( logger, "AppClient" )


dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_speed( 0.0 ), m_speed_speed( 5.0 )
{    
    _INIT_LOGGER( "orbiters.conf" )  
    m_w_title = "orbiters test";
}

dsAppClient::~dsAppClient( void )
{

}

Orbiter* dsAppClient::build_planet( char* p_name, char* p_texture )
{
    Drawable* drawable;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    drawable = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    drawable->RegisterPassSlot( "texture_pass" );
    drawable->SetRenderer( renderer );
    drawable->SetName( p_name );

    drawable->GetMeshe( "" )->SetImporter( m_meshe_import );

    drawable->GetMeshe( "" )->LoadFromFile( "planet.ac", 0 );


    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    drawable->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( p_texture ) ), 0 );

    drawable->GetNodeFromPass( "texture_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( drawable );

    DrawSpace::Dynamics::Orbiter::Parameters sphere_params;
    sphere_params.shape_descr.sphere_radius = 0.5;
    sphere_params.shape_descr.shape = DrawSpace::Dynamics::Body::SPHERE_SHAPE;
    sphere_params.initial_pos = DrawSpace::Utils::Vector( 0.0, 0.0, 0.0, 1.0 );
    sphere_params.initial_rot.Identity();

    Orbiter* orbiter = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Orbiter, DrawSpace::Dynamics::Orbiter( &m_world, drawable ) );

    orbiter->SetKinematic( sphere_params );

    return orbiter;
}

Drawable* dsAppClient::build_orbit_drawable( char* p_name, Orbit* p_orbit )
{
    Drawable* drawable;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    drawable = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    drawable->RegisterPassSlot( "texture_pass" );
    drawable->SetRenderer( renderer );
    drawable->SetName( p_name );

    Meshe* orb0_meshe = drawable->GetMeshe( "" );

    p_orbit->BuildMeshe( 10.0, orb0_meshe );

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "line.vsh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "line.psh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "none" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    drawable->GetNodeFromPass( "texture_pass", "" )->AddShaderParameter( 1, "color", 0 );
    drawable->GetNodeFromPass( "texture_pass", "" )->SetShaderRealVector( "color", Vector( 1.0, 0.0, 0.0, 1.0 ) );

    drawable->GetNodeFromPass( "texture_pass", "" )->AddShaderParameter( 1, "thickness", 1 );
    drawable->GetNodeFromPass( "texture_pass", "" )->SetShaderReal( "thickness", 0.005 );

    m_scenegraph.RegisterNode( drawable );

    return drawable;
}

void dsAppClient::OnRenderFrame( void )
{
    m_freemove.Compute( m_timer );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    
    DrawSpace::Utils::Matrix sbtrans;
    sbtrans.Scale( 20.0, 20.0, 20.0 );
    m_scenegraph.SetNodeLocalTransformation( "spacebox", sbtrans );
    


    m_cube_body->Update();
    m_cube_body_2->Update();


    
    
    Matrix origin;
    origin.Identity();



    m_mars_orbit->OrbitStep( origin );
    m_saturn_orbit->OrbitStep( origin );
    
    

    

     
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

    /*
    m_timer.Update();
    if( m_timer.IsReady() )
    {
        m_world.StepSimulation( m_timer.GetFPS() ); 


    }
    */

    m_calendar->Run();

    m_freemove.SetSpeed( m_speed );
}

bool dsAppClient::OnIdleAppInit( void )
{

    /////////////////////////////////////

    bool status;
    Drawable* drawable;

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0 );


    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    

    //////////////////////////////////////////////////////////////

    m_scenegraph.RegisterPass( m_texturepass );


    //////////////////////////////////////////////////////////////


    status = DrawSpace::Utils::LoadDrawablePlugin( "spacebox.dll", "spacebox_plugin" );

    m_spacebox = DrawSpace::Utils::InstanciateDrawableFromPlugin( "spacebox_plugin" );
    m_spacebox->RegisterPassSlot( "texture_pass" );
    m_spacebox->SetRenderer( renderer );
    m_spacebox->SetName( "spacebox" );


    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "front" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "rear" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "top" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "bottom" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "left" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "right" );

    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_front5.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_back6.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_top3.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_bottom4.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_left2.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_right1.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->SetOrderNumber( 200 );


    m_scenegraph.RegisterNode( m_spacebox );
    

    //////////////////////////////////////////////////////////////


    m_world.Initialize();
    
 
    status = DrawSpace::Utils::LoadDrawablePlugin( "chunk.dll", "chunk_plugin" );

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );



   
    
    m_sun = build_planet( "sun", "texture_sun.jpg" );


    
    m_mars = build_planet( "mars", "mars.jpg" );

    m_mars_centroid = _DRAWSPACE_NEW_( Centroid, Centroid );
    m_mars_centroid->SetOrbiter( m_mars );

    m_mars_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 25.0, 0.37, 0.0, 9.0, 0.0, 0.0, 1.0, m_mars_centroid ) );

    m_mars_orbit->RegisterDrawable( build_orbit_drawable( "mars_orbit", m_mars_orbit ) );


    


    m_moon = build_planet( "moon", "mars.jpg" );

    m_moon_centroid = _DRAWSPACE_NEW_( Centroid, Centroid );
    m_moon_centroid->SetOrbiter( m_moon );

    m_moon_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 5.0, 0.45, 0.0, 10.0, 3.0, 0.0, 0.0001, m_moon_centroid ) );

    m_moon_orbit->RegisterDrawable( build_orbit_drawable( "moon_orbit", m_moon_orbit ) );


    m_mars_centroid->RegisterSubOrbit( m_moon_orbit );





    m_saturn = build_planet( "saturn", "saturnmap.jpg" );

    m_saturn_centroid = _DRAWSPACE_NEW_( Centroid, Centroid );
    m_saturn_centroid->SetOrbiter( m_saturn );

    m_saturn_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 60.0, 0.9999, 8.0, 1.0, 0.0, 0.0, 20.0, m_saturn_centroid ) );

    m_saturn_orbit->RegisterDrawable( build_orbit_drawable( "saturn_orbit", m_saturn_orbit ) );


    


    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer, &m_world ) );

    

    //////////////////////////////////////////////////////////////


    drawable = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    drawable->RegisterPassSlot( "texture_pass" );
    drawable->SetRenderer( renderer );

    drawable->SetName( "box" );
    
    drawable->GetMeshe( "" )->SetImporter( m_meshe_import );

    drawable->GetMeshe( "" )->LoadFromFile( "object.ac", 0 );    

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    drawable->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );



    drawable->GetNodeFromPass( "texture_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( drawable );


    DrawSpace::Dynamics::InertBody::Parameters cube_params;
    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube_params.initial_pos = DrawSpace::Utils::Vector( -5.0, 0.0, -100.0, 1.0 );
    cube_params.initial_rot.Identity();

    m_cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, drawable, cube_params ) );





    drawable = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    drawable->RegisterPassSlot( "texture_pass" );
    drawable->SetRenderer( renderer );

    drawable->SetName( "box2" );
    
    drawable->GetMeshe( "" )->SetImporter( m_meshe_import );

    drawable->GetMeshe( "" )->LoadFromFile( "object.ac", 0 );    

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    drawable->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );



    drawable->GetNodeFromPass( "texture_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( drawable );


    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube_params.initial_pos = DrawSpace::Utils::Vector( 5.0, 0.0, -100.0, 1.0 );
    cube_params.initial_rot.Identity();

    m_cube_body_2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, drawable, cube_params ) );







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

    //m_moon->GetDrawable()->AddChild( m_camera );



    m_scenegraph.SetCurrentCamera( "camera" );

    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_freemove.SetTransformNode( m_camera );
    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 0.0, 20.0, 1.0 ) );


    m_mouse_circularmode = true;

    m_calendar->RegisterOrbit( m_mars_orbit );
    m_calendar->RegisterOrbit( m_moon_orbit );



    m_calendar->Startup( 162682566 );

        
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

            m_calendar->SetTimeFactor( Calendar::MUL2_TIME );
            break;

        case VK_F3:

            m_calendar->SetTimeFactor( Calendar::SEC_1DAY_TIME );
            break;

        case VK_F4:

            m_calendar->SetTimeFactor( Calendar::DIV2_TIME );
            break;

        case VK_F5:

            m_calendar->SetTimeFactor( Calendar::PAUSE_TIME );
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
