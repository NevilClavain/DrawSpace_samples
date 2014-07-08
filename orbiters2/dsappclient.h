
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class MyPlanet;

class Fragment
{
protected:

    typedef DrawSpace::Core::CallBack2<Fragment, void, DrawSpace::Planet::Body*, int>      PlanetEvtCb;
    typedef DrawSpace::Core::CallBack<Fragment, void, DrawSpace::Core::PropertyPool*>      RunnerEvtCb;


    DrawSpace::Planet::Body*                                    m_planetbody;
    DrawSpace::Dynamics::Collider*                              m_collider;

    DrawSpace::Core::Mediator::Event*                           m_buildmeshe_event;
    DrawSpace::Core::Runner*                                    m_runner;
    DrawSpace::Core::Task<DrawSpace::Core::Runner>*             m_task;

    PlanetEvtCb*                                                m_planet_evt_cb;
    RunnerEvtCb*                                                m_runner_evt_cb;

    bool                                                        m_suspend_update;
    bool                                                        m_collision_state;

    DrawSpace::Utils::Mutex                                     m_meshe_ready_mutex;
    bool                                                        m_meshe_ready;

    dsreal                                                      m_planetray;

    bool                                                        m_hot;


    DrawSpace::Dynamics::CameraPoint*                           m_camera;
    DrawSpace::Dynamics::InertBody*                             m_inertbody;

    bool                                                        m_collisions;


    dsstring                                                    m_name;
    
    void on_meshebuild_request( DrawSpace::Core::PropertyPool* p_args );
    void on_planet_event( DrawSpace::Planet::Body* p_body, int p_currentface );

    void build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, int p_patch_orientation, dsreal p_sidelength, dsreal p_xpos, dsreal p_ypos, DrawSpace::Core::Meshe& p_outmeshe );

public:

    Fragment( const dsstring& p_name, DrawSpace::Planet::Body* p_planetbody, DrawSpace::Dynamics::Collider* p_collider, dsreal p_planetray, bool p_collisions );
    virtual ~Fragment( void );

    void Update( MyPlanet* p_owner );    
    void SetHotState( bool p_hotstate );
    void SetCamera( DrawSpace::Dynamics::CameraPoint* p_camera );
    void SetInertBody( DrawSpace::Dynamics::InertBody* p_body );
    DrawSpace::Dynamics::CameraPoint* GetCamera( void );
    DrawSpace::Dynamics::InertBody* GetInertBody( void );

    void RemoveColliderFromWorld( void );

    DrawSpace::Planet::Body* GetPlanetBody( void );

};


class MyPlanet
{
public:
    typedef DrawSpace::Core::CallBack2<MyPlanet, void, DrawSpace::Scenegraph::CameraEvent, DrawSpace::Core::TransformNode*>         CameraEvtCb;

    typedef DrawSpace::Core::BaseCallback<void, MyPlanet*>                                                                          PlanetRelativeEventHandler;

protected:


    typedef struct
    {
        bool                            attached;
        DrawSpace::Dynamics::InertBody* body;

        Fragment*                       fragment;

    } RegisteredBody;


    typedef enum
    {
        FREE,
        FREE_ON_PLANET,
        INERTBODY_LINKED,

    } CameraType;

    typedef struct
    {
        bool                                update_meshe;

        CameraType                          type;
        DrawSpace::Dynamics::InertBody*     attached_body;

        DrawSpace::Dynamics::CameraPoint*   camera;

        Fragment*                           fragment;


    } RegisteredCamera;

    dsreal                                                      m_ray;

    DrawSpace::Dynamics::World                                  m_world;
    dsstring                                                    m_name;
    DrawSpace::Dynamics::Orbiter*                               m_orbiter;
        
    DrawSpace::Planet::Drawing*                                 m_drawable;
    
    CameraEvtCb*                                                m_camera_evt_cb;
      
    std::map<DrawSpace::Dynamics::InertBody*, RegisteredBody>   m_registered_bodies;
    std::map<dsstring, RegisteredCamera>                        m_registered_camerapoints;
    dsstring                                                    m_current_camerapoint;

    std::vector<Fragment*>                                      m_planetfragments_list;

    void attach_body( DrawSpace::Dynamics::InertBody* p_body );
    void detach_body( DrawSpace::Dynamics::InertBody* p_body );
    void body_find_attached_camera( DrawSpace::Dynamics::InertBody* p_body, std::vector<dsstring>& p_name );
    void on_camera_event( DrawSpace::Scenegraph::CameraEvent p_event, DrawSpace::Core::TransformNode* p_node );

public:

    MyPlanet( const dsstring& p_name, dsreal p_ray );
    ~MyPlanet( void );

    DrawSpace::Planet::Drawing* GetDrawable( void );
    DrawSpace::Planet::Body* GetPlanetBody( void );
    DrawSpace::Dynamics::Orbiter*   GetOrbiter( void );
    DrawSpace::Dynamics::World*     GetWorld( void );

    
    CameraEvtCb* GetCameraEvtCb( void );


    void ApplyGravity( void );

    void ManageBodies( void );
    void Update( void );

    void RegisterInertBody( const dsstring& p_bodyname, DrawSpace::Dynamics::InertBody* p_body );
    void RegisterIncludedInertBody( const dsstring& p_bodyname, DrawSpace::Dynamics::InertBody* p_body, const DrawSpace::Utils::Matrix& p_initmat );
    bool RegisterCameraPoint( DrawSpace::Dynamics::CameraPoint* p_camera, bool p_update_meshe );


    void GetName( dsstring& p_name );

    void GetCameraHotpoint( const dsstring& p_name, DrawSpace::Utils::Matrix& p_outmat );
   
};


class dsAppClient : public DrawSpace::App
{
protected:

    typedef DrawSpace::Core::CallBack<dsAppClient, void, MyPlanet*> PlanetRelativeEvtCb;

    typedef DrawSpace::Core::CallBack2<dsAppClient, void, DrawSpace::Scenegraph::CameraEvent, DrawSpace::Core::TransformNode*>         CameraEvtCb;

    static dsAppClient*                         m_instance;

    DrawSpace::Scenegraph                       m_scenegraph;


    DrawSpace::IntermediatePass*                m_texturepass;
    DrawSpace::FinalPass*                       m_finalpass;


    DrawSpace::Utils::TimeManager               m_timer;

    bool                                        m_mouselb;
    bool                                        m_mouserb;


    DrawSpace::Interface::FontImport*           m_font_import;
    DrawSpace::Core::Font*                      m_font;


    DrawSpace::Dynamics::CameraPoint*           m_camera;
    
    DrawSpace::Dynamics::CameraPoint*           m_camera2;

    DrawSpace::Dynamics::CameraPoint*           m_camera3;
    DrawSpace::Core::CircularMovement*          m_circular_mvt;

    DrawSpace::Dynamics::CameraPoint*           m_camera4;


    DrawSpace::Dynamics::CameraPoint*           m_camera5;
    DrawSpace::Core::LongLatMovement*           m_longlat_mvt;
    DrawSpace::Core::CircularMovement*          m_circular_mvt2;


    DrawSpace::Core::LongLatMovement*           m_longlat_mvt2;


    DrawSpace::Dynamics::CameraPoint*           m_curr_camera;

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


    MyPlanet*                                   m_moon;



    DrawSpace::Dynamics::Orbit*                 m_orbit2;
    DrawSpace::Dynamics::Centroid*              m_centroid2;


    DrawSpace::Dynamics::Rocket*                m_ship;
    

    DrawSpace::Chunk*                           m_ship_drawable;


    DrawSpace::Dynamics::InertBody*             m_cube_body;
    DrawSpace::Chunk*                           m_chunk;



    ///////////////////////////////////////////////////////////

    DrawSpace::Utils::Matrix                    m_player_view_transform;

    DrawSpace::Utils::Vector                    m_player_view_pos;
    dsreal                                      m_player_view_linear_acc[3];
    dsreal                                      m_player_view_linear_acc_2[3];
    dsreal                                      m_player_view_linear_speed[3];

    dsreal                                      m_player_view_linear_speed_clamp_up[3];
    dsreal                                      m_player_view_linear_speed_clamp_down[3];


    dsreal                                      m_player_view_theta;
    dsreal                                      m_player_view_phi;
    dsreal                                      m_player_view_rho;

    dsreal                                      m_player_view_angular_acc[3];
    dsreal                                      m_player_view_angular_acc_2[3];

    dsreal                                      m_player_view_angular_speed_clamp_up[3];
    dsreal                                      m_player_view_angular_speed_clamp_down[3];

    dsreal                                      m_player_view_angular_speed[3];

    ////////////////////////////////////////////////////////////


    DrawSpace::Gui::ReticleWidget*              m_reticle_widget;
    DrawSpace::Gui::TextWidget*                 m_text_widget;



    CameraEvtCb*                                m_camera_evt_cb;


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

