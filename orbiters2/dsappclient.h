
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class dsAppClient : public DrawSpace::App
{
protected:

    typedef DrawSpace::Core::CallBack<dsAppClient, void, DrawSpace::Gui::Widget*>   WidgetEventHandler;


    static dsAppClient*                         m_instance;

    //DrawSpace::Scenegraph                       m_scenegraph;
    DrawSpace::Core::SceneNodeGraph             m_scenenodegraph;


    DrawSpace::IntermediatePass*                m_texturepass;
    DrawSpace::FinalPass*                       m_finalpass;


    DrawSpace::Utils::TimeManager               m_timer;

    bool                                        m_mouselb;
    bool                                        m_mouserb;


    DrawSpace::Interface::FontImport*           m_font_import;
    DrawSpace::Core::Font*                      m_font;


    DrawSpace::Dynamics::CameraPoint*           m_camera;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera_node;

    DrawSpace::Core::FreeMovement               m_freemove;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FreeMovement>*          m_freemove_node;

    DrawSpace::Spacebox*                        m_spacebox;
    DrawSpace::Core::SceneNode<DrawSpace::Spacebox>*                    m_spacebox_node;


    
    DrawSpace::Dynamics::CameraPoint*           m_camera2;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera2_node;

    DrawSpace::Core::HeadMovement*              m_head_mvt;
    DrawSpace::Core::SceneNode<DrawSpace::Core::HeadMovement>*          m_headmvt_node;

    DrawSpace::Dynamics::CameraPoint*           m_camera3;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera3_node;

    DrawSpace::Core::CircularMovement*          m_circular_mvt;
    DrawSpace::Core::SceneNode<DrawSpace::Core::CircularMovement>*          m_circmvt_node;

    //DrawSpace::Dynamics::CameraPoint*           m_camera4;


    DrawSpace::Dynamics::CameraPoint*           m_camera5;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera5_node;
    DrawSpace::Core::LongLatMovement*           m_longlat_mvt;
    DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>*          m_longlatmvt_node;
    DrawSpace::Core::CircularMovement*          m_circular_mvt2;
    DrawSpace::Core::SceneNode<DrawSpace::Core::CircularMovement>*          m_circmvt2_node;


    //DrawSpace::Dynamics::CameraPoint*           m_camera6;
    //DrawSpace::Core::SpectatorMovement*         m_spectator_mvt;


    //DrawSpace::Dynamics::CameraPoint*           m_camera7;
    //DrawSpace::Dynamics::CameraPoint*           m_camera8;


    DrawSpace::Core::LongLatMovement*           m_longlat_mvt2;

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




    //DrawSpace::Dynamics::Orbit*                 m_orbit;
    //DrawSpace::Dynamics::Centroid*              m_centroid;


    //DrawSpace::Planetoid::Body*                 m_moon;



    //DrawSpace::Dynamics::Orbit*                 m_orbit2;
    //DrawSpace::Dynamics::Centroid*              m_centroid2;


    DrawSpace::Dynamics::Rocket*                m_ship;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Rocket>*                    m_ship_node;
    

    DrawSpace::Chunk*                           m_ship_drawable;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                    m_ship_drawable_node;


    DrawSpace::Dynamics::InertBody*             m_cube_body;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*             m_cube_body_node;
    DrawSpace::Chunk*                           m_cube;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                    m_cube_node;



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





    DrawSpace::SystemMouseInputProvider         m_mouse_input;

    WidgetEventHandler*                         m_mouseleftbuttondown_eventhandler;


    void on_mouseleftbuttondown( DrawSpace::Gui::Widget* p_widget );


    dsAppClient( void );

    void compute_player_view_transform( void );

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

