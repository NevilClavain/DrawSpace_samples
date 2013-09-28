
#ifndef _DSAPPCLIENT_H_
#define _DSAPPCLIENT_H_

#include <dsapp.h>


class dsAppClient : public DrawSpace::App
{
protected:

    static dsAppClient*                     m_instance;

    DrawSpace::FinalPass*                   m_finalpass;
    DrawSpace::IntermediatePass*            m_texturepass;
    

    DrawSpace::Scenegraph                   m_scenegraph;
    DrawSpace::ChunkNode*                   m_chunknode;


    DrawSpace::Core::Font*                  m_font;

    dsreal                                  m_yangle;
    dsreal                                  m_xangle;
    dsreal                                  m_zangle;
    DrawSpace::Utils::TimeManager           m_timer;

    bool                                    m_mouselb;

    DrawSpace::Image*                       m_image1;

    long                                    m_xmouse;
    long                                    m_ymouse;

    DrawSpace::Gui::TextWidget*             m_text_widget;

    DrawSpace::Gui::TextWidget*             m_fpstext_widget;

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

