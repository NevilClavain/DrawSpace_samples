
#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false )
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

    m_wireframepass->GetRenderingQueue()->Draw();


    m_fpstext_widget->Draw();

    m_finalpass->GetRenderingQueue()->Draw();

    renderer->FlipScreen();

    m_timer.Update();
    if( m_timer.IsReady() )
    {
       
    }
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
    
    m_finalpass->GetViewportQuad()->LoadAssets();

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



    m_spacebox->LoadAssets();


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
        m_planet->GetNodeFromPass( "wireframe_pass", idslist[i] )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
        m_planet->GetNodeFromPass( "wireframe_pass", idslist[i] )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

        m_planet->GetNodeFromPass( "wireframe_pass", idslist[i] )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
        m_planet->GetNodeFromPass( "wireframe_pass", idslist[i] )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

        m_planet->GetNodeFromPass( "wireframe_pass", idslist[i] )->GetFx()->GetShader( 0 )->SetText(

                "float4x4 matWorldViewProjection: register(c0);"

                "struct VS_INPUT"
                "{"
                   "float4 Position : POSITION0;"
                "};"

                "struct VS_OUTPUT"
                "{"
                   "float4 Position : POSITION0;"
                "};"

                "VS_OUTPUT vs_main( VS_INPUT Input )"
                "{"
                   "VS_OUTPUT Output;"
                   "Output.Position = mul( Input.Position, matWorldViewProjection );"                      
                   "return( Output );"
                "}"

                );

        m_planet->GetNodeFromPass( "wireframe_pass", idslist[i] )->GetFx()->GetShader( 1 )->SetText(

                "float4 Color : register(c0);"

                "sampler2D Texture0;"

                "struct PS_INTPUT"
                "{"
                   "float4 Position : POSITION0;"
                "};"

                "float4 ps_main( PS_INTPUT input ) : COLOR0"
                "{"
                   "return Color;"
                "}"
                );

        m_planet->GetNodeFromPass( "wireframe_pass", idslist[i] )->GetFx()->AddShaderRealVectorParameter( 1, "color", 0 );
        m_planet->GetNodeFromPass( "wireframe_pass", idslist[i] )->GetFx()->SetShaderRealVector( "color", Vector( 0.0, 0.0, 1.0, 0.0 ) );
    }

    m_scenegraph.RegisterNode( m_planet );

    DrawSpace::Core::TypedProperty<dsreal> planet_diameter( "diameter", 12.0 );
    m_planet->SetProperty( "diameter", &planet_diameter );

    m_planet->LoadAssets();

    DrawSpace::Core::Meshe* mesh = m_planet->GetMeshe( "front:.0" );
   

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

    m_fpstext_widget->RegisterToPass( m_finalpass );
    m_fpstext_widget->LoadAssets();


    //////////////////////////////////////////////////////////////

    m_camera = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera" ) );
    m_scenegraph.RegisterNode( m_camera );

    m_scenegraph.SetCurrentCamera( "camera" );
    
    m_freemove.SetTransformNode( m_camera );
    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 6.0, 12.0, 1.0 ) );


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
            m_freemove.SetSpeed( 6.0 );
            break;

        case 'W':
            m_freemove.SetSpeed( -6.0 );
            break;

    }
}

void dsAppClient::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':
            m_freemove.SetSpeed( 0.0 );
            break;


        case 'W':
            m_freemove.SetSpeed( 0.0 );
            break;

        case VK_SPACE:

            m_planet->ComputeSpecifics();
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
