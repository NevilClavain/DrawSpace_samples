#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Gui;

dsAppClient* dsAppClient::m_instance = NULL;



dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false )
{    
    _INIT_LOGGER( "loggpunoise.conf" )  
    m_w_title = "gpu perlin noise test";
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{


    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_scenenodegraph.ComputeTransformations( m_timer );




    m_finalpass->GetRenderingQueue()->Draw();


    renderer->DrawText( 255, 0, 0, 10, 20, "%d fps", m_timer.GetFPS() );

    renderer->FlipScreen();

    m_timer.Update();
    if( m_timer.IsReady() )
    {
    }
}

bool dsAppClient::OnIdleAppInit( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
   
    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "noise.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "noise.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    

       


    m_perlinnoisebuffer_texture = new Texture();
    //m_perlinnoisebuffer_texture->SetFormat( 3, 256, 4 );
    m_perlinnoisebuffer_texture->SetFormat( 256, 3, 4 );
    m_perlinnoisebuffer_texture->SetPurpose( Texture::PURPOSE_FLOAT );


    m_perlinnoisemap_texture = new Texture();
    m_perlinnoisemap_texture->SetFormat( 256, 1, 4 );
    m_perlinnoisemap_texture->SetPurpose( Texture::PURPOSE_COLOR );

    /*
    m_finalpass->GetViewportQuad()->SetVertexTexture( m_perlinnoisebuffer_texture, 0 );
    m_finalpass->GetViewportQuad()->SetVertexTexture( m_perlinnoisemap_texture, 1 );
    */

    m_finalpass->GetViewportQuad()->SetTexture( m_perlinnoisebuffer_texture, 0 );
    m_finalpass->GetViewportQuad()->SetTexture( m_perlinnoisemap_texture, 1 );


    /////////////////////////////////////////////////////////////////


    m_mouse_circularmode = true;


    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();

    m_perlinnoisebuffer_texture->AllocTextureContent();
    m_pnbufftexture_content = m_perlinnoisebuffer_texture->GetTextureContentPtr();


    m_perlinnoisemap_texture->AllocTextureContent();
    m_pnmaptexture_content = m_perlinnoisemap_texture->GetTextureContentPtr();

    m_fractal = new CFractal( 3, 87699, 0.5, 2.0 );
    
    unsigned char* color_ptr = (unsigned char*)m_pnmaptexture_content;
    float* float_ptr = (float*)m_pnbufftexture_content;

        
    for(long j = 0; j < 3; j++ )
    {
        for( long i = 0; i < 256; i++ )    
        {
            float temp = m_fractal->GetNBuffer( i, j );
            *float_ptr = temp; float_ptr++;
        }
    }

    for( long i = 0; i < 256; i++ )
    {
        *color_ptr = m_fractal->GetNMap( i ); color_ptr++;
        *color_ptr = m_fractal->GetNMap( i ); color_ptr++;
        *color_ptr = m_fractal->GetNMap( i ); color_ptr++;
        *color_ptr = m_fractal->GetNMap( i ); color_ptr++;
    }

    m_perlinnoisemap_texture->UpdateTextureContent();
    m_perlinnoisebuffer_texture->UpdateTextureContent();
    
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

            break;

        case 'W':

            break;

        case VK_F3:

            break;

    }

}

void dsAppClient::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':
        case 'W':

            break;      
    }

}

void dsAppClient::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:

            break;

        case VK_F2:
            break;
    }
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( m_mouserb )
    {
    }

    else
    {
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
