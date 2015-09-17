
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>





class dsAppClient : public DrawSpace::App
{
protected:

    typedef DrawSpace::Core::CallBack2<dsAppClient, void, DrawSpace::Core::SceneNodeGraph::NodesEvent, DrawSpace::Core::BaseSceneNode*> NodesEventCallback;

    typedef DrawSpace::Core::CallBack<dsAppClient, void, DrawSpace::Core::PropertyPool*>                                                RunnerMsgCb;

    typedef DrawSpace::Core::CallBack2<dsAppClient, void, DrawSpace::Core::SceneNodeGraph::CameraEvent, DrawSpace::Core::BaseSceneNode*> CameraEventCb;


    typedef DrawSpace::Core::CallBack<dsAppClient, void, DrawSpace::Procedural::Atomic*>                                                 ProceduralCb;

    typedef struct
    {
        DrawSpace::ImpostorsDisplayList         idl;

        DrawSpace::Utils::Vector                pos;
        dsreal                                  dist_to_cam;  // pour le z-sort

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



    DrawSpace::Dynamics::Calendar*                                      m_calendar;

    
    NodesEventCallback*                                                 m_nodesevent_cb;


    CameraEventCb*                                                      m_cameraevent_cb;

    int                                                                 m_selected_camera;

    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_current_camera;

    

    ProceduralCb*                                                       m_procedural_cb;

    
   

    void on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );

    void on_camera_event( DrawSpace::Core::SceneNodeGraph::CameraEvent p_event, DrawSpace::Core::BaseSceneNode* p_node );

    void on_procedural( DrawSpace::Procedural::Atomic* p_atom );



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

