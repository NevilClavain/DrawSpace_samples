
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class dsAppClient : public DrawSpace::App
{
protected:

    static dsAppClient*                                                 m_instance;
    
    DrawSpace::Core::SceneNodeGraph                                     m_scenenodegraph;

    DrawSpace::FinalPass*                                               m_finalpass;
    DrawSpace::IntermediatePass*                                        m_fogblendpass;

    DrawSpace::IntermediatePass*                                        m_fogintpass;
    DrawSpace::IntermediatePass*                                        m_texturepass;


    DrawSpace::Utils::TimeManager                                       m_timer;

    bool                                                                m_mouselb;
    bool                                                                m_mouserb;

    

    DrawSpace::Dynamics::CameraPoint*                                   m_camera;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera_node;

    DrawSpace::Dynamics::CameraPoint*                                   m_camera2;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera2_node;

    DrawSpace::Dynamics::CameraPoint*                                   m_camera3;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>*       m_camera3_node;


    DrawSpace::Core::FPSMovement                                        m_fpsmove;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FPSMovement>*           m_fpsmove_node;


    DrawSpace::Core::CircularMovement                                   m_circmove;
    DrawSpace::Core::SceneNode<DrawSpace::Core::CircularMovement>*      m_circmove_node;


    DrawSpace::Interface::MesheImport*                                  m_meshe_import;

    DrawSpace::Chunk*                                                   m_chunk;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_chunk_node;

    DrawSpace::Chunk*                                                   m_ground;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_ground_node;


    DrawSpace::Interface::FontImport*                                   m_font_import;
    DrawSpace::Core::Font*                                              m_font;

    DrawSpace::Text*                                                    m_mytext;

    DrawSpace::Dynamics::World                                          m_world;
    
    DrawSpace::Dynamics::InertBody*                                     m_cube_body;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*         m_cube_body_node;


    DrawSpace::Dynamics::InertBody*                                     m_ground_body;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::InertBody>*         m_ground_body_node;


    ///////////////////////////////////////////////////////////////////////////////////////////////

    DrawSpace::Core::FreeMovement                                       m_freemove;
    DrawSpace::Core::SceneNode<DrawSpace::Core::FreeMovement>*          m_freemove_node;

    DrawSpace::Chunk*                                                   m_cube2;
    DrawSpace::Core::SceneNode<DrawSpace::Chunk>*                       m_cube2_node;

    DrawSpace::Dynamics::Collider*                                      m_cube2_collider;
    DrawSpace::Core::SceneNode<DrawSpace::Dynamics::Collider>*          m_cube2_colider_node;



    dsstring                                                            m_current_camera;


    bool                                                                m_draw_cube2;


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

