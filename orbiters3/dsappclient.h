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


#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class MultiFractalBinder : public DrawSpace::SphericalLOD::Binder
{
public:

    dsreal                                  m_plains_amplitude;
    dsreal                                  m_mountains_amplitude;
    dsreal                                  m_vertical_offset;

    dsreal                                  m_plains_seed1;
    dsreal                                  m_plains_seed2;
    dsreal                                  m_mix_seed1;
    dsreal                                  m_mix_seed2;

public:

    MultiFractalBinder( void );

    virtual void Initialise( void );
    virtual void Bind( void );
    virtual void Unbind( void );
};

class PlanetClimateBinder : public MultiFractalBinder
{
protected:

public:

    PlanetClimateBinder( void );

    virtual void Initialise( void );
    virtual void Bind( void );
    virtual void Unbind( void );
};

class PlanetDetailsBinder : public MultiFractalBinder
{
protected:

    DrawSpace::Core::SceneNode<DrawSpace::Planetoid::Body>* m_planet_node;

    DrawSpace::Utils::Vector                                m_light0_dir;
    DrawSpace::Utils::Vector                                m_light0_local_dir;

public:

    PlanetDetailsBinder( void );

    virtual void Initialise( void );
    virtual void Bind( void );
    virtual void Unbind( void );

    void SetPlanetNode( DrawSpace::Core::SceneNode<DrawSpace::Planetoid::Body>* p_planet_node );
    void Update( void );

};



class dsAppClient : public DrawSpace::App
{
protected:

    typedef DrawSpace::Core::CallBack<dsAppClient, void, DrawSpace::Gui::Widget*>   WidgetEventHandler;


    static dsAppClient*                         m_instance;

    DrawSpace::Core::SceneNodeGraph             m_scenenodegraph;

    bool                                        m_showinfos;


    DrawSpace::IntermediatePass*                m_texturepass;
    DrawSpace::FinalPass*                       m_finalpass;
    DrawSpace::FinalPass*                       m_finalpass2;


    DrawSpace::Utils::TimeManager               m_timer;

    bool                                        m_mouselb;
    bool                                        m_mouserb;


    DrawSpace::Interface::FontImport*           m_font_import;
    DrawSpace::Core::Font*                      m_font;


    DrawSpace::Spacebox*                        m_spacebox;
    DrawSpace::Core::SceneNode<DrawSpace::Spacebox>*                    m_spacebox_node;


    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_spacebox_transfo_node;


    DrawSpace::Dynamics::CameraPoint*           m_camera3;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera3_node;

    

    DrawSpace::Core::CircularMovement*          m_circular_mvt;
    DrawSpace::Core::SceneNode<DrawSpace::Core::CircularMovement>*          m_circmvt_node;

    


    DrawSpace::Core::LongLatMovement*           m_longlat_mvt3;
    DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>*          m_longlatmvt3_node;


    DrawSpace::Core::LongLatMovement*           m_longlat_mvt4;
    DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>*          m_longlatmvt4_node;


    DrawSpace::Dynamics::CameraPoint*           m_curr_camera;


    
    DrawSpace::Interface::MesheImport*          m_meshe_import;

    
    

    dsreal                                      m_speed;
    dsreal                                      m_speed_speed;

    DrawSpace::Dynamics::World                  m_world;

    

    DrawSpace::Dynamics::Calendar*              m_calendar;


    


    DrawSpace::Dynamics::Orbit*                                         m_planet_orbit;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbit>*             m_planet_orbit_node;
    
    DrawSpace::Planetoid::Body*                                         m_planet;
    DrawSpace::Core::SceneNode<DrawSpace::Planetoid::Body>*             m_planet_node;

    

    MultiFractalBinder*                                                 m_planet_collisions_binder[6];

    PlanetClimateBinder*                                                m_planet_climate_binder[6];

    PlanetDetailsBinder*                                                m_planet_detail_binder[6];



    DrawSpace::Dynamics::Rocket*                m_ship;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Rocket>*                    m_ship_node;
    

    DrawSpace::Chunk*                           m_ship_drawable;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                    m_ship_drawable_node;



    DrawSpace::Chunk*                           m_building;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                    m_building_node;
    
    DrawSpace::Dynamics::Collider*              m_building_collider;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Collider>*                    m_building_collider_node;

    DrawSpace::Chunk*                           m_socle;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                    m_socle_node;
    DrawSpace::Dynamics::Collider*              m_socle_collider;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Collider>*                    m_socle_collider_node;



    ///////////////////////////////////////////////////////////


    DrawSpace::Gui::ReticleWidget*              m_reticle_widget;
    DrawSpace::Gui::TextWidget*                 m_text_widget;
    DrawSpace::Gui::TextWidget*                 m_text_widget_2;


    bool                                        m_show_patch_render;


    DrawSpace::SystemMouseInputProvider         m_mouse_input;

    WidgetEventHandler*                         m_mouseleftbuttondown_eventhandler;

    bool                                        m_ready;

    int                                         m_init_count;

    void init_planet_meshes( void );
    void init_assets_loaders( void );
    void init_passes( void );
    void init_world_physics( void );
    void init_spacebox( void );
    void init_buildings( void );
    void init_planet( void );
    void init_ship( void );
    void init_cameras( void );
    void init_reticle( void );
    void init_text_assets( void );
    void init_rendering_queues( void );
    void init_calendar( void );
    void init_planet_noise( void );

    void render_universe( void );

    void print_init_trace( const dsstring& p_string );

    void on_mouseleftbuttondown( DrawSpace::Gui::Widget* p_widget );


    dsAppClient( void );


public:

    ~dsAppClient( void );

    static dsAppClient* GetInstance( void )
    {
        if( m_instance == NULL )
        {
            m_instance = new dsAppClient;
        }

        DrawSpace::App::m_base_instance = m_instance;
        return m_instance;
    }

    virtual void OnRenderFrame( void );
    virtual bool OnIdleAppInit( void );
    virtual void OnAppInit( void );
    virtual void OnClose( void );
    virtual void OnKeyPress( long p_key ) ;
    virtual void OnEndKeyPress( long p_key );
    virtual void OnKeyPulse( long p_key );
    virtual void OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy );
    virtual void OnMouseLeftButtonDown( long p_xm, long p_ym );
    virtual void OnMouseLeftButtonUp( long p_xm, long p_ym );
    virtual void OnMouseRightButtonDown( long p_xm, long p_ym );
    virtual void OnMouseRightButtonUp( long p_xm, long p_ym );
    virtual void OnAppEvent( WPARAM p_wParam, LPARAM p_lParam );
};

#endif

