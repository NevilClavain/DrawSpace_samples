#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Gui;

dsAppClient* dsAppClient::m_instance = NULL;



dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false )
{    
    _INIT_LOGGER( "loggpunoise2.conf" )  
    m_w_title = "gpu perlin noise test 2";
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{


    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_scenenodegraph.ComputeTransformations( m_timer );



    m_texturepass->GetRenderingQueue()->Draw();
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


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );
    m_texturepass->SetTargetDimsFromRenderer( false );
    m_texturepass->SetTargetDims( 64, 64 );
    m_texturepass->SetRenderPurpose( Texture::RENDERPURPOSE_FLOAT32 );
    m_texturepass->Initialize();
    m_texturepass->CreateViewportQuad();
    
    m_texturepass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_texturepass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "noise2.vsh", false ) ) );
    m_texturepass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "noise2.psh", false ) ) );
    m_texturepass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_texturepass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_texturepass->GetViewportQuad()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_texturepass->GetViewportQuad()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );


    m_fractal = new Fractal( 3, 3744567, 0.5, 2.0 );

    m_texturepass->GetViewportQuad()->AddShaderParameter( 0, "flags", 27 );
    double lacunarity = m_fractal->GetLacunarity();
    m_texturepass->GetViewportQuad()->SetShaderRealVector( "flags", Vector( lacunarity, 9.0, -10.0, 10.0 ) );
    


    m_perlinnoisebuffer_texture = new Texture();    
    m_perlinnoisebuffer_texture->SetFormat( 256, 3, 4 );
    m_perlinnoisebuffer_texture->SetPurpose( Texture::PURPOSE_FLOAT );


    m_perlinnoisemap_texture = new Texture();
    m_perlinnoisemap_texture->SetFormat( 256, 1, 4 );
    //m_perlinnoisemap_texture->SetPurpose( Texture::PURPOSE_COLOR );
    m_perlinnoisemap_texture->SetPurpose( Texture::PURPOSE_FLOAT );


    m_fbmexp_texture = new Texture();    
    m_fbmexp_texture->SetFormat( Fractal::MaxOctaves, 1, 4 );
    m_fbmexp_texture->SetPurpose( Texture::PURPOSE_FLOAT );


    m_texturepass->GetViewportQuad()->SetVertexTexture( m_perlinnoisebuffer_texture, 0 );
    m_texturepass->GetViewportQuad()->SetVertexTexture( m_perlinnoisemap_texture, 1 );
    m_texturepass->GetViewportQuad()->SetVertexTexture( m_fbmexp_texture, 2 );


   
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
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );


       
    /////////////////////////////////////////////////////////////////




    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();

    m_texturepass->GetTargetTexture()->AllocTextureContent();
    m_texture_content = m_texturepass->GetTargetTexture()->GetTextureContentPtr();


    m_perlinnoisebuffer_texture->AllocTextureContent();
    m_pnbufftexture_content = m_perlinnoisebuffer_texture->GetTextureContentPtr();

    m_perlinnoisemap_texture->AllocTextureContent();
    m_pnmaptexture_content = m_perlinnoisemap_texture->GetTextureContentPtr();

    m_fbmexp_texture->AllocTextureContent();
    m_fbmexptexture_content = m_fbmexp_texture->GetTextureContentPtr();
    
    //unsigned char* color_ptr = (unsigned char*)m_pnmaptexture_content;
    float* float_ptr = (float*)m_pnbufftexture_content;

        
    for(long j = 0; j < 3; j++ )
    {
        for( long i = 0; i < 256; i++ )    
        {
            float temp = m_fractal->GetNBuffer( i, j );
            *float_ptr = temp; float_ptr++;
        }
    }

    float_ptr = (float*)m_pnmaptexture_content;
    for( long i = 0; i < 256; i++ )
    {
        float temp = m_fractal->GetNMap( i );
        *float_ptr = temp; float_ptr++;
    }

    
    float_ptr = (float*)m_fbmexptexture_content;

    for( long i = 0; i < Fractal::MaxOctaves; i++ )
    {
        float temp = m_fractal->GetExponent( i );
        *float_ptr = temp; float_ptr++;
    }
    

    m_perlinnoisemap_texture->UpdateTextureContent();
    m_perlinnoisebuffer_texture->UpdateTextureContent();
    m_fbmexp_texture->UpdateTextureContent();
    
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
            {

                Utils::Vector finput;

                /*
                finput[0] = 0.0231;
                finput[1] = 0.777;
                finput[2] = -2.25;
                finput[3] = 0.0;
                */

                finput[0] = 3.031415;
                finput[1] = 0.97;
                finput[2] = -1.0025;
                finput[3] = 0.0;

                //double val1 = m_fractal->GetNoise( finput.GetArray() );
                double val1 = 12000.0 * m_fractal->fBm( finput.GetArray(), 9 );

                m_texturepass->GetTargetTexture()->CopyTextureContent();
                float* ptr = (float*)m_texture_content;
                float val = *ptr;
                _asm nop
            }
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
