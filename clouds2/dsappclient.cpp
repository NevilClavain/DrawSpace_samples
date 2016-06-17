/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/

#include "dsappclient.h"
#include <algorithm>

//#include <random>

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_speed( 0.0 ), m_speed_speed( 5.0 ), m_current_camera( NULL ), m_selected_camera( 0 )
{    
    _INIT_LOGGER( "logclouds.conf" )  
    m_w_title = "clouds test";           
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{
    Matrix cam2_pos;
    cam2_pos.Translation( 0.0, 1.0, 0.0 );
   
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
           
    m_scenenodegraph.ComputeTransformations( m_timer );



   

    
    m_texturepass->GetRenderingQueue()->Draw();
    m_texturepass2->GetRenderingQueue()->Draw();
    m_maskpass->GetRenderingQueue()->Draw();
    m_filterpass->GetRenderingQueue()->Draw();
    
    m_finalpass->GetRenderingQueue()->Draw();    

    long current_fps = m_timer.GetFPS();
    renderer->DrawText( 0, 0, 0, 10, 35, "%d", current_fps );

    renderer->FlipScreen();



    m_calendar->Run();

    m_freemove.SetSpeed( m_speed );

       
}






bool dsAppClient::OnIdleAppInit( void )
{
    /////////////////////////////////////
           
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );
    m_texturepass->Initialize();

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 145, 230, 230, 255 );




    m_texturepass2 = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass2" ) );

    m_texturepass2->Initialize();
    m_texturepass2->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass2->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass2->GetRenderingQueue()->SetTargetClearingColor( 145, 230, 230, 255 );


    m_maskpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "mask_pass" ) );

    m_maskpass->Initialize();
    m_maskpass->GetRenderingQueue()->EnableDepthClearing( true );
    m_maskpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_maskpass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 0 );



    m_filterpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "filter_pass" ) );
    m_filterpass->Initialize();
    m_filterpass->CreateViewportQuad();
    

    m_filterpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_filterpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "clouds_final.vsh", false ) ) );
    m_filterpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "clouds_final.psh", false ) ) );
    m_filterpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_filterpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_filterpass->GetViewportQuad()->SetTexture( m_maskpass->GetTargetTexture(), 0 );
    m_filterpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 1 );
    m_filterpass->GetViewportQuad()->SetTexture( m_texturepass2->GetTargetTexture(), 2 );



    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_filterpass->GetTargetTexture(), 0 );


    m_ground = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ground->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    m_ground->RegisterPassSlot( m_texturepass );
    m_ground->RegisterPassSlot( m_texturepass2 );
    m_ground->RegisterPassSlot( m_maskpass );


    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    m_ground->GetMeshe()->SetImporter( m_meshe_import );

    m_ground->GetMeshe()->LoadFromFile( "terran.ac", 0 );
    

    m_ground->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    

    m_ground->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "mapcolor.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();



    m_ground->GetNodeFromPass( m_texturepass2 )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    

    m_ground->GetNodeFromPass( m_texturepass2 )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "mapcolor.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetTexture( 0 )->LoadFromFile();



    m_ground->GetNodeFromPass( m_maskpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.psh", false ) ) );
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );    
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    m_ground->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "color", 0 );
    m_ground->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "color", Vector( 0.0, 0.0, 0.0, 0.0 ) );



    m_ground_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "ground" ) );
    m_ground_node->SetContent( m_ground );

    m_scenenodegraph.AddNode( m_ground_node );
    m_scenenodegraph.RegisterNode( m_ground_node );

  

    m_clouds_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "impostor2_transfo" ) );
    m_clouds_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

    Matrix clouds_pos;
    clouds_pos.Translation( 6.0, 1200.0, -450.0 );

    m_clouds_transfo_node->GetContent()->PushMatrix( clouds_pos );

    m_scenenodegraph.AddNode( m_clouds_transfo_node );
    m_scenenodegraph.RegisterNode( m_clouds_transfo_node );



    m_clouds = _DRAWSPACE_NEW_( DrawSpace::Clouds, DrawSpace::Clouds );
    m_clouds->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_clouds->EnableDetails( true );


    /////////////////////////////////////////////////
    m_procedural_rules = new DrawSpace::Procedural::RulesPackage( m_clouds->GetProceduralCallback() );

    m_procedural_rules->InitializeSeedBase( 56645 );
    m_procedural_rules->Run( "clouds.rules", " " );

    m_procedural_rules->GetRootParser()->GetRules()->Apply();




    /////////////////////////////////////////////////
    

    m_clouds->ImpostorsInit();
    



    m_clouds->RegisterPassSlot( m_texturepass2 );
    m_clouds->RegisterPassSlot( m_maskpass );

    
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.vso", true ) ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.pso", true ) ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );
    

    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    

    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );





    m_clouds->GetNodeFromPass( m_texturepass2 )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "clouds.bmp" ) ), 0 );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetTexture( 0 )->LoadFromFile();

    
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetOrderNumber( 12000 );


    m_clouds->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 0, "flags_v", 24 );
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "flags_v", Vector( 0.5, 0.0, 0.0, 0.0 ) );

    m_clouds->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 0, "clouds_dims", 25 );
    //m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "clouds_dims", Vector( 900, -500, 1.0, 0.66 ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "clouds_dims", Vector( 400, -200, 1.0, 0.8 ) );



    m_clouds->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 1, "flags", 0 );
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "flags", Vector( 0.5, 0.0, 0.0, 0.0 ) );

    m_clouds->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 1, "color", 1 );
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "color", Vector( 0.99, 0.99, 0.99, 1.0 ) );
    


    m_clouds->GetNodeFromPass( m_maskpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.vso", true ) ) );
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.pso", true ) ) );
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    


    
    m_clouds->GetNodeFromPass( m_maskpass )->SetOrderNumber( 12000 );


    m_clouds->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "flags", 0 );
    m_clouds->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "flags", Vector( 1.0, 0.0, 0.0, 0.0 ) );

    m_clouds->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "color", 1 );
    m_clouds->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );



    m_clouds_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Clouds>, SceneNode<DrawSpace::Clouds>( "clouds_1" ) );

    m_clouds_node->SetContent( m_clouds );


    m_scenenodegraph.RegisterNode( m_clouds_node );

    m_clouds_node->LinkTo( m_clouds_transfo_node );




    //////////////////////////////////////////////////////////////


    m_world.Initialize();
    
    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();





    //////////////////////////////////////////////


    ////////////////////////////////////////////////////

  

    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );
    m_calendar->RegisterWorld( &m_world );


    //////////////////////////////////////////////////////////////





    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );

    
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );
    m_scenenodegraph.RegisterNode( m_camera_node );


    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 5.0, 0.0, 1.0 ) );

    m_freemove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FreeMovement>, SceneNode<DrawSpace::Core::FreeMovement>( "free_node" ) );
    m_freemove_node->SetContent( &m_freemove );

    m_scenenodegraph.AddNode( m_freemove_node );
    m_scenenodegraph.RegisterNode( m_freemove_node );
    
    m_camera_node->LinkTo( m_freemove_node );



    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );

    
    m_camera2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera2" ) );
    m_camera2_node->SetContent( m_camera2 );
    m_scenenodegraph.RegisterNode( m_camera2_node );



    m_camera2_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "camera2_transfo" ) );
    m_camera2_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

    Matrix cam2_pos;
    cam2_pos.Translation( 20000.0, 9000.0, -60000.0 );

    m_camera2_transfo_node->GetContent()->PushMatrix( cam2_pos );

    m_scenenodegraph.AddNode( m_camera2_transfo_node );
    m_scenenodegraph.RegisterNode( m_camera2_transfo_node );






    m_camera2_node->LinkTo( m_camera2_transfo_node );



    m_camera2->Lock( m_clouds_node );

    m_scenenodegraph.SetCurrentCamera( "camera" );


    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass2->GetRenderingQueue()->UpdateOutputQueue();
    m_maskpass->GetRenderingQueue()->UpdateOutputQueue();
    m_filterpass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();




    m_mouse_circularmode = true;



    m_calendar->Startup( 0 );


        
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

        
            break;

        case VK_F2:
            {              
            }        
            break;

        case VK_F3:

            if( 0 == m_selected_camera )
            {
                m_scenenodegraph.SetCurrentCamera( "camera2" );
                m_selected_camera = 1;
            }
            else
            {
                m_scenenodegraph.SetCurrentCamera( "camera" );
                m_selected_camera = 0;
            }
            break;

        case VK_F4:

            break;

        case VK_F5:


            break;

        case VK_F6:


            break;

        case VK_F7:

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
