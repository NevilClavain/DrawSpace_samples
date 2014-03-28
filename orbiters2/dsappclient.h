
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class Planet
{
protected:

    dsstring                            m_name;
    DrawSpace::Dynamics::Orbiter*       m_orbiter;
    DrawSpace::Interface::Drawable*     m_drawable;

public:

    Planet( const dsstring& p_name, DrawSpace::Dynamics::World* p_world );
    ~Planet( void );

    DrawSpace::Interface::Drawable* GetDrawable( void );
    DrawSpace::Dynamics::Orbiter*   GetOrbiter( void );

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

    DrawSpace::Interface::Drawable*             m_spacebox;
    

    dsreal                                      m_speed;
    dsreal                                      m_speed_speed;

    DrawSpace::Dynamics::World                  m_world;

    

    DrawSpace::Dynamics::Calendar*              m_calendar;


    Planet*                                     m_planet;

    DrawSpace::Dynamics::Orbit*                 m_orbit;
    DrawSpace::Dynamics::Centroid*              m_centroid;

    DrawSpace::Dynamics::Rocket*                m_ship;
    DrawSpace::Interface::Drawable*             m_ship_drawable;

    

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

