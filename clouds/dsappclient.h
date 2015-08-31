
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>





class dsAppClient : public DrawSpace::App
{
protected:

    typedef DrawSpace::Core::CallBack2<dsAppClient, void, DrawSpace::Core::SceneNodeGraph::NodesEvent, DrawSpace::Core::BaseSceneNode*> NodesEventCallback;

    typedef DrawSpace::Core::CallBack<dsAppClient, void, DrawSpace::Core::PropertyPool*>                                                RunnerMsgCb;

    typedef DrawSpace::Core::CallBack2<dsAppClient, void, DrawSpace::Core::SceneNodeGraph::CameraEvent, DrawSpace::Core::BaseSceneNode*> CameraEventCb;


    typedef struct
    {
        DrawSpace::Chunk::ImpostorsDisplayList  idl;

        DrawSpace::Utils::Vector                pos;
        DrawSpace::Utils::Vector                transformed_pos;  // pour le z-sort

    } Cloud;


    static dsAppClient*                                                 m_instance;

    
    DrawSpace::Core::SceneNodeGraph                                     m_scenenodegraph;


    DrawSpace::IntermediatePass*                                        m_texturepass;
    DrawSpace::IntermediatePass*                                        m_texturepass2;

    DrawSpace::IntermediatePass*                                        m_maskpass;
    DrawSpace::IntermediatePass*                                        m_filterpass;



    DrawSpace::FinalPass*                                               m_finalpass;
    //DrawSpace::FinalPass*                                               m_finalpass2;


    DrawSpace::Utils::TimeManager                                       m_timer;

    bool                                                                m_mouselb;
    bool                                                                m_mouserb;


    DrawSpace::Dynamics::CameraPoint*                                   m_camera;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera_node;

    
    DrawSpace::Core::FreeMovement                                       m_freemove;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FreeMovement>*          m_freemove_node;


    DrawSpace::Dynamics::CameraPoint*                                   m_camera2;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera2_node;

    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_camera2_transfo_node;
    
    DrawSpace::Interface::MesheImport*                                  m_meshe_import;

    

    dsreal                                                              m_speed;
    dsreal                                                              m_speed_speed;

    DrawSpace::Dynamics::World                                          m_world;

    
    DrawSpace::Chunk*                                                   m_ground;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_ground_node;



    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>*        m_impostor2_transfo_node;

    DrawSpace::Chunk*                                                   m_impostor2;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_impostor2_node;


    DrawSpace::Dynamics::Calendar*                                      m_calendar;

    
    NodesEventCallback*                                                 m_nodesevent_cb;

    DrawSpace::Chunk::ImpostorsDisplayList                              m_idl;

    std::vector<Cloud>                                                  m_clouds;

    DrawSpace::Core::Mediator::MessageQueue*                            m_sort_msg;
    DrawSpace::Core::Runner*                                            m_runner;
    DrawSpace::Core::Task<DrawSpace::Core::Runner>*                     m_task;
    RunnerMsgCb*                                                        m_runner_msg_cb;

    bool                                                                m_update_clouds_meshes;
    DrawSpace::Utils::Mutex                                             m_mutex;


    CameraEventCb*                                                      m_cameraevent_cb;

    int                                                                 m_selected_camera;

    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_current_camera;

    bool                                                                m_previous_camera_pos_avail;
    DrawSpace::Utils::Vector                                            m_previous_camera_pos;


    int                                                                 m_recompute_count;

    bool                                                                m_ready;
   

    void on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );
    void on_sort_request( DrawSpace::Core::PropertyPool* p_args );

    void on_camera_event( DrawSpace::Core::SceneNodeGraph::CameraEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );



    void clouds_addcloud( dsreal p_xpos, dsreal p_zpos, DrawSpace::Chunk::ImpostorsDisplayList& p_idl );
    void clouds_execsortz( const DrawSpace::Utils::Matrix& p_globalmat );

    void clouds_impostors_init( void );

    static bool clouds_nodes_comp( Cloud p_n1, Cloud p_n2 );


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

