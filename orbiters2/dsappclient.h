
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class dsAppClient : public DrawSpace::App
{
protected:


    typedef enum
    {
        HP_NONE,
        HP_ACCELERATE,
        HP_IN,
        HP_CRUISE,
        HP_OUT,
        HP_DECELERATE

    } HyperSpace_state;


    



    typedef DrawSpace::Core::CallBack<dsAppClient, void, DrawSpace::Gui::Widget*>   WidgetEventHandler;


    static dsAppClient*                         m_instance;

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


    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_spacebox_transfo_node;


    
    DrawSpace::Dynamics::CameraPoint*           m_camera2;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera2_node;

    DrawSpace::Core::HeadMovement*              m_head_mvt;
    DrawSpace::Core::SceneNode<DrawSpace::Core::HeadMovement>*          m_headmvt_node;

    DrawSpace::Dynamics::CameraPoint*           m_camera3;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera3_node;

    DrawSpace::Dynamics::CameraPoint*           m_camera4;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera4_node;


    DrawSpace::Core::CircularMovement*          m_circular_mvt;
    DrawSpace::Core::SceneNode<DrawSpace::Core::CircularMovement>*          m_circmvt_node;

    DrawSpace::Dynamics::CameraPoint*           m_camera5;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera5_node;
    DrawSpace::Core::LongLatMovement*           m_longlat_mvt;
    DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>*          m_longlatmvt_node;
    DrawSpace::Core::CircularMovement*          m_circular_mvt2;
    DrawSpace::Core::SceneNode<DrawSpace::Core::CircularMovement>*          m_circmvt2_node;


    DrawSpace::Dynamics::CameraPoint*           m_camera6;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera6_node;

    DrawSpace::Core::SpectatorMovement*         m_spectator_mvt;
    DrawSpace::Core::SceneNode<DrawSpace::Core::SpectatorMovement>*          m_spectmvt_node;


    DrawSpace::Dynamics::CameraPoint*           m_camera7;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera7_node;

    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_camera7_transfo_node;

    DrawSpace::Dynamics::CameraPoint*           m_camera8;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera8_node;

    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_camera8_transfo_node;



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



    DrawSpace::Dynamics::Orbit*                                         m_moon_orbit;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbit>*             m_moon_orbit_node;
    
    DrawSpace::Planetoid::Body*                                         m_moon;
    DrawSpace::Core::SceneNode<DrawSpace::Planetoid::Body>*             m_moon_node;




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


    bool                                        m_draw_spacebox;



    //////////////////////////////////////////////////////////////

    bool                                                                m_draw_hyperspace;


    DrawSpace::Core::SceneNodeGraph                                     m_scenenodegraph_hyperspace;

    DrawSpace::IntermediatePass*                                        m_texturepass_hyperspace;
    DrawSpace::FinalPass*                                               m_finalpass_hyperspace;


    typedef struct
    {

        DrawSpace::Core::SceneNode<DrawSpace::Spacebox>*                    node;
        DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        transfo_node;

    } hp_spacebox; 



    hp_spacebox                                                         m_spacebox1hp;
    hp_spacebox                                                         m_spacebox2hp;

    dsreal                                                              m_spacebox1hp_transz;
    dsreal                                                              m_spacebox1hp_rotz;

    dsreal                                                              m_spacebox2hp_transz;
    dsreal                                                              m_spacebox2hp_rotz;

    hp_spacebox                                                         m_spacebox1bighp;
    dsreal                                                              m_spacebox1bighp_rotz;
    dsreal                                                              m_spacebox1bighp_transz;

    hp_spacebox                                                         m_spacebox2bighp;
    dsreal                                                              m_spacebox2bighp_rotz;
    dsreal                                                              m_spacebox2bighp_transz;


    HyperSpace_state                                                    m_hp_state;
    bool                                                                m_hp;
    
    dsreal                                                              m_hp_transition_transz;


    void on_mouseleftbuttondown( DrawSpace::Gui::Widget* p_widget );


    dsAppClient( void );

    
    void init_hpspacebox( const dsstring& p_scenename, DrawSpace::Pass* p_pass, hp_spacebox* p_hpsb, long p_order );

    void manage_hp( void );

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

