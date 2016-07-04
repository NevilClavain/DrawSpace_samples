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
#include "Psapi.h"

/*
#define PLANET_RAY                          6000.0
#define PLANET_ATMO_THICKNESS               85000.0
#define ATMO_SCATTERING_SPACE_GROUND_LIMIT  215000.0
#define FOG_ALT_LIMIT                       30000.0
#define ATMO_SCATTERING_ALPHA_ALT_VIEWER    285000.0
#define FLAT_CLOUDS_ALT                     28000.0
#define VOLUMETRIC_CLOUDS_ALT               13000.0
#define PLAINS_AMPLITUDE                    800.0
#define MOUNTAINS_AMPLITUDE                 7000.0
#define MOUNTAINS_OFFSET                    -2200.0
#define VERTICAL_OFFSET                     20.0
#define TEMP_DEC_PER_KM                     6.4
#define BEACH_LIMIT                         25.0
#define CLOUDS_PROCEDURALRULES_FILE         "planet_clouds.rules"
#define CLOUDS_HEIGHT                       5000.0
#define CLOUDS_FOG_DENSITY                  0.0000015
#define FOG_DENSITY                         0.000032
#define ZBUFFER_ACTIVATION_REL_ALT          1.009
#define TERRAIN_BUMP_FACTOR                 100.0
#define NB_LOD_FREECAMERAS                  16
#define NB_LOD_INERTBODIES                  16
*/



#define PLANET_RAY                          500.0
#define PLANET_ATMO_THICKNESS               12000.0
#define ATMO_SCATTERING_SPACE_GROUND_LIMIT  70000.0
#define FOG_ALT_LIMIT                       10000.0
#define ATMO_SCATTERING_ALPHA_ALT_VIEWER    75000.0
#define FLAT_CLOUDS_ALT                     3300.0
#define VOLUMETRIC_CLOUDS_ALT               2100.0
#define PLAINS_AMPLITUDE                    50.0
#define MOUNTAINS_AMPLITUDE                 800.0
#define MOUNTAINS_OFFSET                    -120.0
#define VERTICAL_OFFSET                     2.0
#define TEMP_DEC_PER_KM                     66.0
#define BEACH_LIMIT                         25.0
#define CLOUDS_PROCEDURALRULES_FILE         "planet_clouds_small_small.rules"
#define CLOUDS_HEIGHT                       750.0
#define CLOUDS_FOG_DENSITY                  0.000025
#define FOG_DENSITY                         0.00020
#define ZBUFFER_ACTIVATION_REL_ALT          1.0099
#define TERRAIN_BUMP_FACTOR                 10.0
#define NB_LOD_FREECAMERAS                  10
#define NB_LOD_INERTBODIES                  15




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

MultiFractalBinder::MultiFractalBinder( void ) :
m_plains_amplitude( PLAINS_AMPLITUDE ),
m_mountains_amplitude( MOUNTAINS_AMPLITUDE ),
m_vertical_offset( VERTICAL_OFFSET ),
m_mountains_offset( MOUNTAINS_OFFSET ),
m_plains_seed1( 18334.0 ),
m_plains_seed2( 1770.0 ),
m_mix_seed1( 635.0 ),
m_mix_seed2( 82.0 )
{
    m_renderer = SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
}

void MultiFractalBinder::Bind( void )
{
    Vector landscape_control;
    Vector seeds;

    landscape_control[0] = m_plains_amplitude;
    landscape_control[1] = m_mountains_amplitude;
    landscape_control[2] = m_vertical_offset;
    landscape_control[3] = m_mountains_offset;

    seeds[0] = m_plains_seed1;
    seeds[1] = m_plains_seed2;
    seeds[2] = m_mix_seed1;
    seeds[3] = m_mix_seed2;

    m_renderer->SetFxShaderParams( 0, 30, landscape_control );
    m_renderer->SetFxShaderParams( 0, 31, seeds );
}

void MultiFractalBinder::Unbind( void )
{
}



PlanetClimateBinder::PlanetClimateBinder( void ) : MultiFractalBinder()
{
}

void PlanetClimateBinder::Bind( void )
{
    // planete temperee
    

    Vector thparams( 120.0, 220.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
    Vector thparams2( 0.48, 0.75, 0.45, 0.55 );



    // planete chaude et peu humide (aride) : desertique
    //Vector thparams( 0.0, 10.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
    //Vector thparams2( 0.0, 0.0, 0.78, 0.78 );

    // planete chaude et tres humide : monde tropical
    //Vector thparams( 2500.0, 1800.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
    //Vector thparams2( 0.05, 0.1, 0.45, 0.48 );


    // monde glacé et plutot sec
    //Vector thparams( 50.0, 80.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
    //Vector thparams2( 1.4, 1.5, 0.28, 0.99 );

    // monde froid et plutot humide
    //Vector thparams( 1400.0, 1900.0, TEMP_DEC_PER_KM, BEACH_LIMIT );
    //Vector thparams2( 0.92, 1.5, 0.37, 0.99 );


    m_renderer->SetFxShaderParams( 0, 32, thparams );
    m_renderer->SetFxShaderParams( 0, 33, thparams2 );

    MultiFractalBinder::Bind();
}

void PlanetClimateBinder::Unbind( void )
{
    MultiFractalBinder::Unbind();
}



PlanetDetailsBinder::PlanetDetailsBinder( dsreal p_planetRay, dsreal p_atmoThickness ) :
m_planet_node( NULL ),
m_ocean_details_alt( 1.0010 ),
m_water_bump_texture_resol( 256 ),
m_water_bump_factor( 0.75 ),
m_terrain_bump_factor( TERRAIN_BUMP_FACTOR )
{

    m_mirror_mode = false;

    m_skyfromspace_ESun = 8.7; //9.0;
    m_skyfromatmo_ESun = 70.0; //50.0;
    m_groundfromspace_ESun = 24.0;
    m_groundfromatmo_ESun = 12.0;

    m_innerRadius = p_planetRay;
    m_outerRadius = p_planetRay + p_atmoThickness;

    m_waveLength[0] = 0.650;
    m_waveLength[1] = 0.570;
    m_waveLength[2] = 0.475;
    m_kr = 0.0025;
    m_km = 0.0010;
    m_scaleDepth = 0.25;

    m_atmo_scattering_flags0[0] = m_outerRadius;
    m_atmo_scattering_flags0[1] = m_innerRadius;
    m_atmo_scattering_flags0[2] = m_outerRadius * m_outerRadius;
    m_atmo_scattering_flags0[3] = m_innerRadius * m_innerRadius;

    m_atmo_scattering_flags1[0] = m_scaleDepth;
    m_atmo_scattering_flags1[1] = 1.0 / m_scaleDepth;
    m_atmo_scattering_flags1[2] = 1.0 / ( m_outerRadius - m_innerRadius );
    m_atmo_scattering_flags1[3] = m_atmo_scattering_flags1[2] / m_scaleDepth;

    m_atmo_scattering_flags2[0] = 1.0 / pow( m_waveLength[0], 4.0 );
    m_atmo_scattering_flags2[1] = 1.0 / pow( m_waveLength[1], 4.0 );
    m_atmo_scattering_flags2[2] = 1.0 / pow( m_waveLength[2], 4.0 );

    m_atmo_scattering_flags3[0] = m_kr;
    m_atmo_scattering_flags3[1] = m_km;
    m_atmo_scattering_flags3[2] = 4.0 * m_kr * 3.1415927;
    m_atmo_scattering_flags3[3] = 4.0 * m_km * 3.1415927;

    m_atmo_scattering_flags4[0] = m_skyfromspace_ESun;
    m_atmo_scattering_flags4[1] = m_skyfromatmo_ESun;
    m_atmo_scattering_flags4[2] = m_groundfromspace_ESun;
    m_atmo_scattering_flags4[3] = m_groundfromatmo_ESun;

    m_atmo_scattering_flags5[0] = ATMO_SCATTERING_SPACE_GROUND_LIMIT; // altitude limite de transition entre xxxfromspace_atmo_scattering et xxxfromatmo_atmo_scattering
    m_atmo_scattering_flags5[1] = FOG_ALT_LIMIT; // altitude debut d'apparition du fog "sol"
    m_atmo_scattering_flags5[2] = FOG_DENSITY; // intensite fog "sol"
    m_atmo_scattering_flags5[3] = ATMO_SCATTERING_ALPHA_ALT_VIEWER; // altitude limite pour prise en compte facteur altitude camera pour le calcul de l'alpha

    // couleurs fog "sol"    
    m_atmo_scattering_flags6[0] = 0.45;
    m_atmo_scattering_flags6[1] = 0.63;
    m_atmo_scattering_flags6[2] = 0.78;
    m_atmo_scattering_flags6[3] = 1.0;


    m_lights[0].m_enable = true;
    m_lights[0].m_color[0] = 1.0;
    m_lights[0].m_color[1] = 0.99;
    m_lights[0].m_color[2] = 0.99;
    m_lights[0].m_color[3] = 1.0;
    m_lights[0].m_dir[0] = -1.0;
    m_lights[0].m_dir[1] = 0.0;
    m_lights[0].m_dir[2] = 0.0;
    m_lights[0].m_dir[3] = 1.0;

    m_lights[0].m_dir.Normalize();

    m_lights[1].m_enable = false;
    m_lights[1].m_color[0] = 1.0;
    m_lights[1].m_color[1] = 0.9;
    m_lights[1].m_color[2] = 0.9;
    m_lights[1].m_color[3] = 1.0;
    m_lights[1].m_dir[0] = -1.0;
    m_lights[1].m_dir[1] = 0.2;
    m_lights[1].m_dir[2] = 0.0;
    m_lights[1].m_dir[3] = 1.0;

    m_lights[1].m_dir.Normalize();

    m_lights[2].m_enable = false;
    m_lights[2].m_color[0] = 1.0;
    m_lights[2].m_color[1] = 0.5;
    m_lights[2].m_color[2] = 0.5;
    m_lights[2].m_color[3] = 1.0;
    m_lights[2].m_dir[0] = 0.0;
    m_lights[2].m_dir[1] = -1.0;
    m_lights[2].m_dir[2] = 0.0;
    m_lights[2].m_dir[3] = 1.0;

    m_lights[2].m_dir.Normalize();

    m_ambient = false;
    m_ambient_color[0] = 0.1;
    m_ambient_color[1] = 0.1;
    m_ambient_color[2] = 0.1;
    m_ambient_color[3] = 1.0;

}

void PlanetDetailsBinder::Bind( void )
{

    Vector flags6( 16.0, 1.095, 1.0040, m_ocean_details_alt );
    
    Vector flags_lights;
    flags_lights[0] = ( m_ambient ? 1.0 : 0.0 );
    flags_lights[1] = ( m_lights[0].m_enable ? 1.0 : 0.0 );
    flags_lights[2] = ( m_lights[1].m_enable ? 1.0 : 0.0 );
    flags_lights[3] = ( m_lights[2].m_enable ? 1.0 : 0.0 );


    m_renderer->SetFxShaderParams( 0, 32, m_atmo_scattering_flags0 );
    m_renderer->SetFxShaderParams( 0, 33, m_atmo_scattering_flags1 );
    m_renderer->SetFxShaderParams( 0, 34, m_atmo_scattering_flags2 );   
    m_renderer->SetFxShaderParams( 0, 35, m_atmo_scattering_flags3 );
    m_renderer->SetFxShaderParams( 0, 36, m_atmo_scattering_flags4 );
    m_renderer->SetFxShaderParams( 0, 37, m_atmo_scattering_flags5 );
    m_renderer->SetFxShaderParams( 0, 38, m_atmo_scattering_flags6 );

    m_renderer->SetFxShaderParams( 0, 40, flags_lights );
    m_renderer->SetFxShaderParams( 0, 41, m_ambient_color );

    m_renderer->SetFxShaderParams( 0, 42, m_lights[0].m_local_dir );
    m_renderer->SetFxShaderParams( 0, 43, m_lights[0].m_dir );
    m_renderer->SetFxShaderParams( 0, 44, m_lights[0].m_color );   

    m_renderer->SetFxShaderParams( 0, 45, m_lights[1].m_local_dir );
    m_renderer->SetFxShaderParams( 0, 46, m_lights[1].m_dir );
    m_renderer->SetFxShaderParams( 0, 47, m_lights[1].m_color );   

    m_renderer->SetFxShaderParams( 0, 48, m_lights[2].m_local_dir );
    m_renderer->SetFxShaderParams( 0, 49, m_lights[2].m_dir );
    m_renderer->SetFxShaderParams( 0, 50, m_lights[2].m_color );

    Vector mirror_flag;

    mirror_flag[0] = ( m_mirror_mode ? 1.0 : 0.0 );
    mirror_flag[1] = m_innerRadius;

    m_renderer->SetFxShaderParams( 0, 51, mirror_flag ); 

    m_renderer->SetFxShaderParams( 1, 6, flags6 );

    m_renderer->SetFxShaderParams( 1, 7, flags_lights );
    m_renderer->SetFxShaderParams( 1, 8, m_ambient_color );

    m_renderer->SetFxShaderParams( 1, 9, m_lights[0].m_local_dir );
    m_renderer->SetFxShaderParams( 1, 10, m_lights[0].m_dir );
    m_renderer->SetFxShaderParams( 1, 11, m_lights[0].m_color );   

    m_renderer->SetFxShaderParams( 1, 12, m_lights[1].m_local_dir );
    m_renderer->SetFxShaderParams( 1, 13, m_lights[1].m_dir );
    m_renderer->SetFxShaderParams( 1, 14, m_lights[1].m_color );   

    m_renderer->SetFxShaderParams( 1, 15, m_lights[2].m_local_dir );
    m_renderer->SetFxShaderParams( 1, 16, m_lights[2].m_dir );
    m_renderer->SetFxShaderParams( 1, 17, m_lights[2].m_color );  

    m_renderer->SetFxShaderParams( 1, 18, m_atmo_scattering_flags0 );
    m_renderer->SetFxShaderParams( 1, 19, m_atmo_scattering_flags1 );
    m_renderer->SetFxShaderParams( 1, 20, m_atmo_scattering_flags2 );   
    m_renderer->SetFxShaderParams( 1, 21, m_atmo_scattering_flags3 );
    m_renderer->SetFxShaderParams( 1, 22, m_atmo_scattering_flags4 );
    m_renderer->SetFxShaderParams( 1, 23, m_atmo_scattering_flags5 );
    m_renderer->SetFxShaderParams( 1, 24, m_atmo_scattering_flags6 );

    m_planet_final_transform_rots.Transpose(); // faire comme dans le plugin
    m_renderer->SetFxShaderMatrix( 1, 25, m_planet_final_transform_rots );


    long clouds_texture_w;
    long clouds_texture_h;
    long clouds_texture_bpp;
                    
    m_clouds_texture->GetFormat( clouds_texture_w, clouds_texture_h, clouds_texture_bpp );

    Vector clouds_texture_infos( clouds_texture_w, clouds_texture_h, 67.0, 0.0 );

    m_renderer->SetFxShaderParams( 1, 29, clouds_texture_infos );


    Vector water_bump_flag( m_water_bump_texture_resol, m_water_bump_factor, 0.0, 0.0 );
    m_renderer->SetFxShaderParams( 1, 30, water_bump_flag );

    Vector terrain_bump_flag( m_terrain_bump_factor, 0.0, 0.0, 0.0 );
    m_renderer->SetFxShaderParams( 1, 31, terrain_bump_flag );


    MultiFractalBinder::Bind();
}

void PlanetDetailsBinder::Unbind( void )
{
    MultiFractalBinder::Unbind();
}

void PlanetDetailsBinder::SetPlanetNode( DrawSpace::Core::SceneNode<DrawSpace::SphericalLOD::Root>* p_planet_node )
{
    m_planet_node = p_planet_node;
}

void PlanetDetailsBinder::SetCloudsTexture( DrawSpace::Core::Texture* p_texture )
{
    m_clouds_texture = p_texture;
}

void PlanetDetailsBinder::Update( void )
{
    Matrix planet_final_transform;

    m_planet_node->GetFinalTransform( planet_final_transform );

    Vector planet_pos;
    planet_pos[0] = -planet_final_transform( 3, 0 );
    planet_pos[1] = -planet_final_transform( 3, 1 );
    planet_pos[2] = -planet_final_transform( 3, 2 );
    planet_pos[3] = 1.0;

    planet_pos.Normalize();

    // TEMPORAIRE
    m_lights[0].m_dir = planet_pos;
    //m_lights[0].m_dir[0] = 0.0;
    //m_lights[0].m_dir[1] = 0.0;
    //m_lights[0].m_dir[2] = 1.0;

    m_lights[0].m_dir.Normalize();

    planet_final_transform.ClearTranslation();
    m_planet_final_transform_rots = planet_final_transform;



    planet_final_transform.Transpose();

    for( long i = 0; i < 3; i++ )
    {
        if( m_lights[i].m_enable )
        {
            planet_final_transform.Transform( &m_lights[i].m_dir, &m_lights[i].m_local_dir );

            m_lights[i].m_local_dir.Normalize();
        }
    }
}

CloudsStateMachine::CloudsStateMachine( DrawSpace::Clouds* p_clouds, DrawSpace::Clouds* p_clouds_low, DrawSpace::Core::LongLatMovement* p_ll ) :
m_clouds( p_clouds ),
m_clouds_low( p_clouds_low ),
m_ll( p_ll )
{
}

CloudsStateMachine::~CloudsStateMachine( void )
{
}

void CloudsStateMachine::UpdateViewerSphericalPos( dsreal p_degLong, dsreal p_degLat, dsreal p_alt )
{

}

void CloudsStateMachine::Run( void )
{

}

dsAppClient::dsAppClient( void ) : 
m_mouselb( false ), 
m_mouserb( false ), 
m_speed( 0.0 ), 
m_speed_speed( 5.0 ),
m_curr_camera( NULL ),
m_final_pass_2( false ),
m_ready( false ),
m_init_count( 0 ),
m_showinfos( false ),
m_water_anim( 0.0 ),
m_water_anim_inc( true ),
m_timefactor( "x1" )
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
    DrawSpace::SphericalLOD::Body::BuildMeshes();
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



    m_occlusionpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "occlusion_pass" ) );
    m_occlusionpass->Initialize();

    m_occlusionpass->GetRenderingQueue()->EnableDepthClearing( false );
    m_occlusionpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_occlusionpass->GetRenderingQueue()->SetTargetClearingColor( 255, 255, 255, 255 );




    m_zoompass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "zoom_pass" ) );
    m_zoompass->SetTargetDimsFromRenderer( false );
    m_zoompass->SetTargetDims( 32, 32 );
    m_zoompass->Initialize();
    m_zoompass->CreateViewportQuad();
    

    m_zoompass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_zoompass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "zoom.vsh", false ) ) );
    m_zoompass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "zoom.psh", false ) ) );
    m_zoompass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_zoompass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_zoompass->GetViewportQuad()->SetTexture( m_occlusionpass->GetTargetTexture(), 0 );

    m_zoompass->GetViewportQuad()->AddShaderParameter( 0, "zoom_area", 24 );
    m_zoompass->GetViewportQuad()->SetShaderRealVector( "zoom_area", Vector( 0.0025, 0.0, 0.0, 0.0 ) );





    m_texturemirrorpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texturemirror_pass" ) );

    m_texturemirrorpass->Initialize();
    
    m_texturemirrorpass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturemirrorpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturemirrorpass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 255 );



    m_wavespass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "waves_pass" ) );

    m_wavespass->SetTargetDimsFromRenderer( false );
    //m_wavespass->SetTargetDims( 512, 512 );
    m_wavespass->SetTargetDims( 1024, 1024 );

    m_wavespass->Initialize();
    m_wavespass->CreateViewportQuad();
    
    m_wavespass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_wavespass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_water_waves.vso", true ) ) );
    m_wavespass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_water_waves.pso", true ) ) );
    m_wavespass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_wavespass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_wavespass->GetViewportQuad()->AddShaderParameter( 1, "waves", 0 );





    m_waterbumppass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "water_bump_pass" ) );

    m_waterbumppass->SetRenderPurpose( Texture::RENDERPURPOSE_FLOATVECTOR );

    m_waterbumppass->Initialize();
    
    m_waterbumppass->GetRenderingQueue()->EnableDepthClearing( true );
    m_waterbumppass->GetRenderingQueue()->EnableTargetClearing( true );
    m_waterbumppass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 255 );


    //////////////////////////////////////////////////////////////
        
    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();

    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_water.vso", true ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_water.pso", true ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    m_finalpass->GetViewportQuad()->SetTexture( m_texturemirrorpass->GetTargetTexture(), 1 );
    m_finalpass->GetViewportQuad()->SetTexture( m_waterbumppass->GetTargetTexture(), 2 );

    m_finalpass->GetViewportQuad()->AddShaderParameter( 1, "flags", 0 );
    m_finalpass->GetViewportQuad()->AddShaderParameter( 1, "color", 1 );
    m_finalpass->GetViewportQuad()->SetShaderRealVector( "color", Vector( 0.80, 0.82, 0.9, 1.0 ) );
    
    m_finalpass2 = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass2" ) );
    m_finalpass2->Initialize();
    m_finalpass2->CreateViewportQuad();

    m_finalpass2->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    m_finalpass2->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass2->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass2->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass2->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass2->GetViewportQuad()->SetTexture( m_texturemirrorpass->GetTargetTexture(), 0 );    
    m_finalpass2->GetViewportQuad()->SetDrawingState( false );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    // si zoompass n'est pas connectee a une finalpass
    void* data;
    renderer->CreateTexture( m_zoompass->GetTargetTexture(), &data );

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

    for( int i = 0; i < 6; i++ )
    {
        m_planet_collisions_binder[i] = new MultiFractalBinder;
        m_planet_climate_binder[i] = new PlanetClimateBinder;
        m_planet_detail_binder[i] = new PlanetDetailsBinder( PLANET_RAY * 1000.0, PLANET_ATMO_THICKNESS );

        m_planet_waterbump_binder[i] = new DrawSpace::SphericalLOD::Binder();

        m_planet_atmosphere_binder[i] = new PlanetDetailsBinder( PLANET_RAY * 1000.0, PLANET_ATMO_THICKNESS );
        m_planet_atmosphere_binder_mirror[i] = new PlanetDetailsBinder( PLANET_RAY * 1000.0, PLANET_ATMO_THICKNESS );
        m_planet_atmosphere_binder_mirror[i]->SetMirrorMode( true );

        m_planet_clouds_binder[i] = new PlanetDetailsBinder( PLANET_RAY * 1000.0, PLANET_ATMO_THICKNESS );
        m_planet_cloudslow_binder[i] = new PlanetDetailsBinder( PLANET_RAY * 1000.0, PLANET_ATMO_THICKNESS );
        m_planet_clouds_binder_mirror[i] = new PlanetDetailsBinder( PLANET_RAY * 1000.0, PLANET_ATMO_THICKNESS );

        m_planet_clouds_binder_mirror[i]->SetMirrorMode( true );

        m_planet_occlusion_binder[i] = new DrawSpace::SphericalLOD::Binder();
    }

    Shader* hm_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planethm.vso", true ) );
    Shader* hm_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planethm.pso", true ) );
    hm_vshader->LoadFromFile();
    hm_pshader->LoadFromFile();


    Shader* colors_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.vso", true ) );
    Shader* colors_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_ht.pso", true ) );
    colors_vshader->LoadFromFile();
    colors_pshader->LoadFromFile();

    
    Shader* planet_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_surface.vso", true ) );
    Shader* planet_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_surface.pso", true ) );

    planet_vshader->LoadFromFile();
    planet_pshader->LoadFromFile();




    Shader* planet_water_bump_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_water_bump.vso", true ) );
    Shader* planet_water_bump_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_water_bump.pso", true ) );

    planet_water_bump_vshader->LoadFromFile();
    planet_water_bump_pshader->LoadFromFile();



    
    Shader* planet_atmo_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_atmosphere.vso", true ) );
    Shader* planet_atmo_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_atmosphere.pso", true ) );
    
    planet_atmo_vshader->LoadFromFile();
    planet_atmo_pshader->LoadFromFile();


    Shader* planet_atmo_vshader2 = _DRAWSPACE_NEW_( Shader, Shader( "planet_atmosphere.vso", true ) );
    Shader* planet_atmo_pshader2 = _DRAWSPACE_NEW_( Shader, Shader( "planet_atmosphere.pso", true ) );
    
    planet_atmo_vshader2->LoadFromFile();
    planet_atmo_pshader2->LoadFromFile();


    Shader* planet_clouds_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_clouds.vso", true ) );
    Shader* planet_clouds_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_clouds.pso", true ) );
    
    planet_clouds_vshader->LoadFromFile();
    planet_clouds_pshader->LoadFromFile();


    Shader* planet_occ_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_occlusion.vso", true ) );
    Shader* planet_occ_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet_occlusion.pso", true ) );
    
    planet_occ_vshader->LoadFromFile();
    planet_occ_pshader->LoadFromFile();


    Texture* texture_th_pixels = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_pixels_16.jpg" ) );
    texture_th_pixels->LoadFromFile();

    Texture* texture_th_splatting = _DRAWSPACE_NEW_( Texture, Texture( "earth_th_splatting_16.jpg" ) );
    texture_th_splatting->LoadFromFile();

    Texture* texture_bump_water = _DRAWSPACE_NEW_( Texture, Texture( "water.png" ) );
    texture_bump_water->LoadFromFile();

    Texture* texture_clouds = _DRAWSPACE_NEW_( Texture, Texture( "storm_clouds_8k.jpg" ) );
    texture_clouds->LoadFromFile();


    SphericalLOD::Config config;

    config.m_lod0base = 19000.0;
    config.m_ground_layer = 0;
    config.m_nbLODRanges_inertBodies = NB_LOD_INERTBODIES;
    config.m_nbLODRanges_freeCameras = NB_LOD_FREECAMERAS;

    Fx* collisions_fx = new Fx;
    collisions_fx->AddShader( hm_vshader );
    collisions_fx->AddShader( hm_pshader );
    collisions_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    collisions_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );


    for( int i = 0; i < 6; i++ )
    {
        m_planet_collisions_binder[i]->SetFx( collisions_fx );
    }



    Fx* climate_fx = new Fx;
    climate_fx->AddShader( colors_vshader );
    climate_fx->AddShader( colors_pshader );

    for( int i = 0; i < 6; i++ )
    {
        m_planet_climate_binder[i]->SetFx( climate_fx );
    }



    m_details_fx = new Fx;

    m_details_fx->AddShader( planet_vshader );
    m_details_fx->AddShader( planet_pshader );


    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    //m_details_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );

    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    //m_details_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );


    m_details_fx->SetRenderStateUniqueQueueID( "planet01" ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu

    for( int i = 0; i < 6; i++ )
    {
        m_planet_detail_binder[i]->SetFx( m_details_fx );
        
        m_planet_detail_binder[i]->SetTexture( texture_th_pixels, 0 );
        m_planet_detail_binder[i]->SetTexture( texture_th_splatting, 1 );
    }



    Fx* water_bump_fx = new Fx;


    water_bump_fx->AddShader( planet_water_bump_vshader );
    water_bump_fx->AddShader( planet_water_bump_pshader );


    water_bump_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    water_bump_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    //water_bump_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );

    water_bump_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    water_bump_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    //water_bump_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    for( int i = 0; i < 6; i++ )
    {
        m_planet_waterbump_binder[i]->SetFx( water_bump_fx );
        m_planet_waterbump_binder[i]->SetTexture( m_wavespass->GetTargetTexture(), 0 );
    }





    Fx* atmo_fx = new Fx;

    atmo_fx->AddShader( planet_atmo_vshader );
    atmo_fx->AddShader( planet_atmo_pshader );

    atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );

    atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
    //atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    atmo_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );


    atmo_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    atmo_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    //atmo_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );
    atmo_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    atmo_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );


    for( int i = 0; i < 6; i++ )
    {
        m_planet_atmosphere_binder[i]->SetFx( atmo_fx );
    }


    Fx* atmo_mirror_fx = new Fx;

    atmo_mirror_fx->AddShader( planet_atmo_vshader2 );
    atmo_mirror_fx->AddShader( planet_atmo_pshader2 );

    atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    
    atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );
    
    atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    //atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    atmo_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );


    atmo_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    atmo_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    //atmo_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );
    atmo_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    //atmo_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    for( int i = 0; i < 6; i++ )
    {
        m_planet_atmosphere_binder_mirror[i]->SetFx( atmo_mirror_fx );
    }


    m_clouds_fx = new Fx;

    m_clouds_fx->AddShader( planet_clouds_vshader );
    m_clouds_fx->AddShader( planet_clouds_pshader );

    m_clouds_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_clouds_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_clouds_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_clouds_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_clouds_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_clouds_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_clouds_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );    
    m_clouds_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );


    m_clouds_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_clouds_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_clouds_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    m_clouds_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );


    //m_clouds_fx->SetRenderStateUniqueQueueID( "planet01_clouds" ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu


    for( int i = 0; i < 6; i++ )
    {
        m_planet_clouds_binder[i]->SetFx( m_clouds_fx );
        m_planet_clouds_binder[i]->SetTexture( texture_clouds, 0 );
    }




    Fx* cloudslow_fx = new Fx;

    cloudslow_fx->AddShader( planet_clouds_vshader );
    cloudslow_fx->AddShader( planet_clouds_pshader );

    cloudslow_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    cloudslow_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );
    cloudslow_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    cloudslow_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    cloudslow_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    cloudslow_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    cloudslow_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );    
    cloudslow_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );


    cloudslow_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    cloudslow_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    cloudslow_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    cloudslow_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );


    //cloudslow_fx->SetRenderStateUniqueQueueID( "planet01_clouds" ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu

    for( int i = 0; i < 6; i++ )
    {
        m_planet_cloudslow_binder[i]->SetFx( cloudslow_fx );
        m_planet_cloudslow_binder[i]->SetTexture( texture_clouds, 0 );
    }



    m_clouds_mirror_fx = new Fx;

    m_clouds_mirror_fx->AddShader( planet_clouds_vshader );
    m_clouds_mirror_fx->AddShader( planet_clouds_pshader );

    m_clouds_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_clouds_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_clouds_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_clouds_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_clouds_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_clouds_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_clouds_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );    
    m_clouds_mirror_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );


    m_clouds_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_clouds_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_clouds_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

    m_clouds_mirror_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );


    //m_clouds_mirror_fx->SetRenderStateUniqueQueueID( "planet01_clouds" ); // parce qu'on va updater le renderstate ENABLEZBUFFER pendant le rendu


    for( int i = 0; i < 6; i++ )
    {
        m_planet_clouds_binder_mirror[i]->SetFx( m_clouds_mirror_fx );
        m_planet_clouds_binder_mirror[i]->SetTexture( texture_clouds, 0 );
    }



    Fx* occ_fx = new Fx;

    occ_fx->AddShader( planet_occ_vshader );
    occ_fx->AddShader( planet_occ_pshader );

    occ_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    //occ_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );

    occ_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    //occ_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    for( int i = 0; i < 6; i++ )
    {
        m_planet_occlusion_binder[i]->SetFx( occ_fx );
    }



    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    for( int i = 0; i < 6; i++ )
    {
        m_planet_collisions_binder[i]->SetRenderer( renderer );
        m_planet_detail_binder[i]->SetRenderer( renderer );
        m_planet_climate_binder[i]->SetRenderer( renderer );

        m_planet_atmosphere_binder[i]->SetRenderer( renderer );
        m_planet_atmosphere_binder_mirror[i]->SetRenderer( renderer );
        m_planet_clouds_binder[i]->SetRenderer( renderer );
        m_planet_cloudslow_binder[i]->SetRenderer( renderer );

        m_planet_clouds_binder_mirror[i]->SetRenderer( renderer );

        m_planet_occlusion_binder[i]->SetRenderer( renderer );

        m_planet_waterbump_binder[i]->SetRenderer( renderer );
    }

    

    
    SphericalLOD::Config::LayerDescriptor planet_surface;
    planet_surface.enable_collisions = true;
    planet_surface.enable_datatextures = true;
    planet_surface.enable_lod = true;
    planet_surface.min_lodlevel = 0;
    planet_surface.ray = PLANET_RAY;
    for( int i = 0; i < 6; i++ )
    {
        planet_surface.groundCollisionsBinder[i] = m_planet_collisions_binder[i];
        planet_surface.patchTexturesBinder[i] = m_planet_climate_binder[i];
    }

    config.m_layers_descr.push_back( planet_surface );


    SphericalLOD::Config::LayerDescriptor planet_atmosphere;
    planet_atmosphere.enable_collisions = false;
    planet_atmosphere.enable_datatextures = false;
    planet_atmosphere.enable_lod = false;
    planet_atmosphere.min_lodlevel = 0;
    planet_atmosphere.ray = PLANET_RAY + PLANET_ATMO_THICKNESS / 1000.0;

    for( int i = 0; i < 6; i++ )
    {
        planet_atmosphere.groundCollisionsBinder[i] = NULL;
        planet_atmosphere.patchTexturesBinder[i] = NULL;
    }

    config.m_layers_descr.push_back( planet_atmosphere );


    SphericalLOD::Config::LayerDescriptor planet_clouds;
    planet_clouds.enable_collisions = false;
    planet_clouds.enable_datatextures = false;
    planet_clouds.enable_lod = false;
    planet_clouds.min_lodlevel = 0;
    planet_clouds.ray = PLANET_RAY + FLAT_CLOUDS_ALT / 1000.0;

    for( int i = 0; i < 6; i++ )
    {
        planet_clouds.groundCollisionsBinder[i] = NULL;
        planet_clouds.patchTexturesBinder[i] = NULL;
    }

    config.m_layers_descr.push_back( planet_clouds );



    m_planet = _DRAWSPACE_NEW_( DrawSpace::SphericalLOD::Root, DrawSpace::SphericalLOD::Root( "planet01", PLANET_RAY, &m_timer, config ) );



    
    for( int i = 0; i < 6; i++ )
    {
        m_planet->RegisterSinglePassSlot( m_texturepass, m_planet_detail_binder[i], i, DrawSpace::SphericalLOD::Body::LOWRES_SKIRT_MESHE, 0, 2000 );
        m_planet->RegisterSinglePassSlot( m_occlusionpass, m_planet_occlusion_binder[i], i, DrawSpace::SphericalLOD::Body::LOWRES_SKIRT_MESHE, 0, 1000 );

        m_planet->RegisterSinglePassSlot( m_waterbumppass, m_planet_waterbump_binder[i], i, DrawSpace::SphericalLOD::Body::LOWRES_SKIRT_MESHE, 0, 2000 );
    }

    
    for( int i = 0; i < 6; i++ )
    {
        m_planet->RegisterSinglePassSlot( m_texturepass, m_planet_atmosphere_binder[i], i, DrawSpace::SphericalLOD::Body::HIRES_MESHE, 1, 1000 );
        m_planet->RegisterSinglePassSlot( m_texturemirrorpass, m_planet_atmosphere_binder_mirror[i], i, DrawSpace::SphericalLOD::Body::HIRES_MESHE, 1, 1000 );
    }


    
    for( int i = 0; i < 6; i++ )
    {
        m_flatcloudslow_rnode[i] = m_planet->RegisterSinglePassSlot( m_texturepass, m_planet_cloudslow_binder[i], i, DrawSpace::SphericalLOD::Body::AVGRES_MESHE, 2, 1500 );
        m_flatcloudshigh_rnode[i] = m_planet->RegisterSinglePassSlot( m_texturepass, m_planet_clouds_binder[i], i, DrawSpace::SphericalLOD::Body::AVGRES_MESHE, 2, 3000 );


        m_planet->RegisterSinglePassSlot( m_texturemirrorpass, m_planet_clouds_binder_mirror[i], i, DrawSpace::SphericalLOD::Body::AVGRES_MESHE, 2, 1500 );
    }
    
    
    // TEMPORAIRE
    m_planet->SetOrbitDuration( 0.333 );
    m_planet->SetRevolutionTiltAngle( 8.0 );    
    m_planet->SetRevolutionDuration( 1.0 );
    

    m_planet_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::SphericalLOD::Root>, SceneNode<DrawSpace::SphericalLOD::Root>( "planet01" ) );
    m_planet_node->SetContent( m_planet );

    m_scenenodegraph.RegisterNode( m_planet_node );

    
    m_longlatmvt3_node->LinkTo( m_planet_node );
    m_longlatmvt4_node->LinkTo( m_planet_node );


    m_planet_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 270000000.0, 0.99, 0.0, 0.0, 0.0, 0.0, 0.333, 25.0, 1.0, NULL ) );
    m_planet_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "planet01_orbit" ) );
    m_planet_orbit_node->SetContent( m_planet_orbit );

    m_scenenodegraph.AddNode( m_planet_orbit_node );
    m_scenenodegraph.RegisterNode( m_planet_orbit_node );

    // TEMPORAIRE
    m_planet_node->LinkTo( m_planet_orbit_node );
    //m_scenenodegraph.AddNode( m_planet_node ); // TEMPORAIRE

    m_building_collider->SetReferentOrbiter( m_planet );
    m_socle_collider->SetReferentOrbiter( m_planet );

    ///////////////////////////////////

    m_planet->RegisterScenegraphCallbacks( m_scenenodegraph );


    for( int i = 0; i < 6; i++ )
    {
        m_planet_detail_binder[i]->SetPlanetNode( m_planet_node );
        m_planet_detail_binder[i]->SetCloudsTexture( texture_clouds );

        m_planet_atmosphere_binder[i]->SetPlanetNode( m_planet_node );
        m_planet_atmosphere_binder[i]->SetCloudsTexture( texture_clouds );

        m_planet_atmosphere_binder_mirror[i]->SetPlanetNode( m_planet_node );
        m_planet_atmosphere_binder_mirror[i]->SetCloudsTexture( texture_clouds );


        m_planet_clouds_binder[i]->SetPlanetNode( m_planet_node );
        m_planet_clouds_binder[i]->SetCloudsTexture( texture_clouds );

        m_planet_cloudslow_binder[i]->SetPlanetNode( m_planet_node );
        m_planet_cloudslow_binder[i]->SetCloudsTexture( texture_clouds );

        m_planet_clouds_binder_mirror[i]->SetPlanetNode( m_planet_node );
        m_planet_clouds_binder_mirror[i]->SetCloudsTexture( texture_clouds );

    }

    ////////////////////////


    /// volumetric clouds

    LongLatMovement* clouds_ll = new LongLatMovement();
   
    m_clouds_ll_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>, DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>( "impostor2_ll" ) );
    m_clouds_ll_node->SetContent( clouds_ll );

    //m_clouds_ll_node->GetContent()->Init( 274.0, 0.0, ( PLANET_RAY * 1000 ) + VOLUMETRIC_CLOUDS_ALT, 0.0, 0.0 );
    clouds_ll->Init( 274.0, 0.0, ( PLANET_RAY * 1000 ) + VOLUMETRIC_CLOUDS_ALT, 0.0, 0.0 );

    m_scenenodegraph.RegisterNode( m_clouds_ll_node );
    m_clouds_ll_node->LinkTo( m_planet_node );


    
    m_clouds = _DRAWSPACE_NEW_( DrawSpace::Clouds, DrawSpace::Clouds );
    m_clouds->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_clouds->EnableDetails( false );


    /////////////////////////////////////////////////
    m_clouds_procedural_rules = new DrawSpace::Procedural::RulesPackage( m_clouds->GetProceduralCallback() );

    m_clouds_procedural_rules->InitializeSeedBase( 56645 );
    m_clouds_procedural_rules->Run( CLOUDS_PROCEDURALRULES_FILE, " " );

    m_clouds_procedural_rules->GetRootParser()->GetRules()->Apply();


    m_clouds->ImpostorsInit();
    


    m_clouds->RegisterPassSlot( m_texturepass );
    //m_clouds->RegisterPassSlot( m_texturemirrorpass );

    
    m_clouds->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_cloudsimpostor.vso", true ) ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_cloudsimpostor.pso", true ) ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );
    

    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    

    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_clouds->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );




    m_clouds->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "clouds.bmp" ) ), 0 );
    m_clouds->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    
    m_clouds->GetNodeFromPass( m_texturepass )->SetOrderNumber( 2500 );
    //m_clouds->GetNodeFromPass( m_texturepass )->SetOrderNumber( 12000 );


    m_clouds->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "flags", 24 );
    m_clouds->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "flags", Vector( 0.0, PLANET_RAY * 1000.0, CLOUDS_FOG_DENSITY, 0.0 ) );

    m_clouds->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "clouds_dims", 25 );
    m_clouds->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "clouds_dims", Vector( CLOUDS_HEIGHT / 2.0, -CLOUDS_HEIGHT / 2.0, 1.0, 0.65 ) );

    m_clouds->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "view_pos", 26 );
    m_clouds->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "planet_pos", 27 );


    m_clouds->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "fog_color", 0 );
    m_clouds->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "fog_color", Vector( 0.45, 0.63, 0.78, 1.0 ) );

    m_clouds->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "ambient_lit", 1 );
    

    m_clouds->GetNodeFromPass( m_texturepass )->SetDrawingState( false );


    m_clouds_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Clouds>, SceneNode<DrawSpace::Clouds>( "clouds_1" ) );

    m_clouds_node->SetContent( m_clouds );


    m_scenenodegraph.RegisterNode( m_clouds_node );

    m_clouds_node->LinkTo( m_clouds_ll_node );

    
    
    m_clouds_low = _DRAWSPACE_NEW_( DrawSpace::Clouds, DrawSpace::Clouds );
    m_clouds_low->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_clouds_low->EnableDetails( false );


    /////////////////////////////////////////////////
    m_clouds_low_procedural_rules = new DrawSpace::Procedural::RulesPackage( m_clouds_low->GetProceduralCallback() );

    m_clouds_low_procedural_rules->InitializeSeedBase( 56645 );
    m_clouds_low_procedural_rules->Run( CLOUDS_PROCEDURALRULES_FILE, " " );

    m_clouds_low_procedural_rules->GetRootParser()->GetRules()->Apply();


    m_clouds_low->ImpostorsInit();
    


    m_clouds_low->RegisterPassSlot( m_texturepass );
    m_clouds_low->RegisterPassSlot( m_texturemirrorpass );

    
    m_clouds_low->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_cloudsimpostor.vso", true ) ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_cloudsimpostor.pso", true ) ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );
    

    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    

    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );




    m_clouds_low->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "clouds.bmp" ) ), 0 );
    m_clouds_low->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    
    m_clouds_low->GetNodeFromPass( m_texturepass )->SetOrderNumber( 1600 );


    m_clouds_low->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "flags", 24 );
    m_clouds_low->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "flags", Vector( 0.0, PLANET_RAY * 1000.0, CLOUDS_FOG_DENSITY, 0.0 ) );

    m_clouds_low->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "clouds_dims", 25 );
    m_clouds_low->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "clouds_dims", Vector( CLOUDS_HEIGHT / 2.0, -CLOUDS_HEIGHT / 2.0, 1.0, 0.65 ) );

    m_clouds_low->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "view_pos", 26 );
    m_clouds_low->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "planet_pos", 27 );


    m_clouds_low->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "fog_color", 0 );
    m_clouds_low->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "fog_color", Vector( 0.45, 0.63, 0.78, 1.0 ) );

    m_clouds_low->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "ambient_lit", 1 );
    

    m_clouds_low->GetNodeFromPass( m_texturepass )->SetDrawingState( false );

    

    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_cloudsimpostor.vso", true ) ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet_cloudsimpostor.pso", true ) ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "none" ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );
    

    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    

    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );





    
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "clouds.bmp" ) ), 0 );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->GetTexture( 0 )->LoadFromFile();

    
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetOrderNumber( 1600 );


    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "flags", 24 );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "flags", Vector( 1.0, PLANET_RAY * 1000.0, CLOUDS_FOG_DENSITY, 0.0 ) );

    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "clouds_dims", 25 );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "clouds_dims", Vector( 2500, -2500, 1.0, 0.65 ) );

    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "view_pos", 26 );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 0, "planet_pos", 27 );


    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 1, "fog_color", 0 );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "fog_color", Vector( 0.45, 0.63, 0.78, 1.0 ) );


    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->AddShaderParameter( 1, "ambient_lit", 1 );
    

    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetDrawingState( false );


    m_clouds_low_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Clouds>, SceneNode<DrawSpace::Clouds>( "clouds_1_low" ) );

    m_clouds_low_node->SetContent( m_clouds_low );


    m_scenenodegraph.RegisterNode( m_clouds_low_node );

    m_clouds_low_node->LinkTo( m_clouds_ll_node );


    m_clouds_state_machine = _DRAWSPACE_NEW_( CloudsStateMachine, CloudsStateMachine( m_clouds, m_clouds_low, clouds_ll ) );
    
    //m_planet->SetGravityState( false );
}

void dsAppClient::init_star_impostor( void )
{
    ImpostorsDisplayList idl;
    ImpostorsDisplayListEntry idle;


    m_star_impostor = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );
    m_star_impostor->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );


    idle.width_scale = 55000000.0;
    idle.height_scale = 55000000.0;

    idle.u1 = 0.0;
    idle.v1 = 0.0;
    idle.u2 = 1.0;
    idle.v2 = 0.0;
    idle.u3 = 1.0;
    idle.v3 = 1.0;
    idle.u4 = 0.0;
    idle.v4 = 1.0;

    idle.localpos[0] = 0.0;
    idle.localpos[1] = 0.0;
    idle.localpos[2] = 0.0;
    
    idl.push_back( idle );


    
    m_star_impostor->SetImpostorsDisplayList( idl );
    m_star_impostor->ImpostorsInit();

    m_star_impostor->RegisterPassSlot( m_texturepass );

    m_star_impostor->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor.vsh", false ) ) );
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor.psh", false ) ) );
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one" ) );
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );

    m_star_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_star_impostor->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "star_far.bmp" ) ), 0 );
    m_star_impostor->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    m_star_impostor->GetNodeFromPass( m_texturepass )->SetOrderNumber( 6000 );


    m_star_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "globalscale", 24 );
    m_star_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "globalscale", Vector( 1.0, 1.0, 0.0, 0.0 ) );

    m_star_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "flags", 0 );
    m_star_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "flags", Vector( 0.0, 0.0, 0.0, 0.0 ) );

    m_star_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "color", 1 );
    m_star_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );


    m_star_impostor_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "star_impostor" ) );

    m_star_impostor_node->SetContent( m_star_impostor );

    m_scenenodegraph.AddNode( m_star_impostor_node );
    m_scenenodegraph.RegisterNode( m_star_impostor_node );


    idl.clear();


    m_starhalo_impostor = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );
    m_starhalo_impostor->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );


    idle.width_scale =  125000000.0;
    idle.height_scale = 85000000.0;

    idle.u1 = 0.0;
    idle.v1 = 0.0;
    idle.u2 = 1.0;
    idle.v2 = 0.0;
    idle.u3 = 1.0;
    idle.v3 = 1.0;
    idle.u4 = 0.0;
    idle.v4 = 1.0;

    idle.localpos[0] = 0.0;
    idle.localpos[1] = 0.0;
    idle.localpos[2] = 0.0;
    
    idl.push_back( idle );


    
    m_starhalo_impostor->SetImpostorsDisplayList( idl );
    m_starhalo_impostor->ImpostorsInit();

    m_starhalo_impostor->RegisterPassSlot( m_texturepass );

    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor.vsh", false ) ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "screenimpostor.psh", false ) ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one" ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );

    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "ray.png" ) ), 0 );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->SetOrderNumber( 6001 );


    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 0, "globalscale", 24 );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "globalscale", Vector( 1.0, 1.0, 0.0, 0.0 ) );

    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "flags", 0 );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "flags", Vector( 0.0, 0.0, 0.0, 0.0 ) );

    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->AddShaderParameter( 1, "color", 1 );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );


    m_starhalo_impostor_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "starhalo_impostor" ) );

    m_starhalo_impostor_node->SetContent( m_starhalo_impostor );

    m_scenenodegraph.RegisterNode( m_starhalo_impostor_node );
    m_starhalo_impostor_node->LinkTo( m_star_impostor_node );
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
    
    // TEMPORAIRE

    cube_params.initial_attitude.Translation( 269000000.0, 0.0, 5900000.0 );
    //cube_params.initial_attitude.Translation( 269000000.0, 0.0, 59000000.0 );
    //cube_params.initial_attitude.Translation( 0.0, 0.0, 59000000 );


    m_ship = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Rocket, DrawSpace::Dynamics::Rocket( &m_world, cube_params ) );
   
    m_ship_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Rocket>, SceneNode<DrawSpace::Dynamics::Rocket>( "rocket_body" ) );
    m_ship_node->SetContent( m_ship );

    m_scenenodegraph.AddNode( m_ship_node );
    m_scenenodegraph.RegisterNode( m_ship_node );

    m_ship_drawable_node->LinkTo( m_ship_node );



    m_ship_drawable->SetDrawingState( m_texturepass, false );

    
    //////////////////////////////////////////////////////////////
}


void dsAppClient::init_cameras( void )
{
    m_camera3 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera3->Lock( m_ship_node );
    m_camera3->SetReferentBody( m_ship );
    
    m_camera3_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera3" ) );
    m_camera3_node->SetContent( m_camera3 );
    m_scenenodegraph.RegisterNode( m_camera3_node );


    m_circular_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::CircularMovement, DrawSpace::Core::CircularMovement );
    m_circular_mvt->Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( 185.0, 40.0, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 270.0, 0.0, 0.0 );

    m_circmvt_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::CircularMovement>, SceneNode<DrawSpace::Core::CircularMovement>( "circmvt" ) );


    m_circmvt_node->SetContent( m_circular_mvt );
    m_scenenodegraph.RegisterNode( m_circmvt_node );


    m_camera_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "cameradebug_transfo" ) );
    m_camera_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix cameradebug_pos;
    cameradebug_pos.Translation( 0.0, -20.0, 0.0 );
    m_camera_transfo_node->GetContent()->PushMatrix( cameradebug_pos );

    m_scenenodegraph.RegisterNode( m_camera_transfo_node );
    m_camera_transfo_node->LinkTo( m_ship_node );



    m_camera3_node->LinkTo( m_circmvt_node );

    //m_camera3_node->LinkTo( m_camera_transfo_node );
    

    m_circmvt_node->LinkTo( m_ship_node );


    m_camera_occ = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );

    
    m_camera_occ_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera_occ" ) );
    m_camera_occ_node->SetContent( m_camera_occ );
    m_scenenodegraph.RegisterNode( m_camera_occ_node );

    m_camera_occ_node->LinkTo( m_camera3_node );

    // TEMPORAIRE
    m_camera_occ->Lock( m_star_impostor_node );


    /////////////////////////////////////////////////////////


    m_camera5 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );

    m_camera5->SetReferentBody( m_planet );
    m_camera5_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera5" ) );
    m_camera5_node->SetContent( m_camera5 );

    m_scenenodegraph.RegisterNode( m_camera5_node );

    m_longlat_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    //m_longlat_mvt->Init( 274.0, 0.0, ( PLANET_RAY * 1000 ) + 93.0 + 2.0, 0.0, 0.0 );
    m_longlat_mvt->Init( 274.0, 0.0, ( PLANET_RAY * 1000 ) + 8.0 + 3.0, 0.0, 0.0 );

    m_longlatmvt_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::LongLatMovement>, SceneNode<DrawSpace::Core::LongLatMovement>( "longlatmvt_node" ) );
    m_longlatmvt_node->SetContent( m_longlat_mvt );

    m_scenenodegraph.RegisterNode( m_longlatmvt_node );



    m_fps_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::FPSMovement, DrawSpace::Core::FPSMovement );

    Vector voidpos;
    m_fps_mvt->Init( voidpos );
    

    m_fpsmvt_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FPSMovement>, SceneNode<DrawSpace::Core::FPSMovement>( "fpsmvt" ) );
    m_fpsmvt_node->SetContent( m_fps_mvt );
    m_scenenodegraph.RegisterNode( m_fpsmvt_node );

    m_camera5_node->LinkTo( m_fpsmvt_node );

    m_fpsmvt_node->LinkTo( m_longlatmvt_node );
    m_longlatmvt_node->LinkTo( m_planet_node );
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
    m_occlusionpass->GetRenderingQueue()->UpdateOutputQueue();
    m_zoompass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturemirrorpass->GetRenderingQueue()->UpdateOutputQueue();
    m_wavespass->GetRenderingQueue()->UpdateOutputQueue();
    m_waterbumppass->GetRenderingQueue()->UpdateOutputQueue();

    m_zoompass->GetTargetTexture()->AllocTextureContent();
    m_zoom_texture_content = m_zoompass->GetTargetTexture()->GetTextureContentPtr();
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
}


void dsAppClient::render_universe( void )
{
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_scenenodegraph.ComputeTransformations( m_timer );

    Matrix planet_transf;

    m_planet_node->GetFinalTransform( planet_transf );


    Vector invariantPos;

    if( m_curr_camera == m_camera5 )
    {
        m_planet->GetLayerFromCamera( "camera5", 0 )->GetBody()->GetInvariantViewerPos( invariantPos );
    }
    else
    {
        m_planet->GetLayerFromInertBody( m_ship, 0 )->GetBody()->GetInvariantViewerPos( invariantPos );
    }

    
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "view_pos", invariantPos );


    //////////////////////////////////////////////////////////////////////

    Vector playerpos; // pos viewer relative à la planete
    Vector playerpos_longlatalt;  // meme chose mais en spherique

    Matrix planet_transf_notrans;

    planet_transf_notrans = planet_transf;

    planet_transf_notrans.ClearTranslation();
    planet_transf_notrans.Inverse();

    planet_transf_notrans.Transform( &invariantPos, &playerpos );

    Utils::Maths::CartesiantoSpherical( playerpos, playerpos_longlatalt );


    m_clouds_state_machine->UpdateViewerSphericalPos( playerpos_longlatalt[1], playerpos_longlatalt[2], playerpos_longlatalt[0] );
  
    //////////////////////////////////////////////////////////////////////


    Vector planet_pos;

    planet_pos[0] = planet_transf( 3, 0 );
    planet_pos[1] = planet_transf( 3, 1 );
    planet_pos[2] = planet_transf( 3, 2 );
    planet_pos[3] = 1.0;

    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "planet_pos", planet_pos );



    for( int i = 0; i < 6; i++ )
    {
        m_planet_detail_binder[i]->Update();
        m_planet_atmosphere_binder[i]->Update();
        m_planet_atmosphere_binder_mirror[i]->Update();
        m_planet_clouds_binder[i]->Update();
        m_planet_cloudslow_binder[i]->Update();
        m_planet_clouds_binder_mirror[i]->Update();

    }

    m_text_widget->SetVirtualTranslation( 100, 75 );
    m_text_widget_2->SetVirtualTranslation( -60, 160 );

    char distance[64];
    sprintf( distance, "%.3f km", m_reticle_widget->GetLastDistance() / 1000.0 );

    m_text_widget_2->SetText( -40, 0, 30, dsstring( distance ), DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
    m_reticle_widget->Transform();
    //m_reticle_widget->Draw(); // pour ne plus afficher le reticule
    

    //////////////////////////////////////////////////////////////

    dsreal rel_alt;

    if( m_curr_camera == m_camera5 )
    {
        m_planet->GetCameraRelativeAltitude( "camera5", rel_alt );
    }
    else
    {
        m_planet->GetInertBodyRelativeAltitude( m_ship, rel_alt );
    }

    



    m_finalpass->GetViewportQuad()->SetShaderRealVector( "flags", Vector( rel_alt, m_planet_detail_binder[0]->GetOceansDetailsAlt(), 0.0, 0.0 ) );


    //DrawSpace::SphericalLOD::Patch* current_patch = m_planet->GetFragment( 0 )->GetCurrentPatch();


    m_clouds_state_machine->Run();

    m_wavespass->GetRenderingQueue()->Draw();

    m_planet->DrawSubPasses();

    m_scenenodegraph.SetCurrentCamera( m_curr_camera_name );

    m_waterbumppass->GetRenderingQueue()->Draw();
    m_texturepass->GetRenderingQueue()->Draw();
    m_texturemirrorpass->GetRenderingQueue()->Draw();


    m_scenenodegraph.SetCurrentCamera( "camera_occ" );
    m_occlusionpass->GetRenderingQueue()->Draw();    
    m_zoompass->GetRenderingQueue()->Draw();

    

    

    //m_text_widget->Draw();  // pour ne plus afficher le reticule

    m_finalpass->GetRenderingQueue()->Draw();

    m_finalpass2->GetRenderingQueue()->Draw();


    m_zoompass->GetTargetTexture()->CopyTextureContent();

    // TEMPORAIRE
    
    float star_alpha_occlusion;
    float star_alpha_altitude = 1.0;


    unsigned char* color_ptr = (unsigned char*)m_zoom_texture_content;

    int occlusion_count = 0;
    for( long i = 0; i < 32 * 32; i++ )
    {
        if( *color_ptr == 255 )
        {
            occlusion_count++;
        }

        color_ptr += 4;
    }

    float scale = (float)occlusion_count / (float)( 32 * 32 );

    star_alpha_occlusion = scale;


    star_alpha_altitude = Utils::Maths::Clamp( 0.0, 1.0, ( 1.0 / 0.005 ) * ( rel_alt - 1.005 ) );


    m_star_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "globalscale", Vector( scale, scale, 0.0, 0.0 ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "globalscale", Vector( scale, scale, 0.0, 0.0 ) );

    m_star_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, star_alpha_occlusion * star_alpha_altitude ) );
    m_starhalo_impostor->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, star_alpha_occlusion * star_alpha_altitude ) );
    
    
    ////////////////////////////////////////

    // calcul lumiere ambiante pour les nuages volumetriques


    invariantPos.Normalize();

    PlanetLight l0 = m_planet_detail_binder[0]->GetLight( 0 );


    Vector ambient_lit = l0.m_color;

    ambient_lit.Scale( Utils::Maths::Clamp( 0.0, 1.0, l0.m_dir * invariantPos + 0.35 ) );  // produit scalaire plus un biais

    m_clouds->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "ambient_lit", ambient_lit );
    //m_clouds->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "ambient_lit", ambient_lit );
    m_clouds_low->GetNodeFromPass( m_texturepass )->SetShaderRealVector( "ambient_lit", ambient_lit );
    m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetShaderRealVector( "ambient_lit", ambient_lit );


    ////////////////////////////////////////

    dsreal alt;
    bool hotstate;

    if( m_curr_camera == m_camera5 )
    {
        alt = m_planet->GetLayerFromCamera( "camera5", 0 )->GetBody()->GetHotPointAltitud();
        hotstate = m_planet->GetLayerFromCamera( "camera5", 0 )->GetHotState();
    }
    else
    {
        alt = m_planet->GetLayerFromInertBody( m_ship, 0 )->GetBody()->GetHotPointAltitud();
        hotstate = m_planet->GetLayerFromInertBody( m_ship, 0 )->GetHotState();
    }

    // activer Z buffering seulement si on est pres de la surface
    
    if( rel_alt < ZBUFFER_ACTIVATION_REL_ALT )
    {
        m_details_fx->UpdateRenderStateIn( 0, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    }
    else
    {
        m_details_fx->UpdateRenderStateIn( 0, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    }
    
    if( alt > FLAT_CLOUDS_ALT || false == hotstate )
    {
        //m_clouds_fx->UpdateRenderStateIn( 0, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        //m_clouds_fx->UpdateRenderStateIn( 1, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );

        /*
        for( int i = 0; i < 6; i++ )
        {
            m_flatcloudshigh_rnode[i]->SetDrawingState( true );
            m_flatcloudslow_rnode[i]->SetDrawingState( false );
        }
        */
    }
    else
    {
        //m_clouds_fx->UpdateRenderStateIn( 0, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
        //m_clouds_fx->UpdateRenderStateIn( 1, DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "ccw" ) );

        /*
        for( int i = 0; i < 6; i++ )
        {
            m_flatcloudshigh_rnode[i]->SetDrawingState( false );
            m_flatcloudslow_rnode[i]->SetDrawingState( true );
        }
        */
    }

    
    if( alt > VOLUMETRIC_CLOUDS_ALT )
    {
        m_clouds->GetNodeFromPass( m_texturepass )->SetDrawingState( true );
        m_clouds_low->GetNodeFromPass( m_texturepass )->SetDrawingState( false );
        m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetDrawingState( false );
    }
    else
    {
        m_clouds->GetNodeFromPass( m_texturepass )->SetDrawingState( false );
        m_clouds_low->GetNodeFromPass( m_texturepass )->SetDrawingState( true );
        m_clouds_low->GetNodeFromPass( m_texturemirrorpass )->SetDrawingState( true );    
    }


    long current_fps = m_timer.GetFPS();
    renderer->DrawText( 0, 255, 0, 10, 35, "%d fps", current_fps );

    dsreal speed = m_ship->GetLinearSpeedMagnitude();

    if( m_showinfos )
    {
        dsstring date;
        m_calendar->GetFormatedDate( date );    
        renderer->DrawText( 0, 255, 0, 10, 55, "%s", date.c_str() );

            
        renderer->DrawText( 0, 255, 0, 10, 95, "speed = %.1f km/h ( %.1f m/s) - aspeed = %.1f", speed * 3.6, speed, m_ship->GetAngularSpeedMagnitude() );

        
        if( hotstate )
        {
            renderer->DrawText( 0, 255, 0, 10, 130, "fragment hot" );

            

            if( alt > 10000.0 )
            {
                renderer->DrawText( 0, 255, 0, 10, 145, "hotpoint altitude = %.3f km", alt / 1000.0 );
            }
            else
            {
                renderer->DrawText( 0, 255, 0, 10, 145, "hotpoint altitude = %.1f m", alt );
            }
        
        }


        renderer->DrawText( 0, 255, 0, 10, 220, "relative_alt = %f", rel_alt );


        


        PROCESS_MEMORY_COUNTERS pmc;

        GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof( PROCESS_MEMORY_COUNTERS ) );

        renderer->DrawText( 0, 255, 0, 10, 250, "working set size = %d", pmc.WorkingSetSize );
        renderer->DrawText( 0, 255, 0, 10, 280, "subpasses stack size = %d", m_planet->GetSingleShotSubPassesStackSize() );



        renderer->DrawText( 0, 255, 0, 10, 310, "%.3f %.4f %.4f", playerpos_longlatalt[0] - PLANET_RAY * 1000.0, Utils::Maths::RadToDeg( playerpos_longlatalt[1] ), 
                                                                    Utils::Maths::RadToDeg( playerpos_longlatalt[2] ) );

        renderer->DrawText( 0, 255, 0, 900, 30, "%s", m_deviceDescr.description.c_str() );
    
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
    else
    {
        if( hotstate )
        {

            if( alt > 10000.0 )
            {
                renderer->DrawText( 0, 255, 0, 10, 70, "Altitude = %.3f km", alt / 1000.0 );

                
            }
            else
            {
                renderer->DrawText( 0, 255, 0, 10, 70, "Altitude = %.1f m", alt );
            }
        }

        renderer->DrawText( 0, 255, 0, 10, 130, "time factor = %s", m_timefactor.c_str() );

        renderer->DrawText( 0, 255, 0, 10, 95, "speed = %.1f km/h", speed * 3.6 );
    }
  
    renderer->FlipScreen();

    m_calendar->Run();

    if( m_timer.IsReady() )
    {
        if( m_water_anim_inc )
        {
            if( m_water_anim < 200.0 )
            {
                m_timer.TranslationSpeedInc( &m_water_anim, 1.0 );
            }
            else
            {
                m_water_anim_inc = false;
            }
        }
        else
        {
            if( m_water_anim > 0.0 )
            {
                m_timer.TranslationSpeedDec( &m_water_anim, 1.0 );
            }
            else
            {
                m_water_anim_inc = true;
            }        
        }
    }

    m_wavespass->GetViewportQuad()->SetShaderRealVector( "waves", DrawSpace::Utils::Vector( m_water_anim, 0.0, 0.0, 0.0 ) );
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
                print_init_trace( "star impostor initialisation..." );
                // TEMPORAIRE
                init_star_impostor();
                break;

            case 10:
                print_init_trace( "cameras initialisation..." );
                init_cameras();
                break;

            case 11:
                print_init_trace( "reticle creation..." );
                init_reticle();
                break;

            case 12:
                print_init_trace( "text display assets creation..." );
                init_text_assets();
                break;

            case 13:
                {
                    print_init_trace( "rendering queues update..." );
                    init_rendering_queues();
                }
                break;

            case 14:
                //print_init_trace( "planet atmosphere data initialisation..." );
                break;

            case 15:                
                init_planet_noise();
                break;

            case 16:
                print_init_trace( "calendar initialisation..." );
                init_calendar();
                break;

            case 17:
                print_init_trace( "launching simulation..." );


                
                //m_curr_camera = m_camera3; 
                //m_curr_camera_name = "camera3";
                

                m_mouse_circularmode = true;
                m_curr_camera = m_camera5; 
                m_curr_camera_name = "camera5";


                m_scenenodegraph.SetCurrentCamera( m_curr_camera_name );
                //m_calendar->Startup( 162682566 );
                m_calendar->Startup( 0 );
                m_ready = true;

                renderer->GetDeviceDescr( m_deviceDescr );
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
            m_timefactor = "x1";
            break;

        case 'C':

            m_ship->ApplyDownPitch( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            m_timefactor = "x1";
            break;

        case 'S':

            m_ship->ApplyLeftYaw( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            m_timefactor = "x1";
            break;

        case 'F':

            m_ship->ApplyRightYaw( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            m_timefactor = "x1";
            break;


        case 'Z':

            m_ship->ApplyLeftRoll( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            m_timefactor = "x1";
            break;

        case 'R':

            m_ship->ApplyRightRoll( 50000.0 );

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            m_timefactor = "x1";
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
            m_timefactor = "x1";
            break;

        case VK_F2:

            m_calendar->SetTimeFactor( Calendar::MUL2_TIME );
            m_timefactor = "x2";
            break;

        case VK_F3:

            m_calendar->SetTimeFactor( Calendar::MUL10_TIME );
            m_timefactor = "x10";
            break;

        case VK_F4:

            m_calendar->SetTimeFactor( Calendar::MUL100_TIME );
            m_timefactor = "x100";
            break;

        case VK_F5:

            m_calendar->SetTimeFactor( Calendar::SEC_1HOUR_TIME );
            m_timefactor = "x3600";
            break;


        case VK_F6:

            m_calendar->SetTimeFactor( Calendar::SEC_1DAY_TIME );
            m_timefactor = "x86400";
            break;

        case 'I':
            {
                if( m_curr_camera == m_camera3 )
                {
                    m_curr_camera = m_camera5; 
                    m_curr_camera_name = "camera5";
                    m_mouse_circularmode = true;                
                }
                else if( m_curr_camera == m_camera5 )
                {
                    m_curr_camera = m_camera3; 
                    m_curr_camera_name = "camera3";
                    m_mouse_circularmode = false;
                }
            }
            break;

        case VK_F8:

            if( !m_final_pass_2)
            {
                m_final_pass_2 = true;
            }
            else
            {
                m_final_pass_2 = false;
            }

            m_finalpass2->GetViewportQuad()->SetDrawingState( m_final_pass_2 );
            break;


        case VK_F9:

            m_ship_drawable->SetDrawingState( m_texturepass, false );
            m_showinfos = false;
            break;

        case VK_F7:

            m_ship_drawable->SetDrawingState( m_texturepass, true );
            m_showinfos = true;
            break;

        case 'U':
            {
                _DSTRACE( logger, ">>>>>>>>>>>>>>>>>>>>>>>>>>>> memalloc dump begin <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" );

                //_DSTRACE( logger, ">>>>>>>>>>>>>>>>>>>>>>>>>>>> core dump <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" );
                MemAlloc::GetInstance()->DumpContent();
                _DSTRACE( logger, ">>>>>>>>>>>>>>>>>>>>>>>>>>>> plugin dump <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" );
                DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
                renderer->DumpMemoryAllocs();
                _DSTRACE( logger, ">>>>>>>>>>>>>>>>>>>>>>>>>>>> memalloc dump end   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" );
            }

            break;
    }
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( m_curr_camera == m_camera5 )
    {
	    m_fps_mvt->RotateYaw( - p_dx / 20.0, m_timer );
	    m_fps_mvt->RotatePitch( - p_dy / 20.0, m_timer );
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

void dsAppClient::on_mouseleftbuttondown( DrawSpace::Gui::Widget* p_widget )
{

}