
#include "dsappclient.h"



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_box_count( 0 ), m_box_texture( 0 )
{    
    _INIT_LOGGER( "physics.conf" )  
    m_w_title = "physics test";    
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{

    m_fpsmove.Compute( m_timer, true );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    DrawSpace::Utils::Matrix sbtrans;

    sbtrans.Scale( 20.0, 20.0, 20.0 );
    m_scenegraph.SetNodeLocalTransformation( "spacebox", sbtrans );

    m_ground_body->Update();


    for( size_t i = 0; i < m_boxes.size(); i++ )
    {
        m_boxes[i].inert_body->Update();
    }

 
    m_scenegraph.ComputeTransformations();    

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

    m_texturepass->GetRenderingQueue()->Draw();


    m_fpstext_widget->Draw();

    m_finalpass->GetRenderingQueue()->Draw();


    dsstring date;
    m_calendar->GetFormatedDate( date );

    renderer->DrawText( 255, 0, 0, 30, 50, "%d %d", m_texturepass->GetRenderingQueue()->GetSwitchesCost(), m_texturepass->GetRenderingQueue()->GetTheoricalSwitchesCost() );

    renderer->DrawText( 0, 255, 0, 10, 85, "%s", date.c_str() );


    renderer->FlipScreen();

    /*
    m_timer.Update();
    if( m_timer.IsReady() )
    {
        m_world.StepSimulation( m_timer.GetFPS() );
    }
    */

    m_calendar->Run();
}

void dsAppClient::create_box( void )
{
    
    DrawSpace::Interface::Drawable* drawable;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    drawable = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    drawable->RegisterPassSlot( "texture_pass" );
    drawable->SetRenderer( renderer );

    char name[32];
    sprintf( name, "box_%d", m_box_count );

    drawable->SetName( name );
    
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

    
    if( 0 == m_box_texture )
    {
        drawable->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "shelby.jpg" ) ), 0 );
        m_box_texture = 1;
    }
    else
    {
        drawable->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );
        m_box_texture = 0;
    }
    

    drawable->GetNodeFromPass( "texture_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( drawable );


    DrawSpace::Dynamics::InertBody::Parameters cube_params;

    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube_params.initial_pos = DrawSpace::Utils::Vector( 0.0, 10.5, 0.0, 1.0 );
    cube_params.initial_rot.Identity();

    DrawSpace::Dynamics::InertBody* cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, drawable, cube_params ) );


    Box box;

    box.drawable = drawable;
    box.inert_body = cube_body;

    m_boxes.push_back( box );

    m_box_count++;


    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();

    
}

bool dsAppClient::OnIdleAppInit( void )
{
    bool status;

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0 );
    /*
    */



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


    status = DrawSpace::Utils::LoadDrawablePlugin( "chunk.dll", "chunk_plugin" );
    m_ground = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    m_ground->RegisterPassSlot( "texture_pass" );
    m_ground->SetRenderer( renderer );
    m_ground->SetName( "ground" );

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );
    m_ground->GetMeshe( "" )->SetImporter( m_meshe_import );

    m_ground->GetMeshe( "" )->LoadFromFile( "grid.ac", 0 );
    

    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    

    m_ground->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "ground.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( m_ground );



    ////////////////////////////////////////////////////////////////

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

    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->SetOrderNumber( 200 );



    m_scenegraph.RegisterNode( m_spacebox );




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

    

    m_fpsmove.SetTransformNode( m_camera );
    m_fpsmove.Init( DrawSpace::Utils::Vector( 0.0, 0.75, 12.0, 1.0 ) );

    m_mouse_circularmode = true;


    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera2" ) );
    m_scenegraph.RegisterNode( m_camera2 );


    //m_box->AddChild( m_camera2 );

    //m_scenegraph.SetCurrentCamera( "camera2" );
    m_scenegraph.SetCurrentCamera( "camera" );


    
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();


    //////////////////////////////////////////////////////////////

    m_world.Initialize();
    m_world.SetGravity( DrawSpace::Utils::Vector( 0.0, -9.81, 0.0, 0.0 ) );

    DrawSpace::Dynamics::InertBody::Parameters ground_params;
    //ground_params.box_dims = DrawSpace::Utils::Vector( 100.0, 0.0, 100., 1.0 );
    ground_params.mass = 0.0;
    //ground_params.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    ground_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    ground_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 100.0, 0.0, 100., 1.0 );
    ground_params.initial_pos = DrawSpace::Utils::Vector( 0.0, 0.0, 0.0, 1.0 );
    ground_params.initial_rot.Identity();

    m_ground_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, m_ground, ground_params ) );


    create_box();

    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer, &m_world ) );


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

        case VK_SPACE:

            create_box();
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

            m_calendar->SetTimeFactor( Calendar::DIV2_TIME );
            break;

        case VK_F4:

            m_calendar->SetTimeFactor( Calendar::DIV10_TIME );
            break;

        case VK_F5:
            m_calendar->Suspend( true );
            break;

        case VK_F6:
            m_calendar->Suspend( false );
            break;

    }
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
