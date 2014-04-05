
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

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

    DrawSpace::Gui::TextWidget*                 m_fpstext_widget;

    DrawSpace::Interface::FontImport*           m_font_import;
    DrawSpace::Core::Font*                      m_font;

    DrawSpace::Camera*                          m_camera;
    DrawSpace::Core::FPSMovement                m_fpsmove;
    DrawSpace::Camera*                          m_camera2;

    //DrawSpace::Interface::Drawable*             m_spacebox;
    DrawSpace::Spacebox*                        m_spacebox;

    DrawSpace::Interface::MesheImport*          m_meshe_import;
    //DrawSpace::Interface::Drawable*             m_ground;
    //DrawSpace::Interface::Drawable*             m_box;

    DrawSpace::Chunk*                           m_ground;

    int                                         m_box_count;
    int                                         m_box_texture;

    DrawSpace::Dynamics::World                  m_world;
    DrawSpace::Dynamics::InertBody*             m_ground_body;

    DrawSpace::Dynamics::Calendar*              m_calendar;

    typedef struct
    {
        DrawSpace::Drawable* drawable;
        DrawSpace::Dynamics::InertBody* inert_body;

    } Box;

    std::vector<Box>                            m_boxes;

    void create_box( void );

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

