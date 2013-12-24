
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class SceneTransform
{
public:
    dsreal                                  m_xangle, m_yangle;
    dsreal                                  m_zpos;
    DrawSpace::Utils::Mutex                 m_mutex_angles;
    DrawSpace::Utils::Mutex                 m_mutex_z;

    DrawSpace::Utils::Mutex                 m_mutex_result;
    DrawSpace::Utils::Matrix                m_result;

public:
    SceneTransform( void );
    ~SceneTransform( void );

    void Run( void );
    void Transform( void );
    void GetResult( DrawSpace::Utils::Matrix& p_out );

    void OnLeftDrag( DrawSpace::Utils::TimeManager& p_timer, long p_dx, long p_dy );
    void OnRightDrag( DrawSpace::Utils::TimeManager& p_timer, long p_dx, long p_dy );
};




class dsAppClient : public DrawSpace::App
{
protected:

    static dsAppClient*                         m_instance;


    DrawSpace::Scenegraph                       m_scenegraph;

    DrawSpace::FinalPass*                       m_finalpass;
    DrawSpace::IntermediatePass*                m_fogblendpass;

    DrawSpace::IntermediatePass*                m_fogintpass;
    DrawSpace::IntermediatePass*                m_texturepass;


    SceneTransform*                             m_transform;
    DrawSpace::Core::Task<SceneTransform>*      m_transformtask;
    DrawSpace::Utils::TimeManager               m_timer;

    bool                                        m_mouselb;
    bool                                        m_mouserb;

    DrawSpace::Camera*                          m_camera;
    DrawSpace::Core::FPSMovement                m_fpsmove;

    DrawSpace::Interface::MesheImport*          m_meshe_import;
    DrawSpace::Interface::Drawable*             m_chunk;

    DrawSpace::Interface::Drawable*             m_ground;



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

