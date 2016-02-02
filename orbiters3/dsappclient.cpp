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

#define PLANET_RAY 6000.0

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;


_DECLARE_DS_LOGGER( logger, "AppClient", DrawSpace::Logger::Configuration::GetInstance() )


#define SHIP_MASS 50.0


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

dsAppClient::dsAppClient( void ) : 
m_mouselb( false ), 
m_mouserb( false ), 
m_speed( 0.0 ), 
m_speed_speed( 5.0 ),
m_curr_camera( NULL ),
m_show_patch_render( false ),
m_ready( false ),
m_init_count( 0 ),
m_showinfos( true )
{    
    _INIT_LOGGER( "logorbiters3.conf" )  
    m_w_title = "orbiters 3 test";
    m_mouseleftbuttondown_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mouseleftbuttondown ) );
}

dsAppClient::~dsAppClient( void )
{

}


#define SPEED     1.5


void dsAppClient::init_planet_meshes( void )
{
    DrawSpace::SphericalLOD::Body::BuildMeshe();
    DrawSpace::SphericalLOD::Body::BuildMesheHigh();
}

void dsAppClient::init_assets_loaders( void )
{
    RegisterMouseInputEventsProvider( &m_mouse_input );

    bool status;

    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    /////////////////////////////////////

    m_font_import = new DrawSpace::Utils::CBFGFontImport();
    m_font = _DRAWSPACE_NEW_( Font, Font );
    m_font->SetImporter( m_font_import );

    status = m_font->Build( "mangalfont.bmp", "mangalfont.csv" );
}

void dsAppClient::init_passes( void )
{
    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );
    m_texturepass->Initialize();

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( /*false*/ true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 255 );


    //////////////////////////////////////////////////////////////
        
    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();

    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    
    m_finalpass2 = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass2" ) );
    m_finalpass2->Initialize();
    m_finalpass2->CreateViewportQuad();

    m_finalpass2->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    m_finalpass2->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass2->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass2->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass2->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass2->GetViewportQuad()->SetDrawingState( false );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

}

void dsAppClient::init_world_physics( void )
{
    World::m_scale = 1.0;
    m_world.Initialize();
}

void dsAppClient::init_spacebox( void )
{
    m_spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
    m_spacebox->RegisterPassSlot( m_texturepass );
    

    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, m_texturepass );

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_front5.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_back6.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_top3.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_bottom4.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_left2.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_right1.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetOrderNumber( 90 );

    

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
}

void dsAppClient::init_buildings( void )
{
    m_building = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_building->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_building->RegisterPassSlot( m_texturepass );
    
    m_building->GetMeshe()->SetImporter( m_meshe_import );

    m_building->GetMeshe()->LoadFromFile( "building3.ac", 0 );  



    m_building->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_building->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "building3.bmp" ) ), 0 );
    m_building->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_building_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "building" ) );
    m_building_node->SetContent( m_building );


    m_scenenodegraph.RegisterNode( m_building_node );
  
    
    DrawSpace::Dynamics::Body::Parameters bld_params;
    bld_params.mass = 0.0;
    bld_params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
    bld_params.shape_descr.meshe = *( m_building->GetMeshe() );
   
    m_building_collider = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Collider, DrawSpace::Dynamics::Collider );

    m_building_collider->SetKinematic( bld_params );



    m_building_collider_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Collider>, SceneNode<DrawSpace::Dynamics::Collider>( "building_collider" ) );
    m_building_collider_node->SetContent( m_building_collider );

    m_scenenodegraph.RegisterNode( m_building_collider_node );

    m_building_node->LinkTo( m_building_collider_node );



    m_longlat_mvt3 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt3->Init( -21.0000, 20.0089, /*400114.0*/PLANET_RAY * 1000.0 + 114.0 + 2000.0, 80.0, 0.0 );

    m_longlatmvt3_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::LongLatMovement>, SceneNode<DrawSpace::Core::LongLatMovement>( "longlatmvt3_node" ) );
    m_longlatmvt3_node->SetContent( m_longlat_mvt3 );

    m_scenenodegraph.RegisterNode( m_longlatmvt3_node );

    m_building_collider_node->LinkTo( m_longlatmvt3_node );
    
    

    ///////////////////////////////////////////////////////////////


    m_socle = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_socle->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_socle->RegisterPassSlot( m_texturepass );


    
    m_socle->GetMeshe()->SetImporter( m_meshe_import );

    m_socle->GetMeshe()->LoadFromFile( "socle.ac", 0 );  



    m_socle->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_socle->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "socle.bmp" ) ), 0 );

    m_socle->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_socle_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "socle" ) );
    m_socle_node->SetContent( m_socle );


    m_scenenodegraph.RegisterNode( m_socle_node );

    
  
    
    DrawSpace::Dynamics::Body::Parameters socle_params;
    socle_params.mass = 0.0;
    socle_params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
    socle_params.shape_descr.meshe = *( m_socle->GetMeshe() );


   
    m_socle_collider = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Collider, DrawSpace::Dynamics::Collider );

    m_socle_collider->SetKinematic( socle_params );


    m_socle_collider_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Collider>, SceneNode<DrawSpace::Dynamics::Collider>( "socle_collider" ) );
    m_socle_collider_node->SetContent( m_socle_collider );

    m_scenenodegraph.RegisterNode( m_socle_collider_node );

    m_socle_node->LinkTo( m_socle_collider_node );



    m_longlat_mvt4 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt4->Init( -21.0129, 20.0089, /*400129.0*/ PLANET_RAY * 1000.0 + 129.0 + 2000.0, 40.0, 0.0 );



    m_longlatmvt4_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::LongLatMovement>, SceneNode<DrawSpace::Core::LongLatMovement>( "longlatmvt4_node" ) );
    m_longlatmvt4_node->SetContent( m_longlat_mvt4 );

    m_scenenodegraph.RegisterNode( m_longlatmvt4_node );

    m_socle_collider_node->LinkTo( m_longlatmvt4_node );


    ///////////////////////////////////////////////////////////////
}

void dsAppClient::init_planet( void )
{

    m_multiFbm = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::LandscapeMultiFbm, DrawSpace::SphericalLOD::LandscapeMultiFbm );

  
    m_multiFbm->m_mask_seed1 = 671.0;
    m_multiFbm->m_mask_seed2 = 8444.0;

    m_multiFbm->m_mountains_seed1 = 117.0;
    m_multiFbm->m_mountains_seed2 = 245443.0;
    m_multiFbm->m_mountains_input_half_range = 16.0;
    m_multiFbm->m_mountains_lacunarity = 2.15;
    m_multiFbm->m_mountains_roughness = 0.25;

    m_multiFbm->m_plains_seed1 = 178.0;
    m_multiFbm->m_plains_seed2 = 3400.0;



    m_multiFbm->InitialiseResources();


    Shader* hm_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planethm.vso", true ) );
    Shader* hm_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planethm.pso", true ) );
    hm_vshader->LoadFromFile();
    hm_pshader->LoadFromFile();


    Shader* colors_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.vso", true ) );
    Shader* colors_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.pso", true ) );
    colors_vshader->LoadFromFile();
    colors_pshader->LoadFromFile();


    m_multiFbm->AddCollisionsShaders( hm_vshader );
    m_multiFbm->AddCollisionsShaders( hm_pshader );

    m_multiFbm->AddColorsTextureShaders( colors_vshader );
    m_multiFbm->AddColorsTextureShaders( colors_pshader );


    SphericalLOD::Config config;

    config.m_lod0base = 19000.0;

    config.m_landscape = m_multiFbm;

    m_planet = _DRAWSPACE_NEW_( DrawSpace::Planetoid::Body, DrawSpace::Planetoid::Body( "planet01", PLANET_RAY, &m_timer, config ) );

    m_planet->RegisterPlanetBodyPassSlot( m_texturepass );

    Texture* texture_planet = _DRAWSPACE_NEW_( Texture, Texture( "map.jpg" ) );
    texture_planet->LoadFromFile();

    Texture* texture_th_pixels = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_pixels_16.jpg" ) );
    texture_th_pixels->LoadFromFile();


    Texture* texture_th_splatting = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_splatting_16.jpg" ) );
    texture_th_splatting->LoadFromFile();


    Shader* planet_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet2.vso", true ) );
    Shader* planet_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet2.pso", true ) );
    planet_vshader->LoadFromFile();
    planet_pshader->LoadFromFile();
    
    for( long i = 0; i < 6; i++ )
    {
        Fx* fx = m_planet->CreatePlanetBodyFx( m_texturepass, i );

        //fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
        fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
        fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
        fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
        //fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

        fx->AddShader( planet_vshader );
        fx->AddShader( planet_pshader );

        //m_planet->BindExternalGlobalTexture( texture_planet, m_texturepass, i );

        m_planet->GetPlanetBodyNodeFromPass( m_texturepass, i )->SetTexture( texture_th_pixels, 1 );
        m_planet->GetPlanetBodyNodeFromPass( m_texturepass, i )->SetTexture( texture_th_splatting, 2 );
    }


    m_planet->SetOrbitDuration( 0.333 );
    m_planet->SetRevolutionTiltAngle( 25.0 );
    m_planet->SetRevolutionDuration( 1.0 );



    m_planet_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Planetoid::Body>, SceneNode<DrawSpace::Planetoid::Body>( "planet01" ) );
    m_planet_node->SetContent( m_planet );

    m_scenenodegraph.RegisterNode( m_planet_node );

    
    m_longlatmvt3_node->LinkTo( m_planet_node );
    m_longlatmvt4_node->LinkTo( m_planet_node );


    m_planet_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 270000000.0, 0.99, 0.0, 0.0, 0.0, 0.0, 0.333, 25.0, 1.0, NULL ) );
    m_planet_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "planet01_orbit" ) );
    m_planet_orbit_node->SetContent( m_planet_orbit );

    m_scenenodegraph.AddNode( m_planet_orbit_node );
    m_scenenodegraph.RegisterNode( m_planet_orbit_node );

    m_planet_node->LinkTo( m_planet_orbit_node );

    m_building_collider->SetReferentOrbiter( m_planet );
    m_socle_collider->SetReferentOrbiter( m_planet );

    ///////////////////////////////////

    m_planet->RegisterScenegraphCallbacks( m_scenenodegraph );
}


void dsAppClient::init_ship( void )
{
    DrawSpace::Dynamics::Body::Parameters cube_params;


    m_ship_drawable = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ship_drawable->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_ship_drawable->RegisterPassSlot( m_texturepass );

    
    m_ship_drawable->GetMeshe()->SetImporter( m_meshe_import );

    m_ship_drawable->GetMeshe()->LoadFromFile( "survey.ac", 0 );    

    m_ship_drawable->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_ship_drawable->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "surveyship_color.jpg" ) ), 0 );



    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    

    m_ship_drawable_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "rocket" ) );
    m_ship_drawable_node->SetContent( m_ship_drawable );
    

    m_scenenodegraph.RegisterNode( m_ship_drawable_node );



    cube_params.mass = SHIP_MASS;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 74.1285 / 2.0, 21.4704 / 2.0, 81.911 / 2.0, 1.0 );
    
    //cube_params.initial_attitude.Translation( 265000000.0, 0.0, 0.0 );
    cube_params.initial_attitude.Translation( 269000000.0, 0.0, 59000000.0 );

    m_ship = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Rocket, DrawSpace::Dynamics::Rocket( &m_world, cube_params ) );
   
    m_ship_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Rocket>, SceneNode<DrawSpace::Dynamics::Rocket>( "rocket_body" ) );
    m_ship_node->SetContent( m_ship );

    m_scenenodegraph.AddNode( m_ship_node );
    m_scenenodegraph.RegisterNode( m_ship_node );

    m_ship_drawable_node->LinkTo( m_ship_node );

    
    //////////////////////////////////////////////////////////////
}


void dsAppClient::init_cameras( void )
{
    m_camera3 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera3->LockOnBody( "ship", m_ship );
    m_camera3->Lock( m_ship_node );
    m_camera3->SetReferentBody( m_ship );
    
    m_camera3_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera3" ) );
    m_camera3_node->SetContent( m_camera3 );
    m_scenenodegraph.RegisterNode( m_camera3_node );


    m_circular_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::CircularMovement, DrawSpace::Core::CircularMovement );
    m_circular_mvt->Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( 385.0, 90.0, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 270.0, 0.0, 0.0 );

    m_circmvt_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::CircularMovement>, SceneNode<DrawSpace::Core::CircularMovement>( "circmvt" ) );


    m_circmvt_node->SetContent( m_circular_mvt );
    m_scenenodegraph.RegisterNode( m_circmvt_node );

    m_camera3_node->LinkTo( m_circmvt_node );

    m_circmvt_node->LinkTo( m_ship_node );
}

void dsAppClient::init_reticle( void )
{
    m_reticle_widget = _DRAWSPACE_NEW_( ReticleWidget, ReticleWidget( "reticle_widget", (long)10, (long)10, &m_scenenodegraph, NULL ) );

    m_reticle_widget->Lock( m_socle_node );
    
    DrawSpace::Gui::ReticleWidget::ClippingParams clp;
    clp.clipping_policy = DrawSpace::Gui::ReticleWidget::CLIPPING_HOLD;
    clp.xmin = -0.5;
    clp.xmax = 0.5;
    clp.ymin = -0.375;
    clp.ymax = 0.375;

    m_reticle_widget->SetClippingParams( clp );

    m_reticle_widget->RegisterMouseLeftButtonDownEventHandler( m_mouseleftbuttondown_eventhandler );

    m_mouse_input.RegisterWidget( m_reticle_widget );    
}

void dsAppClient::init_text_assets( void )
{
    m_text_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget", DRAWSPACE_GUI_WIDTH, DRAWSPACE_GUI_HEIGHT, m_font, true, m_reticle_widget ) );


    m_text_widget->GetBackgroundImage()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_text_widget->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );


    m_text_widget->GetBackgroundImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetBackgroundImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget->GetBackgroundImage()->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "reticle.bmp" ) ), 0 );
    m_text_widget->GetBackgroundImage()->GetTexture( 0 )->LoadFromFile();


    m_text_widget->GetImage()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );  
    m_text_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );

    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one"  ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_text_widget->GetImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetImage()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_text_widget->GetText()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );      
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
    
    m_text_widget_2 = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget_2", 1200, 360, m_font, false, m_text_widget ) );
    
    m_text_widget_2->GetImage()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_text_widget_2->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget_2->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );

    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one"  ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_text_widget_2->GetImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_2->GetImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget_2->GetText()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_text_widget_2->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_text_widget_2->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_text_widget_2->GetText()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_2->GetText()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_text_widget_2->GetText()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_text_widget_2->GetText()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_text_widget_2->GetText()->AddShaderParameter( 1, "color", 0 );
    m_text_widget_2->GetText()->SetShaderRealVector( "color", Utils::Vector( 0.0, 1.0, 0.0, 1.0 ) );


    m_text_widget_2->GetImage()->SetOrderNumber( 20000 );
    m_text_widget_2->GetInternalPass()->GetRenderingQueue()->UpdateOutputQueue();

    m_text_widget_2->RegisterToPass( m_finalpass );
}

void dsAppClient::init_rendering_queues( void )
{
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_finalpass2->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
}

void dsAppClient::init_calendar( void )
{
    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );

    m_calendar->RegisterWorld( &m_world );
    m_calendar->RegisterWorld( m_planet->GetWorld() );

    m_calendar->RegisterOrbit( m_planet_orbit );
    m_calendar->RegisterOrbiter( m_planet );
}

void dsAppClient::init_planet_noise( void )
{    
    //m_planet->InitNoisingTextures();
}


void dsAppClient::render_universe( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_scenenodegraph.ComputeTransformations( m_timer );

    m_text_widget->SetVirtualTranslation( 100, 75 );
    m_text_widget_2->SetVirtualTranslation( -60, 160 );

    char distance[64];
    sprintf( distance, "%.3f km", m_reticle_widget->GetLastDistance() / 1000.0 );

    m_text_widget_2->SetText( -40, 0, 30, dsstring( distance ), DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
    m_reticle_widget->Transform();
    m_reticle_widget->Draw();
    

    //////////////////////////////////////////////////////////////


    DrawSpace::SphericalLOD::Patch* current_patch = m_planet->GetFragment( 0 )->GetCurrentPatch();

    m_planet->DrawSubPasses();

    m_texturepass->GetRenderingQueue()->Draw();

    m_text_widget->Draw();

    m_finalpass->GetRenderingQueue()->Draw();

    m_finalpass2->GetRenderingQueue()->Draw();


    long current_fps = m_timer.GetFPS();
    renderer->DrawText( 0, 255, 0, 10, 35, "%d fps", current_fps );

    if( m_showinfos )
    {
        dsstring date;
        m_calendar->GetFormatedDate( date );    
        renderer->DrawText( 0, 255, 0, 10, 55, "%s", date.c_str() );

    


        dsreal speed = m_ship->GetLinearSpeedMagnitude();

        renderer->DrawText( 0, 255, 0, 10, 95, "speed = %.1f km/h ( %.1f m/s) - aspeed = %.1f", speed * 3.6, speed, m_ship->GetAngularSpeedMagnitude() );

        bool hotstate = m_planet->GetFragment( 0 )->GetHotState();
        if( hotstate )
        {
            renderer->DrawText( 0, 255, 0, 10, 130, "fragment hot" );

            dsreal alt = m_planet->GetFragment( 0 )->GetPlanetBody()->GetHotPointAltitud();

            if( alt > 10000.0 )
            {
                renderer->DrawText( 0, 255, 0, 10, 145, "hotpoint altitude = %.3f km", alt / 1000.0 );
            }
            else
            {
                renderer->DrawText( 0, 255, 0, 10, 145, "hotpoint altitude = %.1f m", alt );
            }
        
        }

        dsreal rel_alt;
        m_planet->GetInertBodyRelativeAltitude( m_ship, rel_alt );
        renderer->DrawText( 0, 255, 0, 10, 220, "relative_alt = %f", rel_alt );

    
        /*
        if( current_patch )
        {
            int face = current_patch->GetOrientation();
            double xpos, ypos;
            current_patch->GetUnitPos( xpos, ypos );
            double sidelength = current_patch->GetUnitSideLenght();
            int lod = m_planet->GetFragment( 0 )->GetCurrentPatchLOD();
            long nb_meshebuild;
            m_planet->GetFragment( 0 )->GetCollisionMesheBuildStats( nb_meshebuild );

            renderer->DrawText( 0, 255, 0, 10, 245, "current_patch => face %d lod %d dims %f", face, lod, sidelength, xpos, ypos, nb_meshebuild, current_patch->GetUnitSideLenght() );
            renderer->DrawText( 0, 255, 0, 10, 260, "width = %f x = %f y = %f, %d", sidelength, xpos, ypos, nb_meshebuild );

            SphericalLOD::FaceDrawingNode::Stats stats;
            SphericalLOD::FaceDrawingNode* facenode;

            facenode = static_cast<SphericalLOD::FaceDrawingNode*>( m_planet->GetPlanetBodyNodeFromPass( m_texturepass, face ) );

            facenode->GetStats( stats );

            renderer->DrawText( 0, 255, 0, 10, 275, "nb patch %d", stats.nb_patchs );

        }
        else
        {
            renderer->DrawText( 0, 255, 0, 10, 245, "current_patch => null" );
        }
        */
    }
  
    renderer->FlipScreen();

    m_calendar->Run();
}

void dsAppClient::print_init_trace( const dsstring& p_string )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    renderer->BeginScreen();
    renderer->DrawText( 255, 0, 0, 10, ( m_init_count * 20 ) + 30, p_string.c_str() );
    renderer->EndScreen();
}


void dsAppClient::OnRenderFrame( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    if( !m_ready )
    {
        switch( m_init_count )
        {
            case 0:
                print_init_trace( "initialising universe..." );
                break;

            case 1:
                print_init_trace( "planets patches meshes initialisation..." );
                init_planet_meshes();
                break;

            case 2:
                print_init_trace( "assets loaders initialisation..." );
                init_assets_loaders();
                break;

            case 3:
                print_init_trace( "rendering passes creation..." );
                init_passes();
                break;

            case 4:
                print_init_trace( "physics world initialisation..." );
                init_world_physics();
                break;

            case 5:
                print_init_trace( "spacebox creation..." );
                init_spacebox();
                break;

            case 6:                
                print_init_trace( "buildings creation..." );
                init_buildings();
                break;

            case 7:
                print_init_trace( "planet creation..." );
                init_planet();
                break;

            case 8:
                print_init_trace( "ship creation..." );
                init_ship();
                break;

            case 9:
                print_init_trace( "cameras initialisation..." );
                init_cameras();
                break;

            case 10:
                print_init_trace( "reticle creation..." );
                init_reticle();
                break;

            case 11:
                print_init_trace( "text display assets creation..." );
                init_text_assets();
                break;

            case 12:
                print_init_trace( "rendering queues update..." );
                init_rendering_queues();
                break;

            case 13:
                print_init_trace( "planet noise data initialisation..." );
                break;

            case 14:                
                init_planet_noise();
                break;

            case 15:
                print_init_trace( "calendar initialisation..." );
                init_calendar();
                break;

            case 16:
                print_init_trace( "launching simulation..." );
                m_mouse_circularmode = true;
                m_scenenodegraph.SetCurrentCamera( "camera3" );
                m_curr_camera = m_camera3;   
                //m_calendar->Startup( 162682566 );
                m_calendar->Startup( 0 );
                m_ready = true;
                break;        
        }

        renderer->FlipScreen();

        m_init_count++;
        return;
    }
    else
    {
        render_universe();
    }
}


bool dsAppClient::OnIdleAppInit( void )
{
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


            m_timer.TranslationSpeedInc( &m_speed, m_speed_speed );
            m_speed_speed *= 1.83;
          
            break;

        case 'W':

            m_timer.TranslationSpeedDec( &m_speed, m_speed_speed );
            m_speed_speed *= 1.86;
 
            break;


        case 'E':

            m_ship->ApplyUpPitch( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            break;

        case 'C':

            m_ship->ApplyDownPitch( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            break;

        case 'S':

            m_ship->ApplyLeftYaw( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            break;

        case 'F':

            m_ship->ApplyRightYaw( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            break;


        case 'Z':

            m_ship->ApplyLeftRoll( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            break;

        case 'R':

            m_ship->ApplyRightRoll( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            break;

        case 'T':

            m_ship->ApplyFwdForce( 51000000.0 );
            break;


        case 'M':
            m_ship->ZeroASpeed();
            break;


        case 'L':
            
            m_ship->ZeroLSpeed();
            break;


        case VK_SPACE:
            break;

        case VK_RETURN:

            m_ship->ApplyFwdForce( 11000.0 );
            break;

        case VK_UP:

            m_ship->ApplyFwdForce( -5000.0 );
            break;

        case VK_DOWN:

            m_ship->ApplyDownForce( -10000.0 );
            break;


        case VK_LEFT:

            m_circular_mvt->SetAngularSpeed( 30.0 );
            break;

        case VK_RIGHT:

            m_circular_mvt->SetAngularSpeed( -30.0 );
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

        case VK_LEFT:
        case VK_RIGHT:
            m_circular_mvt->SetAngularSpeed( 0.0 );
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

            m_calendar->SetTimeFactor( Calendar::MUL2_TIME );
            break;

        case VK_F3:

            m_calendar->SetTimeFactor( Calendar::MUL10_TIME );
            break;

        case VK_F4:

            m_calendar->SetTimeFactor( Calendar::MUL100_TIME );
            break;

        case VK_F5:

            m_calendar->SetTimeFactor( Calendar::SEC_1HOUR_TIME );
            break;


        case VK_F6:

            m_calendar->SetTimeFactor( Calendar::SEC_1DAY_TIME );
            break;



        case VK_F8:

            if( !m_show_patch_render )
            {
                m_show_patch_render = true;
                m_finalpass2->GetViewportQuad()->SetDrawingState( true );
            }
            else
            {
                m_show_patch_render = false;
                m_finalpass2->GetViewportQuad()->SetDrawingState( false );
            }

            break;

        case VK_F9:

            m_ship_drawable->SetDrawingState( m_texturepass, false );
            m_showinfos = false;
            break;

        case VK_F7:

            m_ship_drawable->SetDrawingState( m_texturepass, true );
            m_showinfos = true;
            break;
    }
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
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

void dsAppClient::on_mouseleftbuttondown( DrawSpace::Gui::Widget* p_widget )
{

}
