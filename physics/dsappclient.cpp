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



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_box_count( 0 ), m_box_texture( 0 ), m_fpsmove( true ), m_zoom( 1.0 )
{    
    _INIT_LOGGER( "logphysics.conf" )  
    m_w_title = "physics test";    
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{

    if( m_linear_mvt->GetTranslationLength() < 1.0 )
    {
        m_linear_mvt->SetSpeed( 2.5 );
    }
    else if( m_linear_mvt->GetTranslationLength() > 58.0 )
    {
        m_linear_mvt->SetSpeed( -2.5 );
    }

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    m_scenenodegraph.ComputeTransformations( m_timer );


    long current_fps = m_timer.GetFPS();
    char fps[256];
    sprintf( fps, "%d fps", m_timer.GetFPS() );
    
    renderer->DrawText( 255, 0, 0, 30, 20, "%s", fps );

    m_texturepass->GetRenderingQueue()->Draw();
    m_texturepass2->GetRenderingQueue()->Draw();
    m_maskpass->GetRenderingQueue()->Draw();
    m_filterpass->GetRenderingQueue()->Draw();

    m_finalpass->GetRenderingQueue()->Draw();
    m_finalpass2->GetRenderingQueue()->Draw();


    dsstring date;
    m_calendar->GetFormatedDate( date );

    renderer->DrawText( 255, 0, 0, 30, 50, "%d %d", m_texturepass->GetRenderingQueue()->GetSwitchesCost(), m_texturepass->GetRenderingQueue()->GetTheoricalSwitchesCost() );

    renderer->DrawText( 0, 255, 0, 10, 85, "%s", date.c_str() );
    renderer->DrawText( 255, 0, 0, 10, 70, "%s", m_current_camera.c_str() );

  
    ///////////////////////////////////////////

    Matrix cube0_pos;
    Vector cube0_pos_v;
    dsreal center_x, center_y;
    dsreal out_z;

    
    m_boxes[0].inert_body->GetLastWorldTransformation( cube0_pos );

    cube0_pos_v[0] = cube0_pos( 3, 0 );
    cube0_pos_v[1] = cube0_pos( 3, 1 );
    cube0_pos_v[2] = cube0_pos( 3, 2 );
    cube0_pos_v[3] = 1.0;

    m_scenenodegraph.PointProjection( cube0_pos_v, center_x, center_y, out_z );

    m_image1->SetTranslation( center_x, center_y );

    renderer->DrawText( 0, 255, 0, 10, 105, "%f %f", center_x, center_y );
    

    ///////////////////////////////////////////


    renderer->FlipScreen();


    m_calendar->Run();
}

void dsAppClient::create_box( void )
{
    
    Chunk* chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>* chunk_node;

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    //drawable = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    chunk->RegisterPassSlot( m_texturepass );
    chunk->RegisterPassSlot( m_maskpass );

    char name[32];
    sprintf( name, "box_%d", m_box_count );

    char namebody[32];
    sprintf( namebody, "body_box_%d", m_box_count );



    
    chunk->GetMeshe()->SetImporter( m_meshe_import );

    chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );    

    chunk->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    chunk->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet chunk_rss;

    chunk_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    chunk_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    chunk_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    chunk_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    chunk->GetNodeFromPass( m_texturepass )->GetFx()->SetRenderStates( chunk_rss );


    RenderStatesSet chunk_mask_rss;

    chunk->GetNodeFromPass( m_maskpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    chunk->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.vsh", false ) ) );
    chunk->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.psh", false ) ) );
    chunk->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    chunk->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    chunk_mask_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );    
    chunk_mask_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    chunk->GetNodeFromPass( m_maskpass )->GetFx()->SetRenderStates( chunk_mask_rss );

    chunk->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "color", 0 );
    chunk->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "color", Vector( 0.0, 0.0, 0.0, 0.0 ) );




    if( 0 == m_box_texture )
    {
        chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "shelby.jpg" ) ), 0 );
        m_box_texture = 1;
    }
    else
    {
        chunk->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );
        m_box_texture = 0;
    }
    

    chunk->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    chunk_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( name ) );
    chunk_node->SetContent( chunk );
    m_scenenodegraph.RegisterNode( chunk_node );


    DrawSpace::Dynamics::InertBody::Parameters cube_params;

    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );
    cube_params.initial_attitude.Translation( 0.0, 10.5, -25.0 );


    DrawSpace::Dynamics::InertBody* cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, cube_params ) );

    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>* cube_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( namebody ) );
    cube_body_node->SetContent( cube_body );

    m_scenenodegraph.AddNode( cube_body_node );
    m_scenenodegraph.RegisterNode( cube_body_node );

    chunk_node->LinkTo( cube_body_node );




    Box box;

    box.drawable = chunk;
    box.drawable_node = chunk_node;
    box.inert_body = cube_body;
    box.inert_body_node = cube_body_node;

    m_boxes.push_back( box );

    m_box_count++;


    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_maskpass->GetRenderingQueue()->UpdateOutputQueue();

    
}

bool dsAppClient::OnIdleAppInit( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->Initialize();
    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 0 );



    m_texturepass2 = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass2" ) );

    m_texturepass2->Initialize();
    m_texturepass2->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass2->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass2->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 0 );


    m_maskpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "mask_pass" ) );

    m_maskpass->Initialize();
    m_maskpass->GetRenderingQueue()->EnableDepthClearing( true );
    m_maskpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_maskpass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 0 );



    m_filterpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "filter_pass" ) );
    m_filterpass->Initialize();
    m_filterpass->CreateViewportQuad();
    

    m_filterpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_filterpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "impostors_filter.vsh", false ) ) );
    m_filterpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "impostors_filter.psh", false ) ) );
    m_filterpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_filterpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_filterpass->GetViewportQuad()->SetTexture( m_maskpass->GetTargetTexture(), 0 );
    m_filterpass->GetViewportQuad()->SetTexture( m_texturepass2->GetTargetTexture(), 1 );




    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );



    m_finalpass2 = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass2->Initialize();
    m_finalpass2->CreateViewportQuad();
    m_finalpass2->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_finalpass2->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass2->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass2->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass2->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet finalpass_rss;

    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one" ) );
    finalpass_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );


    finalpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    finalpass_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    
    m_finalpass2->GetViewportQuad()->GetFx()->SetRenderStates( finalpass_rss );
    
    m_finalpass2->GetViewportQuad()->SetTexture( m_filterpass->GetTargetTexture(), 0 );




    //////////////////////////////////////////////////////////////


    m_ground = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ground->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    m_ground->RegisterPassSlot( m_texturepass );


    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    m_ground->GetMeshe()->SetImporter( m_meshe_import );

    m_ground->GetMeshe()->LoadFromFile( "grid.ac", 0 );
    

    m_ground->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    RenderStatesSet ground_rss;

    ground_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    
    ground_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    

    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->SetRenderStates( ground_rss );

    m_ground->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "ground.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_ground_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "ground" ) );
    m_ground_node->SetContent( m_ground );

    m_scenenodegraph.RegisterNode( m_ground_node );




    ////////////////////////////////////////////////////////////////

       
    m_spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
    m_spacebox->RegisterPassSlot( m_texturepass );




    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, m_texturepass );
    
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetOrderNumber( 200 );


    
    m_spacebox_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Spacebox>, SceneNode<DrawSpace::Spacebox>( "spacebox" ) );
    m_spacebox_node->SetContent( m_spacebox );

    
    m_scenenodegraph.RegisterNode( m_spacebox_node );


    m_spacebox_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "spacebox_transfo" ) );
    m_spacebox_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix spacebox_scale;
    spacebox_scale.Scale( 20.0, 20.0, 20.0 );
    m_spacebox_transfo_node->GetContent()->PushMatrix( spacebox_scale );

    m_scenenodegraph.AddNode( m_spacebox_transfo_node );
    m_scenenodegraph.RegisterNode( m_spacebox_transfo_node );
    m_spacebox_node->LinkTo( m_spacebox_transfo_node );

    



    //////////////////////////////////////////////////////////////

    
    m_image1 = _DRAWSPACE_NEW_( Image, Image( (long)8, (long)8 ) );

    RenderStatesSet image_rss;

    m_image1->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_image1->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_image1->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    image_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    image_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    image_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    image_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one"  ) );
    image_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    image_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_image1->GetFx()->SetRenderStates( image_rss );

    m_image1->GetFx()->GetShader( 0 )->LoadFromFile();
    m_image1->GetFx()->GetShader( 1 )->LoadFromFile();


    m_image1->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "reticle.bmp" ) ), 0 );
    m_image1->GetTexture( 0 )->LoadFromFile();

    m_image1->SetOrderNumber( 20000 );
    


    m_finalpass->GetRenderingQueue()->Add( m_image1 );



    m_world.Initialize();
    m_world.SetGravity( DrawSpace::Utils::Vector( 0.0, -9.81, 0.0, 0.0 ) );





    m_impostor_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "impostor_transfo" ) );
    m_impostor_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

    Matrix impostor_pos;
    impostor_pos.Translation( -3.0, 5.4, -24.0 );

    m_impostor_transfo_node->GetContent()->PushMatrix( impostor_pos );

    m_scenenodegraph.AddNode( m_impostor_transfo_node );
    m_scenenodegraph.RegisterNode( m_impostor_transfo_node );
    


    m_impostor = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );
    m_impostor->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    ImpostorsDisplayList idl;
    ImpostorsDisplayListEntry idle;

    for( long j = 0; j < 1; j++ )
    {
        for( long i = 0; i < 2; i++ )
        {
            idle.width_scale = 0.5;
            idle.height_scale = 0.5;

            idle.u1 = 0.0;
            idle.v1 = 0.0;
            idle.u2 = 1.0;
            idle.v2 = 0.0;
            idle.u3 = 1.0;
            idle.v3 = 1.0;
            idle.u4 = 0.0;
            idle.v4 = 1.0;

            idle.localpos[0] = i * 0.66;
            idle.localpos[1] = j * 0.66;
            idle.localpos[2] = 0.0;
    
            idl.push_back( idle );
        }
    }


    
    m_impostor->SetImpostorsDisplayList( idl );
    m_impostor->ImpostorsInit();

    m_impostor->RegisterPassSlot( m_texturepass );
    m_impostor->RegisterPassSlot( m_maskpass );

    m_impostor->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor.vsh", false ) ) );
    m_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor.psh", false ) ) );
    m_impostor->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_impostor->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();


    RenderStatesSet imp_rss;

    imp_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );    
    imp_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    m_impostor->GetNodeFromPass( m_texturepass )->GetFx()->SetRenderStates( imp_rss );

    m_impostor->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "map.jpg" ) ), 0 );
    m_impostor->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    m_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "globalscale", 24 );
    m_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "globalscale", Vector( 1.0, 1.0, 0.0, 0.0 ) );
    
    m_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "flags", 0 );
    m_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "flags", Vector( 0.0, 0.0, 0.0, 0.0 ) );

    m_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "color", 1 );
    m_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );

    


    m_impostor->GetNodeFromPass( m_maskpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_impostor->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor.vsh", false ) ) );
    m_impostor->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor.psh", false ) ) );
    m_impostor->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_impostor->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet imp_mask_rss;

    imp_mask_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );    
    imp_mask_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    m_impostor->GetNodeFromPass( m_maskpass )->GetFx()->SetRenderStates( imp_mask_rss );

    m_impostor->GetNodeFromPass( m_maskpass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "map.jpg" ) ), 0 );
    m_impostor->GetNodeFromPass( m_maskpass )->GetTexture( 0 )->LoadFromFile();

    m_impostor->GetNodeFromPass( m_maskpass )->AddShaderParameter( 0, "globalscale", 24 );
    m_impostor->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "globalscale", Vector( 1.0, 1.0, 0.0, 0.0 ) );
    
    m_impostor->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "flags", 0 );
    m_impostor->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "flags", Vector( 1.0, 0.0, 0.0, 0.0 ) );

    m_impostor->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "color", 1 );
    m_impostor->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "color", Vector( 0.0, 0.0, 0.0, 1.0 ) );



    m_impostor_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "impostor0" ) );

    m_impostor_node->SetContent( m_impostor );


    m_scenenodegraph.RegisterNode( m_impostor_node );

    m_impostor_node->LinkTo( m_impostor_transfo_node );








    m_impostor2_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "impostor2_transfo" ) );
    m_impostor2_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

    Matrix impostor2_pos;
    impostor2_pos.Translation( 6.0, 3.0, -16.0 );

    m_impostor2_transfo_node->GetContent()->PushMatrix( impostor2_pos );

    m_scenenodegraph.AddNode( m_impostor2_transfo_node );
    m_scenenodegraph.RegisterNode( m_impostor2_transfo_node );



    m_impostor2 = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );
    m_impostor2->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );


    idl.clear();

    for( long j = 0; j < 10; j++ )
    {
        for( long i = 0; i < 10; i++ )
        {
            idle.width_scale = 0.5;
            idle.height_scale = 0.5;

            idle.u1 = 0.0;
            idle.v1 = 0.0;
            idle.u2 = 1.0;
            idle.v2 = 0.0;
            idle.u3 = 1.0;
            idle.v3 = 1.0;
            idle.u4 = 0.0;
            idle.v4 = 1.0;

            idle.localpos[0] = i * 0.66;
            idle.localpos[1] = j * 0.66;
            idle.localpos[2] = 0.0;
    
            idl.push_back( idle );
        }
    }
    


    
    m_impostor2->SetImpostorsDisplayList( idl );
    m_impostor2->ImpostorsInit();

    m_impostor2->RegisterPassSlot( m_texturepass2 );
    m_impostor2->RegisterPassSlot( m_maskpass );

    
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.vso", true ) ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.pso", true ) ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet imp2_rss;

    
    imp2_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    imp2_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    imp2_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    imp2_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one" ) );
    imp2_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );


    imp2_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    imp2_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->SetRenderStates( imp2_rss );


    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "star_far.bmp" ) ), 0 );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetTexture( 0 )->LoadFromFile();

    
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetOrderNumber( 12000 );


    m_impostor2->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 1, "flags", 0 );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "flags", Vector( 0.0, 0.0, 0.0, 0.0 ) );

    m_impostor2->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 1, "color", 1 );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );
    


    m_impostor2->GetNodeFromPass( m_maskpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.vso", true ) ) );
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.pso", true ) ) );
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    RenderStatesSet imp2_mask_rss;

    imp2_mask_rss.AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    imp2_mask_rss.AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->SetRenderStates( imp2_mask_rss );

    m_impostor2->GetNodeFromPass( m_maskpass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "star_far.bmp" ) ), 0 );
    m_impostor2->GetNodeFromPass( m_maskpass )->GetTexture( 0 )->LoadFromFile();

    
    m_impostor2->GetNodeFromPass( m_maskpass )->SetOrderNumber( 12000 );


    m_impostor2->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "flags", 0 );
    m_impostor2->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "flags", Vector( 1.0, 0.0, 0.0, 0.0 ) );

    m_impostor2->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "color", 1 );
    m_impostor2->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );



    m_impostor2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "impostor1" ) );

    m_impostor2_node->SetContent( m_impostor2 );


    m_scenenodegraph.RegisterNode( m_impostor2_node );

    m_impostor2_node->LinkTo( m_impostor2_transfo_node );









    create_box();

    //////////////////////////////////////////////////////////////


    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );

    m_scenenodegraph.RegisterNode( m_camera_node );





    m_fpsmove.Init( DrawSpace::Utils::Vector( 0.0, 0.75, 12.0, 1.0 ) );
    m_fpsmove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FPSMovement>, SceneNode<DrawSpace::Core::FPSMovement>( "fps_node" ) );
    m_fpsmove_node->SetContent( &m_fpsmove );

    m_scenenodegraph.AddNode( m_fpsmove_node );
    m_scenenodegraph.RegisterNode( m_fpsmove_node );
    
    m_camera_node->LinkTo( m_fpsmove_node );












    m_camera_2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    
    //m_camera_2->Lock( m_boxes[0].drawable_node );

    m_camera_2->Lock( m_impostor2_node );

    
    m_camera2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera2" ) );
    m_camera2_node->SetContent( m_camera_2 );

    m_scenenodegraph.RegisterNode( m_camera2_node );

    
    




    

    m_linear_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::LinearMovement, DrawSpace::Core::LinearMovement );
    m_linear_mvt->Init( Vector( -5.0, 2.7, 9.0, 1.0 ), Vector( 0.0, 0.0, -1.0, 1.0 ), /*-90.0*/ 0.0, 0.0 );
    m_linearmove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::LinearMovement>, SceneNode<DrawSpace::Core::LinearMovement>( "lin_node" ) );
    m_linearmove_node->SetContent( m_linear_mvt );


    m_scenenodegraph.AddNode( m_linearmove_node );
    m_scenenodegraph.RegisterNode( m_linearmove_node );
    
    m_camera2_node->LinkTo( m_linearmove_node );









    

    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass2->GetRenderingQueue()->UpdateOutputQueue();
    m_maskpass->GetRenderingQueue()->UpdateOutputQueue();
    m_filterpass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass2->GetRenderingQueue()->UpdateOutputQueue();


    //////////////////////////////////////////////////////////////

    
    DrawSpace::Dynamics::InertBody::Parameters ground_params;    
    ground_params.mass = 0.0;
    ground_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    ground_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 100.0, 0.0, 100., 1.0 );
    ground_params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    m_ground_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, ground_params ) );
    m_ground_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "ground_body" ) );
    m_ground_body_node->SetContent( m_ground_body );

    m_scenenodegraph.AddNode( m_ground_body_node );
    m_scenenodegraph.RegisterNode( m_ground_body_node );

    m_ground_node->LinkTo( m_ground_body_node );
    

    

    

    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );
    m_calendar->RegisterWorld( &m_world );


    m_calendar->Startup( 162682566 );

    m_mouse_circularmode = true;

    m_current_camera = "camera2";
    m_scenenodegraph.SetCurrentCamera( m_current_camera );
  

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

            m_calendar->SetTimeFactor( Calendar::MUL4_TIME );
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

        case VK_F7:
            {
                if( "camera" == m_current_camera )
                {
                    m_current_camera = "camera2";
                }
                else if( "camera2" == m_current_camera )
                {
                    m_current_camera = "camera";
                }

                m_scenenodegraph.SetCurrentCamera( m_current_camera );
            }
            break;

        case VK_PRIOR:

            m_timer.TranslationSpeedInc( &m_zoom, 2.0 );
            Maths::Clamp( 0.01, 50.0, m_zoom );

            m_camera->UpdateProjectionZNear( m_zoom );

            break;

        case VK_NEXT:

            m_timer.TranslationSpeedDec( &m_zoom, 2.0 );
            Maths::Clamp( 0.01, 50.0, m_zoom );

            m_camera->UpdateProjectionZNear( m_zoom );

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
