#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Gui;

dsAppClient* dsAppClient::m_instance = NULL;



dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false )
{    
    _INIT_LOGGER( "logcpunoise.conf" )  
    m_w_title = "cpu perlin noise test";
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
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    

       


    m_output_texture = new Texture();
    m_output_texture->SetFormat( OUTPUT_TEXTURE_SIZE, OUTPUT_TEXTURE_SIZE, 4 );
    m_output_texture->SetPurpose( Texture::PURPOSE_COLOR );


    m_finalpass->GetViewportQuad()->SetTexture( m_output_texture, 0 );


    /////////////////////////////////////////////////////////////////


    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();

    m_output_texture->AllocTextureContent();
    m_outputtexture_content = m_output_texture->GetTextureContentPtr();

    m_fractal = new Fractal( 3, 290001, 0.65, 1.29 );
    
    unsigned char* color_ptr = (unsigned char*)m_outputtexture_content;

    /*
    for( int i = 0; i < OUTPUT_TEXTURE_SIZE * OUTPUT_TEXTURE_SIZE; i++ )
    {
        *color_ptr = 0; color_ptr++; // B
        *color_ptr = 0; color_ptr++; // G
        *color_ptr = 0; color_ptr++; // R
        *color_ptr = 0; color_ptr++; // A
    }
    */

    for( int y = 0; y < OUTPUT_TEXTURE_SIZE; y++ )
    {
        for( int x = 0; x < OUTPUT_TEXTURE_SIZE; x++ )
        {
            double f_array[3];
            unsigned char color;

            f_array[0] = ( (double)x / OUTPUT_TEXTURE_SIZE ) * 4.0; 
            f_array[1] = ( (double)y / OUTPUT_TEXTURE_SIZE ) * 4.0;
            f_array[2] = 1.0;

            //double res = m_fractal->GetNoise( f_array );

            double res = m_fractal->fBm( f_array, 15.0 );

            if( res >= 0.2 )
            {
                color = 255.0 * ( ( res * 0.5 ) + 0.5 );
                *color_ptr = color; color_ptr++;
                *color_ptr = color; color_ptr++;
                *color_ptr = color; color_ptr++;
                *color_ptr = color; color_ptr++;

            }
            
            else
            {
                *color_ptr = 100; color_ptr++;
                *color_ptr = 0; color_ptr++;
                *color_ptr = 0; color_ptr++;
                *color_ptr = 0; color_ptr++;                
            }
            
        }    
    }

    m_output_texture->UpdateTextureContent();
    
    
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
