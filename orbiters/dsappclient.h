
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>





class dsAppClient : public DrawSpace::App
{
protected:

    typedef DrawSpace::Core::CallBack2<dsAppClient, void, DrawSpace::Core::SceneNodeGraph::NodesEvent, DrawSpace::Core::BaseSceneNode*> NodesEventCallback;

    static dsAppClient*                                                 m_instance;

    
    DrawSpace::Core::SceneNodeGraph                                     m_scenenodegraph;


    DrawSpace::IntermediatePass*                                        m_texturepass;

    DrawSpace::IntermediatePass*                                        m_fillpass;

    DrawSpace::IntermediatePass*                                        m_zoompass;

    DrawSpace::FinalPass*                                               m_finalpass;

    DrawSpace::FinalPass*                                               m_debugfinalpass;


    DrawSpace::Utils::TimeManager                                       m_timer;

    bool                                                                m_mouselb;
    bool                                                                m_mouserb;

    DrawSpace::Gui::TextWidget*                                         m_fpstext_widget;

    DrawSpace::Interface::FontImport*                                   m_font_import;
    DrawSpace::Core::Font*                                              m_font;

    DrawSpace::Dynamics::CameraPoint*                                   m_camera;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera_node;

    DrawSpace::Dynamics::CameraPoint*                                   m_camera2;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera2_node;

    
    DrawSpace::Core::FreeMovement                                       m_freemove;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FreeMovement>*          m_freemove_node;


    DrawSpace::Chunk*                                                   m_cube;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_cube_node;

    DrawSpace::Dynamics::InertBody*                                     m_cube_body;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*         m_cube_body_node;



    DrawSpace::Chunk*                                                   m_cube2;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_cube2_node;

    DrawSpace::Dynamics::InertBody*                                     m_cube_body_2;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*         m_cube_body_2_node;

    
    DrawSpace::Interface::MesheImport*                                  m_meshe_import;

    
    DrawSpace::Spacebox*                                                m_spacebox;
    DrawSpace::Core::SceneNode<DrawSpace::Spacebox>*                    m_spacebox_node;
    
    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_spacebox_transfo_node;

    dsreal                                                              m_speed;
    dsreal                                                              m_speed_speed;

    DrawSpace::Dynamics::World                                          m_world;

    
    



    DrawSpace::Chunk*                                                   m_sun_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_sun_chunk_node;





    DrawSpace::Dynamics::Orbit*                                         m_mars_orbit;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbit>*             m_mars_orbit_node;
    DrawSpace::Dynamics::Orbiter*                                       m_mars_orbiter;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbiter>*           m_mars_orbiter_node;
    DrawSpace::Chunk*                                                   m_mars_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_mars_chunk_node;
    DrawSpace::Chunk*                                                   m_mars_orbit_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_mars_orbit_chunk_node;




    DrawSpace::Dynamics::Orbit*                                         m_moon_orbit;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbit>*             m_moon_orbit_node;
    DrawSpace::Dynamics::Orbiter*                                       m_moon_orbiter;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbiter>*           m_moon_orbiter_node;
    DrawSpace::Chunk*                                                   m_moon_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_moon_chunk_node;
    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_moon_transform_node;
    DrawSpace::Chunk*                                                   m_moon_orbit_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_moon_orbit_chunk_node;


    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_impostor_transfo_node;
    DrawSpace::Chunk*                                                   m_impostor;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_impostor_node;



    DrawSpace::Chunk*                                                   m_impostor2;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_impostor2_node;
    DrawSpace::Core::SceneNode<DrawSpace::Core::LongLatMovement>*       m_impostor2_ll_node;


    DrawSpace::Dynamics::Orbit*                                         m_saturn_root_orbit;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbit>*             m_saturn_root_orbit_node;

    DrawSpace::Chunk*                                                   m_saturn_root_orbit_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_saturn_root_orbit_chunk_node;



    DrawSpace::Dynamics::Orbit*                                         m_saturn1_orbit;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbit>*             m_saturn1_orbit_node;
    DrawSpace::Dynamics::Orbiter*                                       m_saturn1_orbiter;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbiter>*           m_saturn1_orbiter_node;
    DrawSpace::Chunk*                                                   m_saturn1_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_saturn1_chunk_node;
    DrawSpace::Chunk*                                                   m_saturn1_orbit_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_saturn1_orbit_chunk_node;



    DrawSpace::Dynamics::Orbit*                                         m_saturn2_orbit;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbit>*             m_saturn2_orbit_node;
    DrawSpace::Dynamics::Orbiter*                                       m_saturn2_orbiter;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Orbiter>*           m_saturn2_orbiter_node;
    DrawSpace::Chunk*                                                   m_saturn2_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_saturn2_chunk_node;
    DrawSpace::Chunk*                                                   m_saturn2_orbit_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_saturn2_orbit_chunk_node;


    DrawSpace::Dynamics::Calendar*                                      m_calendar;

    
    NodesEventCallback*                                                 m_nodesevent_cb;

    bool                                                                m_switch;


    DrawSpace::Chunk*               build_planet_chunk( char* p_name, char* p_texture );
    DrawSpace::Chunk*               build_orbit_drawable( char* p_name, DrawSpace::Dynamics::Orbit* p_orbit );


    void                            on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );

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

