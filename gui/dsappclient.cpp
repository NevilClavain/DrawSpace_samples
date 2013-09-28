
#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;

dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_yangle( 0.0 ), m_xangle( 0.0 ), m_zangle( 0.0 ), m_mouselb( false ), m_xmouse( 0 ), m_ymouse( 0 )
{    
    _INIT_LOGGER( "gui.conf" )  
    m_w_title = "gui test";
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{
    DrawSpace::Utils::Matrix result;

    DrawSpace::Utils::Matrix translate;
    translate.Translation( 0.0, 0.0, -8.0 );
    DrawSpace::Utils::Matrix yrotate;
    yrotate.Rotation( DrawSpace::Utils::Vector( 0.0, 1.0, 0.0, 1.0 ), 3.1415927 * m_yangle / 180.0 );

    DrawSpace::Utils::Matrix xrotate;
    xrotate.Rotation( DrawSpace::Utils::Vector( 1.0, 0.0, 0.0, 1.0 ), 3.1415927 * m_xangle / 180.0 );

    DrawSpace::Utils::Transformation transform;

    transform.PushMatrix( translate );
    transform.PushMatrix( yrotate );
    transform.PushMatrix( xrotate );
    transform.BuildResult();
    transform.GetResult( &result );

    m_scenegraph.SetNodeLocalTransformation( "cube", result );
    m_scenegraph.ComputeTransformations();



    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::Plugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    static long last_fps;

    long current_fps = m_timer.GetFPS();
    char fps[256];
    sprintf( fps, "%d fps", m_timer.GetFPS() );

    if( last_fps != current_fps )
    {
        m_text_widget->SetText( 5, 120, 15, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );

        m_fpstext_widget->SetText( 0, 0, 70, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
        last_fps = current_fps;		
    }

    

    m_image1->SetVirtualTranslation( m_xmouse, m_ymouse );

    m_fpstext_widget->SetVirtualTranslation( 15, 5 );

    m_texturepass->GetRenderingQueue()->Draw();


    m_text_widget->Draw();
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
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 14, 30, 23 );

    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    
    m_finalpass->GetViewportQuad()->LoadAssets();

    ///////////////////////////////////////////////////////////////

    m_scenegraph.RegisterPass( m_texturepass );

    ///////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////

    DrawSpace::Core::RenderingNode* texture_rnode = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );

    texture_rnode->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    texture_rnode->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    texture_rnode->GetFx()->GetShader( 0 )->LoadFromFile();
    texture_rnode->GetFx()->GetShader( 1 )->LoadFromFile();


    texture_rnode->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    texture_rnode->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    texture_rnode->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    texture_rnode->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );


    texture_rnode->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    texture_rnode->GetTexture( 0 )->LoadFromFile();


    m_chunknode = _DRAWSPACE_NEW_( ChunkNode, ChunkNode( "cube" ) );

    DrawSpace::Utils::AC3DMesheImport importer;
    m_chunknode->GetMeshe()->SetImporter( &importer );
    m_chunknode->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_chunknode->RegisterPassRenderingNode( "texture_pass", texture_rnode );


    m_scenegraph.RegisterNode( m_chunknode );
    m_chunknode->LoadAssets();

    //////////////////////////////////////////////////////////////////
    
    m_image1 = _DRAWSPACE_NEW_( Image, Image( (long)20, (long)20 ) );

    m_image1->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texturekeyed.vsh", false ) ) );
    m_image1->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texturekeyed.psh", false ) ) );
    m_image1->GetFx()->GetShader( 0 )->LoadFromFile();
    m_image1->GetFx()->GetShader( 1 )->LoadFromFile();


    m_image1->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "money.bmp" ) ), 0 );
    m_image1->GetTexture( 0 )->LoadFromFile();



    m_finalpass->GetRenderingQueue()->Add( m_image1 );

    status = m_image1->LoadAssets();
    if( !status )
    {
        return false;
    }


    /////////////////////////////////////////////////////////////////


    DrawSpace::Utils::CBFGFontImport fontimporter;
    m_font = _DRAWSPACE_NEW_( Font, Font );

    m_font->SetImporter( &fontimporter );

    status = m_font->Build( "mangalfont.bmp", "mangalfont.csv" );
    if( !status )
    {
        return false;
    }

    //////////////////////////////////////////////////////////////////////


    m_text_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget", 50, 30, m_font ) );

    m_text_widget->GetBackGroundImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetBackGroundImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_text_widget->GetBackGroundImageFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetBackGroundImageFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget->SetBackgroundTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_text_widget->GetBackGroundImageTexture( 0 )->LoadFromFile();
    
    m_text_widget->GetImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_text_widget->GetImageFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetImageFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_text_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_text_widget->GetTextFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetTextFx()->GetShader( 1 )->LoadFromFile();
    m_text_widget->GetTextFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_text_widget->GetTextFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_text_widget->GetTextFx()->AddShaderRealVectorParameter( 1, "color", 0 );
    m_text_widget->GetTextFx()->SetShaderRealVector( "color", Utils::Vector( 0.0, 1.0, 0.0, 1.0 ) );

    m_text_widget->RegisterToPass( m_finalpass );
    m_text_widget->LoadAssets();

    //////////////////////////////////////////////////////////////////////

    m_fpstext_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( "fps_text_widget", 20, 10, m_font, false ) );

    m_fpstext_widget->GetImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_fpstext_widget->GetImageFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_fpstext_widget->GetImageFx()->GetShader( 0 )->LoadFromFile();
    m_fpstext_widget->GetImageFx()->GetShader( 1 )->LoadFromFile();

    m_fpstext_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_fpstext_widget->GetTextFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_fpstext_widget->GetTextFx()->GetShader( 0 )->LoadFromFile();
    m_fpstext_widget->GetTextFx()->GetShader( 1 )->LoadFromFile();
    m_fpstext_widget->GetTextFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_fpstext_widget->GetTextFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_fpstext_widget->GetTextFx()->AddShaderRealVectorParameter( 1, "color", 0 );
    m_fpstext_widget->GetTextFx()->SetShaderRealVector( "color", Utils::Vector( 0.0, 1.0, 0.0, 1.0 ) );

    m_fpstext_widget->RegisterToPass( m_finalpass );
    m_fpstext_widget->LoadAssets();


    return true;
}

void dsAppClient::OnAppInit( void )
{

}

void dsAppClient::OnClose( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
    DrawSpace::Core::Plugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface->DumpMemoryAllocs();
}

void dsAppClient::OnKeyPress( long p_key ) 
{

}

void dsAppClient::OnEndKeyPress( long p_key )
{

}

void dsAppClient::OnKeyPulse( long p_key )
{

}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( m_mouselb )
    {
        m_timer.AngleSpeedInc( &m_yangle, p_dx * 75.0 );
        m_timer.AngleSpeedInc( &m_xangle, p_dy * 75.0 );
    }

    dsreal xmouse, ymouse;
    xmouse = m_xmouse;
    ymouse = m_ymouse;

    m_timer.TranslationSpeedInc( &xmouse,  p_dx * 200 );
    m_timer.TranslationSpeedInc( &ymouse,  p_dy * 200 );

    m_xmouse = (long)xmouse;
    m_ymouse = (long)ymouse;


    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::Plugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    DrawSpace::Interface::Renderer::Characteristics characteristics;
    renderer->GetRenderCharacteristics( characteristics );

    if( m_xmouse > DRAWSPACE_GUI_WIDTH - 1 )
    {
        m_xmouse = DRAWSPACE_GUI_WIDTH - 1;
    }
    if( m_xmouse < 0 )
    {
        m_xmouse = 0;
    }

    if( m_ymouse > DRAWSPACE_GUI_HEIGHT - 1 )
    {
        m_ymouse = DRAWSPACE_GUI_HEIGHT - 1;
    }
    if( m_ymouse < 0 )
    {
        m_ymouse = 0;
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

}

void dsAppClient::OnMouseRightButtonUp( long p_xm, long p_ym )
{

}

void dsAppClient::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{

}
