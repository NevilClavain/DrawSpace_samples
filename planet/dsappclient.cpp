
#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Planet;
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

    m_fpsmove.Compute( m_timer, true );

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

void dsAppClient::prepare_spaceboxnode( const dsstring& p_nodeid, const dsstring& p_texture )
{


    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );


    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddShaderRealVectorParameter( 1, "color", 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 0.0 ) );

    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( p_texture ) ), 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    



    /*
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->GetShader( 0 )->SetText(

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

    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->GetShader( 1 )->SetText(

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

    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->AddShaderRealVectorParameter( 1, "color", 0 );
    m_spacebox->GetNodeFromPass( "wireframe_pass", p_nodeid )->GetFx()->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 0.0 ) );
    */
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



    status = DrawSpace::Utils::LoadDrawablePlugin( "spacebox.dll", "spacebox_plugin" );

    m_spacebox = DrawSpace::Utils::InstanciateDrawableFromPlugin( "spacebox_plugin" );
    m_spacebox->RegisterPassSlot( "wireframe_pass" );
    m_spacebox->SetRenderer( renderer );
    m_spacebox->SetName( "spacebox" );


    prepare_spaceboxnode( "front", "spacebox_front5.png" );
    prepare_spaceboxnode( "rear", "spacebox_back6.png" );
    prepare_spaceboxnode( "top", "spacebox_top3.png" );
    prepare_spaceboxnode( "bottom", "spacebox_bottom4.png" );
    prepare_spaceboxnode( "left", "spacebox_left2.png" );
    prepare_spaceboxnode( "right", "spacebox_right1.png" );


    m_spacebox->LoadAssets();


    m_scenegraph.RegisterNode( m_spacebox );



    //////////////////////////////////////////////////////////////



    ///////////////////////////////////////////////////////////////



    m_planet = _DRAWSPACE_NEW_( Body, Body( "planet1" ) );

    m_planet->RegisterPassFaceSet( "wireframe_pass" );


    for( long i = 0; i < Body::AllPlanetFaces; i++ )
    {
        m_planet->GetPassFaceFx( "wireframe_pass", i )->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
        m_planet->GetPassFaceFx( "wireframe_pass", i )->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

        m_planet->GetPassFaceFx( "wireframe_pass", i )->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
        m_planet->GetPassFaceFx( "wireframe_pass", i )->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

        m_planet->GetPassFaceFx( "wireframe_pass", i )->GetShader( 0 )->SetText(

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

        m_planet->GetPassFaceFx( "wireframe_pass", i )->GetShader( 1 )->SetText(

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

        m_planet->GetPassFaceFx( "wireframe_pass", i )->AddShaderRealVectorParameter( 1, "color", 0 );
        m_planet->GetPassFaceFx( "wireframe_pass", i )->SetShaderRealVector( "color", Vector( 0.0, 0.0, 1.0, 0.0 ) );
    }

    

    m_scenegraph.RegisterNode( m_planet );

    m_planet->LoadAssets();


    m_planet->GetFace( "wireframe_pass", DrawSpace::Planet::Patch::FrontPlanetFace )->Split( ".0" );
    m_planet->GetFace( "wireframe_pass", DrawSpace::Planet::Patch::FrontPlanetFace )->Split( ".0.0" );
    m_planet->GetFace( "wireframe_pass", DrawSpace::Planet::Patch::FrontPlanetFace )->Split( ".0.0.2" );


    //////////////////////////////////////////////////////////////


    DrawSpace::Utils::CBFGFontImport fontimporter;
    m_font = _DRAWSPACE_NEW_( Font, Font );

    m_font->SetImporter( &fontimporter );

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

    m_fpsmove.SetTransformNode( m_camera );
    m_fpsmove.Init( DrawSpace::Utils::Vector( 0.0, 6.0, 12.0, 1.0 ) );

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
