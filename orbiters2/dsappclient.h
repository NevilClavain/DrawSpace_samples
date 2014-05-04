
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class MyPlanet
{
protected:

    typedef DrawSpace::Core::CallBack<MyPlanet, void, int> PlanetEvtCb;

    DrawSpace::Dynamics::World                              m_world;
    dsstring                                                m_name;
    DrawSpace::Dynamics::Orbiter*                           m_orbiter;
    DrawSpace::Planet::Body*                                m_drawable;
    PlanetEvtCb*                                            m_planet_evt_cb;

    dsreal                                                  m_ray;

    bool                                                    m_collision_state;

    std::vector<DrawSpace::Dynamics::InertBody*>            m_attached_bodies;
    DrawSpace::Dynamics::InertBody*                         m_player_body;

    bool                                                    m_player_relative;
    bool                                                    m_suspend_update;

    DrawSpace::Core::Task<MyPlanet>*                        m_task;

    //////// bullet patch build thread
    HANDLE                                                  m_buildmeshe_request_event;

    DrawSpace::Utils::Mutex                                 m_buildmeshe_inputs_mutex;
    DrawSpace::Core::Meshe                                  m_buildmeshe_patchmeshe;

    dsreal                                                  m_buildmeshe_sidelength;
    dsreal                                                  m_buildmeshe_xpos, m_buildmeshe_ypos;
    dsreal                                                  m_buildmeshe_planetray;
    int                                                     m_buildmeshe_patch_orientation;

    bool                                                    m_buildmeshe_collision_state;


    HANDLE                                                  m_buildmeshe_done_event; 


    void on_planet_event( int p_currentface );

public:

    MyPlanet( const dsstring& p_name );
    ~MyPlanet( void );

    DrawSpace::Planet::Body* GetDrawable( void );
    DrawSpace::Dynamics::Orbiter*   GetOrbiter( void );
    DrawSpace::Dynamics::World*     GetWorld( void );

    bool GetCollisionState( void );

    dsreal GetAltitud( void );

    void AttachBody( DrawSpace::Dynamics::InertBody* p_body );
    


    void ApplyGravity( void );

    //void Update( const DrawSpace::Utils::Vector& p_hotpoint );

    void Update( DrawSpace::Dynamics::InertBody* p_player_body );

    void Run( void );

};


class dsAppClient : public DrawSpace::App
{
protected:

    static dsAppClient*                         m_instance;

    DrawSpace::Scenegraph                       m_scenegraph;


    DrawSpace::IntermediatePass*                m_texturepass;
    DrawSpace::FinalPass*                       m_finalpass;


    DrawSpace::Utils::TimeManager               m_timer;

    bool                                        m_mouselb;
    bool                                        m_mouserb;


    DrawSpace::Interface::FontImport*           m_font_import;
    DrawSpace::Core::Font*                      m_font;

    DrawSpace::Camera*                          m_camera;
    DrawSpace::Camera*                          m_camera2;
    DrawSpace::Core::FPSMovement                m_fpsmove;
    DrawSpace::Core::FreeMovement               m_freemove;
    
    DrawSpace::Interface::MesheImport*          m_meshe_import;

    
    DrawSpace::Spacebox*                        m_spacebox;
    

    dsreal                                      m_speed;
    dsreal                                      m_speed_speed;

    DrawSpace::Dynamics::World                  m_world;

    

    DrawSpace::Dynamics::Calendar*              m_calendar;


    MyPlanet*                                   m_planet;

    DrawSpace::Dynamics::Orbit*                 m_orbit;
    DrawSpace::Dynamics::Centroid*              m_centroid;

    DrawSpace::Dynamics::Rocket*                m_ship;
    

    DrawSpace::Chunk*                           m_ship_drawable;

    

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

