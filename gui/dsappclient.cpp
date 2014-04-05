
#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;

dsAppClient* dsAppClient::m_instance = NULL;

_DECLARE_DS_LOGGER( logger, "AppClient" )


dsAppClient::dsAppClient( void ) : 
m_yangle( 0.0 ), 
m_xangle( 0.0 ), 
m_zangle( 0.0 ), 
m_mouselb( false ), 
m_xmouse( 0 ), 
m_ymouse( 0 )
{    
    _INIT_LOGGER( "gui.conf" )  
    m_w_title = "gui test";

    m_mousein_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mousein ) );
    m_mouseout_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mouseout ) );
    m_mouseleftbuttondown_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mouseleftbuttondown ) );
    m_mouseleftbuttonup_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mouseleftbuttonup ) );
    m_mouserightbuttondown_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mouserightbuttondown ) );
    m_mouserightbuttonup_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mouserightbuttonup ) );

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
   



    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    static long last_fps;

    long current_fps = m_timer.GetFPS();
    char fps[256];
    sprintf( fps, "%d fps", m_timer.GetFPS() );

    
    if( last_fps != current_fps )
    {
        m_text_widget->SetText( 5, 120, 40, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );

        m_fpstext_widget->SetText( 0, 0, 70, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
        last_fps = current_fps;		
    }
    

    m_image1->SetVirtualTranslation( m_xmouse, m_ymouse );

    
    m_fpstext_widget->SetVirtualTranslation( 15, 5 );
    m_fpstext_widget->Transform();

    
    m_text_widget->SetVirtualTranslation( 160, 110 );
    
    m_text_widget_2->SetVirtualTranslation( 10, 120 );

    m_text_widget_3->SetVirtualTranslation( 100, 100 );

    m_text_widget->Transform();

    m_text_widget_3->Transform();
    

    m_texturepass->GetRenderingQueue()->Draw();


    
    m_text_widget->Draw();
    
    m_fpstext_widget->Draw();
   
    m_text_widget_3->Draw();
    

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

    RegisterMouseInputEventsProvider( &m_mouse_input );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
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
    
    

    ///////////////////////////////////////////////////////////////

    m_scenegraph.RegisterPass( m_texturepass );

    ///////////////////////////////////////////////////////////////


    
    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->RegisterPassSlot( "texture_pass" );
    m_chunk->SetRenderer( renderer );
    m_chunk->SetName( "cube" );

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );
    m_chunk->GetMeshe( "" )->SetImporter( m_meshe_import );
    m_chunk->GetMeshe( "" )->LoadFromFile( "object.ac", 0 );

    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_chunk->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_chunk->GetNodeFromPass( "texture_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( m_chunk );


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

    //////////////////////////////////////////////////////////////////////


    m_text_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget", 25, 15, m_font, true, NULL ) );

    m_text_widget->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_text_widget->GetBackgroundImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetBackgroundImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget->GetBackgroundImage()->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "bellerophon.jpg" ) ), 0 );
    m_text_widget->GetBackgroundImage()->GetTexture( 0 )->LoadFromFile();


  
    m_text_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_text_widget->GetImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetImage()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_text_widget->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_text_widget->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_text_widget->GetText()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetText()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_text_widget->GetText()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_text_widget->GetText()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );


    m_text_widget->GetText()->AddShaderParameter( 1, "color", 0 );
    m_text_widget->GetText()->SetShaderRealVector( "color", Utils::Vector( 1.0, 1.0, 0.0, 1.0 ) );

    m_text_widget->GetImage()->SetOrderNumber( 20000 );
    m_text_widget->GetInternalPass()->GetRenderingQueue()->UpdateOutputQueue();

    m_text_widget->RegisterToPass( m_finalpass );
    

    m_text_widget->RegisterMouseInEventHandler( m_mousein_eventhandler );
    m_text_widget->RegisterMouseOutEventHandler( m_mouseout_eventhandler );
    m_text_widget->RegisterMouseLeftButtonDownEventHandler( m_mouseleftbuttondown_eventhandler );
    m_text_widget->RegisterMouseLeftButtonUpEventHandler( m_mouseleftbuttonup_eventhandler );
    m_text_widget->RegisterMouseRightButtonDownEventHandler( m_mouserightbuttondown_eventhandler );
    m_text_widget->RegisterMouseRightButtonUpEventHandler( m_mouserightbuttonup_eventhandler );

    //////////////////////////////////////////////////////////////////////

    m_text_widget_2 = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget_2", 50, 50, m_font, true, m_text_widget ) );

    m_text_widget_2->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget_2->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_text_widget_2->GetBackgroundImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_2->GetBackgroundImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget_2->GetBackgroundImage()->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "shelby.jpg" ) ), 0 );
    m_text_widget_2->GetBackgroundImage()->GetTexture( 0 )->LoadFromFile();

    m_text_widget_2->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget_2->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_text_widget_2->GetImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_2->GetImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget_2->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_text_widget_2->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_text_widget_2->GetText()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_2->GetText()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_text_widget_2->GetText()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_text_widget_2->GetText()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_text_widget_2->GetText()->AddShaderParameter( 1, "color", 0 );
    m_text_widget_2->GetText()->SetShaderRealVector( "color", Utils::Vector( 1.0, 1.0, 0.0, 1.0 ) );


    m_text_widget_2->GetImage()->SetOrderNumber( 20000 );
    m_text_widget_2->GetInternalPass()->GetRenderingQueue()->UpdateOutputQueue();

    m_text_widget_2->RegisterToPass( m_finalpass );
    

    m_text_widget_2->RegisterMouseInEventHandler( m_mousein_eventhandler );
    m_text_widget_2->RegisterMouseOutEventHandler( m_mouseout_eventhandler );
    m_text_widget_2->RegisterMouseLeftButtonDownEventHandler( m_mouseleftbuttondown_eventhandler );
    m_text_widget_2->RegisterMouseLeftButtonUpEventHandler( m_mouseleftbuttonup_eventhandler );
    m_text_widget_2->RegisterMouseRightButtonDownEventHandler( m_mouserightbuttondown_eventhandler );
    m_text_widget_2->RegisterMouseRightButtonUpEventHandler( m_mouserightbuttonup_eventhandler );



    //////////////////////////////////////////////////////////////////////

    m_text_widget_3 = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget_3", 70, 50, m_font, false, NULL ) );

    m_text_widget_3->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_text_widget_3->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_text_widget_3->GetImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_3->GetImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget_3->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_text_widget_3->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_text_widget_3->GetText()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_3->GetText()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_text_widget_3->GetText()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_text_widget_3->GetText()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_text_widget_3->GetText()->AddShaderParameter( 1, "color", 0 );
    m_text_widget_3->GetText()->SetShaderRealVector( "color", Utils::Vector( 1.0, 1.0, 1.0, 1.0 ) );

    m_text_widget_3->GetImage()->SetOrderNumber( 20000 );
    m_text_widget_3->GetInternalPass()->GetRenderingQueue()->UpdateOutputQueue();
    m_text_widget_3->RegisterToPass( m_finalpass );


    
    //////////////////////////////////////////////////////////////////////

    m_fpstext_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( "fps_text_widget", 20, 10, m_font, false, NULL ) );

    m_fpstext_widget->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_fpstext_widget->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_fpstext_widget->GetText()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_fpstext_widget->GetText()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_fpstext_widget->GetText()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_fpstext_widget->GetText()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_fpstext_widget->GetText()->AddShaderParameter( 1, "color", 0 );
    m_fpstext_widget->GetText()->SetShaderRealVector( "color", Utils::Vector( 0.0, 1.0, 0.0, 1.0 ) );

    m_fpstext_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_fpstext_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_fpstext_widget->GetImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_fpstext_widget->GetImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_fpstext_widget->GetImage()->SetOrderNumber( 20000 );

    m_fpstext_widget->GetInternalPass()->GetRenderingQueue()->UpdateOutputQueue();


    m_fpstext_widget->RegisterToPass( m_finalpass );
    


    //////////////////////////////////////////////////////////////////
    
    m_image1 = _DRAWSPACE_NEW_( Image, Image( (long)8, (long)8 ) );

    m_image1->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texturekeyed.vsh", false ) ) );
    m_image1->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texturekeyed.psh", false ) ) );
    m_image1->GetFx()->GetShader( 0 )->LoadFromFile();
    m_image1->GetFx()->GetShader( 1 )->LoadFromFile();


    m_image1->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "money.bmp" ) ), 0 );
    m_image1->GetTexture( 0 )->LoadFromFile();

    m_image1->SetOrderNumber( 20000 );



    m_finalpass->GetRenderingQueue()->Add( m_image1 );



    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();



    m_mouse_input.RegisterImage( m_image1 );
    m_mouse_input.RegisterWidget( m_text_widget );

    return true;
}

void dsAppClient::on_mousein( DrawSpace::Gui::Widget* p_widget )
{
    dsstring widget_name;
    p_widget->GetName( widget_name );
    m_text_widget_3->SetText( 0, 0, 12, dsstring( "mouse in event on widget " ) + widget_name , DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering ); 
}

void dsAppClient::on_mouseout( DrawSpace::Gui::Widget* p_widget )
{
    dsstring widget_name;
    p_widget->GetName( widget_name );
    m_text_widget_3->SetText( 0, 0, 12, dsstring( "mouse out event on widget " ) + widget_name , DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering ); 
}

void dsAppClient::on_mouseleftbuttondown( DrawSpace::Gui::Widget* p_widget )
{
    dsstring widget_name;
    p_widget->GetName( widget_name );
    m_text_widget_3->SetText( 0, 0, 12, dsstring( "mouse left down event on widget " ) + widget_name , DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering ); 
}

void dsAppClient::on_mouseleftbuttonup( DrawSpace::Gui::Widget* p_widget )
{
    dsstring widget_name;
    p_widget->GetName( widget_name );
    m_text_widget_3->SetText( 0, 0, 12, dsstring( "mouse left up event on widget " ) + widget_name , DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering ); 

}

void dsAppClient::on_mouserightbuttondown( DrawSpace::Gui::Widget* p_widget )
{
    dsstring widget_name;
    p_widget->GetName( widget_name );
    m_text_widget_3->SetText( 0, 0, 12, dsstring( "mouse right down event on widget " ) + widget_name , DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering ); 
}

void dsAppClient::on_mouserightbuttonup( DrawSpace::Gui::Widget* p_widget )
{
    dsstring widget_name;
    p_widget->GetName( widget_name );
    m_text_widget_3->SetText( 0, 0, 12, dsstring( "mouse right up event on widget " ) + widget_name , DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering ); 
}


void dsAppClient::OnAppInit( void )
{

}

void dsAppClient::OnClose( void )
{
    DrawSpace::Utils::MemAlloc::GetInstance()->DumpContent();
    DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface->DumpMemoryAllocs();
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


    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

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
