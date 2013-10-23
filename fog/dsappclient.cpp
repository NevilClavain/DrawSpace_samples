
#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;

dsAppClient* dsAppClient::m_instance = NULL;


SceneTransform::SceneTransform( void ) : m_xangle( 0.0 ), m_yangle( 0.0 ), m_zpos( -8.0 )
{

}

SceneTransform::~SceneTransform( void )
{


}

void SceneTransform::Transform( void )
{
    DrawSpace::Utils::Matrix result;

    DrawSpace::Utils::Matrix translate;
    
    m_mutex_angles.WaitInfinite();
    translate.Translation( 0.0, 0.0, m_zpos );
    m_mutex_angles.Release();

    DrawSpace::Utils::Matrix yrotate;

    m_mutex_z.WaitInfinite();
    yrotate.Rotation( DrawSpace::Utils::Vector( 0.0, 1.0, 0.0, 1.0 ), 3.1415927 * m_yangle / 180.0 );
    DrawSpace::Utils::Matrix xrotate;
    xrotate.Rotation( DrawSpace::Utils::Vector( 1.0, 0.0, 0.0, 1.0 ), 3.1415927 * m_xangle / 180.0 );
    m_mutex_z.Release();

    DrawSpace::Utils::Transformation transform;

    transform.PushMatrix( translate );
    transform.PushMatrix( yrotate );
    transform.PushMatrix( xrotate );
    transform.BuildResult();
    transform.GetResult( &result );


    m_mutex_result.WaitInfinite();
    m_result = result;
    m_mutex_result.Release();
}

void SceneTransform::Run( void )
{
    while( 1 )
    {
        Transform();
        //Sleep( 1 );
    }
}


void SceneTransform::OnLeftDrag( DrawSpace::Utils::TimeManager& p_timer, long p_dx, long p_dy )
{
    m_mutex_angles.WaitInfinite();
    p_timer.AngleSpeedInc( &m_yangle, p_dx * 150.0 );
    p_timer.AngleSpeedInc( &m_xangle, p_dy * 150.0 );
    m_mutex_angles.Release();
}

void SceneTransform::OnRightDrag( DrawSpace::Utils::TimeManager& p_timer, long p_dx, long p_dy )
{
    m_mutex_z.WaitInfinite();
    p_timer.TranslationSpeedInc( &m_zpos, p_dy * 40.0 );
    m_mutex_z.Release();
}

void SceneTransform::GetResult( DrawSpace::Utils::Matrix& p_out )
{
    m_mutex_result.WaitInfinite();
    p_out = m_result;
    m_mutex_result.Release();
}




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

    m_fpsmove.Compute( m_timer, true );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::Plugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    DrawSpace::Utils::Matrix result;
    m_transform->GetResult( result );

    m_scenegraph.SetNodeLocalTransformation( "cube", result );

    m_scenegraph.ComputeTransformations();

    static long last_fps;

    long current_fps = m_timer.GetFPS();
    char fps[256];
    sprintf( fps, "%d"
        " fps", m_timer.GetFPS() );
    if( last_fps != current_fps )
    {
        m_fpstext_widget->SetText( 0, 0, 70, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
        last_fps = current_fps;		
    }


    m_fpstext_widget->SetVirtualTranslation( 15, 15 );
    m_fpstext_widget->Transform();



    m_texturepass->GetRenderingQueue()->Draw();
    m_fogintpass->GetRenderingQueue()->Draw();
    
    m_fogblendpass->GetRenderingQueue()->Draw();

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

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::Plugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 145, 230, 230 );

    //////////////////////////////////////////////////////////////

    m_fogintpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "fogint_pass" ) );
    m_fogintpass->GetRenderingQueue()->EnableDepthClearing( true );
    m_fogintpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_fogintpass->GetRenderingQueue()->SetTargetClearingColor( 255, 0, 0 );


    //////////////////////////////////////////////////////////////

    m_fogblendpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "fogblend_pass" ) );
    m_fogblendpass->CreateViewportQuad();


    m_fogblendpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogblend.vsh", false ) ) );
    m_fogblendpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogblend.psh", false ) ) );
    m_fogblendpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_fogblendpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_fogblendpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    m_fogblendpass->GetViewportQuad()->SetTexture( m_fogintpass->GetTargetTexture(), 1 );

    m_fogblendpass->GetViewportQuad()->LoadAssets();

    m_fogblendpass->GetViewportQuad()->GetFx()->AddShaderRealVectorParameter( 1, "fog_color", 0 );
    m_fogblendpass->GetViewportQuad()->GetFx()->SetShaderRealVector( "fog_color", DrawSpace::Utils::Vector( 145.0 / 255.0, 230.0 / 255.0, 230.0 / 255.0, 1.0 ) );


    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_fogblendpass->GetTargetTexture(), 0 );
    
    m_finalpass->GetViewportQuad()->LoadAssets();



    ///////////////////////////////////////////////////////////////

    m_scenegraph.RegisterPass( m_texturepass );
    m_scenegraph.RegisterPass( m_fogintpass );


    ///////////////////////////////////////////////////////////////
    
    m_chunknode = _DRAWSPACE_NEW_( ChunkNode, ChunkNode( "cube" ) );
    
    m_chunknode->RegisterPassRenderingNode( "fogint_pass", _DRAWSPACE_NEW_( RenderingNode, RenderingNode ) );
    m_chunknode->GetNodeFromPass( "fogint_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.vsh", false ) ) );
    m_chunknode->GetNodeFromPass( "fogint_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "fogintensity.psh", false ) ) );
    m_chunknode->GetNodeFromPass( "fogint_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunknode->GetNodeFromPass( "fogint_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunknode->GetNodeFromPass( "fogint_pass" )->GetFx()->AddShaderRealParameter( 0, "fog_intensity", 12 );
    m_chunknode->GetNodeFromPass( "fogint_pass" )->GetFx()->SetShaderReal( "fog_intensity", 0.02 );

    m_chunknode->GetNodeFromPass( "fogint_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunknode->GetNodeFromPass( "fogint_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );



    m_chunknode->RegisterPassRenderingNode( "texture_pass", _DRAWSPACE_NEW_( RenderingNode, RenderingNode ) );
    m_chunknode->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_chunknode->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_chunknode->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunknode->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunknode->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunknode->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_chunknode->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_chunknode->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_chunknode->GetNodeFromPass( "texture_pass" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_chunknode->GetNodeFromPass( "texture_pass" )->GetTexture( 0 )->LoadFromFile();


    DrawSpace::Utils::AC3DMesheImport importer;
    m_chunknode->GetMeshe()->SetImporter( &importer );
    m_chunknode->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_scenegraph.RegisterNode( m_chunknode );
    m_chunknode->LoadAssets();


    //////////////////////////////////////////////////////////////

    m_transform = _DRAWSPACE_NEW_( SceneTransform, SceneTransform );
    m_transformtask = _DRAWSPACE_NEW_( DrawSpace::Core::Task<SceneTransform>, DrawSpace::Core::Task<SceneTransform>( DrawSpace::Core::Task<SceneTransform>::Kill ) );


    m_transformtask->Startup( m_transform );


    //////////////////////////////////////////////////////////////

    DrawSpace::Utils::CBFGFontImport fontimporter;
    m_font = _DRAWSPACE_NEW_( Font, Font );

    m_font->SetImporter( &fontimporter );

    status = m_font->Build( "mangalfont.bmp", "mangalfont.csv" );
    if( !status )
    {
        return false;
    }



    m_fpstext_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( "fps_text_widget", 20, 10, m_font, false, NULL ) );

    m_fpstext_widget->GetImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
    m_fpstext_widget->GetImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

    m_fpstext_widget->GetImageFx()->GetShader( 0 )->SetText( 
        
        "float4x4 matViewProjection: register(c0);"
        "struct VS_INPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"
        "struct VS_OUTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"
        "VS_OUTPUT vs_main( VS_INPUT Input )"
        "{"
           "VS_OUTPUT Output;"
           "Output.Position = mul( Input.Position, matViewProjection );"
           "Output.TexCoord0 = Input.TexCoord0;"  
           "return( Output );"
        "}"     
        );

    m_fpstext_widget->GetImageFx()->GetShader( 1 )->SetText( 

        "float4 text_color: register(c0);"
        "sampler2D Texture0;"
        "struct PS_INTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"
        "float4 ps_main( PS_INTPUT input ) : COLOR0"
        "{"
           "float4 color = tex2D( Texture0, input.TexCoord0 );"

           "float4 final_color;"

           "final_color.r = text_color.r;"
           "final_color.g = text_color.g;"
           "final_color.b = text_color.b;"
           "final_color.a = color.r;"

           "return final_color;"

        "}"        
      );

                                                                


    m_fpstext_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_fpstext_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    m_fpstext_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    m_fpstext_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha"  ) );
    m_fpstext_widget->GetImageFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    m_fpstext_widget->GetImageFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    m_fpstext_widget->GetImageFx()->AddShaderRealVectorParameter( 1, "color", 0 );
    m_fpstext_widget->GetImageFx()->SetShaderRealVector( "color", Utils::Vector( 0.0, 0.4, 0.0, 0.0 ) );



    /*
    m_fpstext_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_fpstext_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_fpstext_widget->GetTextFx()->GetShader( 0 )->LoadFromFile();
    m_fpstext_widget->GetTextFx()->GetShader( 1 )->LoadFromFile();
    */

    m_fpstext_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
    m_fpstext_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

    m_fpstext_widget->GetTextFx()->GetShader( 0 )->SetText( 

        "float4x4 matViewProjection: register(c0);"

        "struct VS_INPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"  
        "};"

        "struct VS_OUTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"

        "VS_OUTPUT vs_main( VS_INPUT Input )"
        "{"
           "VS_OUTPUT Output;"

           "Output.Position = mul( Input.Position, matViewProjection );"
           "Output.TexCoord0 = Input.TexCoord0;"
              
           "return( Output );"
        "}"
    );


    m_fpstext_widget->GetTextFx()->GetShader( 1 )->SetText( 
        
        "float4 text_color: register(c0);"
        "sampler2D Texture0;"

        "struct PS_INTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"

        "float4 ps_main( PS_INTPUT input ) : COLOR0"
        "{"
           "float4 color = tex2D( Texture0, input.TexCoord0 );"
           "if( color.r == 0.0 && color.g == 0.0 && color.b == 0.0 )"
           "{"
              "clip( -1.0 );"
           "}"
           "return color * text_color;"
        "}"
    );

    m_fpstext_widget->GetTextFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_fpstext_widget->GetTextFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_fpstext_widget->GetTextFx()->AddShaderRealVectorParameter( 1, "color", 0 );
    m_fpstext_widget->GetTextFx()->SetShaderRealVector( "color", Utils::Vector( 1.0, 1.0, 1.0, 0.0 ) );

    m_fpstext_widget->SetPassTargetClearingColor( 0, 0, 0 );

    m_fpstext_widget->RegisterToPass( m_finalpass );
    m_fpstext_widget->LoadAssets();


    //////////////////////////////////////////////////////////////


    m_camera = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera" ) );
    m_scenegraph.Add( m_camera );

    m_scenegraph.SetCurrentCamera( "camera" );

    m_fpsmove.SetTransformNode( m_camera );
    m_fpsmove.Init( DrawSpace::Utils::Vector( 0.0, 1.0, 6.0, 1.0 ) );

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
    if( m_mouselb )
    {
        m_transform->OnLeftDrag( m_timer, p_dx, p_dy );
    }
    else if( m_mouserb )
    {
        m_transform->OnRightDrag( m_timer, p_dx, p_dy );
    }

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
