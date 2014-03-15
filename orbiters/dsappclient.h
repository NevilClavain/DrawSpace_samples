
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>





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
    DrawSpace::Core::FreeMovement               m_freemove;
    
    DrawSpace::Interface::MesheImport*          m_meshe_import;

    DrawSpace::Interface::Drawable*             m_spacebox;
    

    dsreal                                      m_speed;
    dsreal                                      m_speed_speed;

    DrawSpace::Dynamics::World                  m_world;

    
    DrawSpace::Dynamics::InertBody*             m_cube_body;
    DrawSpace::Dynamics::InertBody*             m_cube_body_2;


    DrawSpace::Dynamics::Orbiter*               m_sun;

    DrawSpace::Dynamics::Orbiter*               m_mars;
    DrawSpace::Dynamics::Orbit*                 m_mars_orbit;
    DrawSpace::Dynamics::Centroid*              m_mars_centroid;

    DrawSpace::Dynamics::Orbiter*               m_moon;
    DrawSpace::Dynamics::Orbit*                 m_moon_orbit;
    DrawSpace::Dynamics::Centroid*              m_moon_centroid;


    DrawSpace::Dynamics::Orbiter*               m_saturn;
    DrawSpace::Dynamics::Orbit*                 m_saturn_orbit;
    DrawSpace::Dynamics::Centroid*              m_saturn_centroid;


    DrawSpace::Dynamics::Calendar*              m_calendar;


    DrawSpace::Dynamics::Orbiter*               build_planet( char* p_name, char* p_texture );
    DrawSpace::Interface::Drawable*             build_orbit_drawable( char* p_name, DrawSpace::Dynamics::Orbit* p_orbit );


    

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
