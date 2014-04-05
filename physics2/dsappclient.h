
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

    DrawSpace::Spacebox*                        m_spacebox;

    DrawSpace::Interface::MesheImport*          m_meshe_import;


    int                                         m_box_count;
    int                                         m_box_texture;



    // bullet stuff

    btDiscreteDynamicsWorld*                    m_myWorld;
    btBroadphaseInterface*                      m_myBroadphase;
    btCollisionDispatcher*                      m_myDispatcher;
    btDefaultCollisionConfiguration*            m_myCollisionConfiguration;
    btSequentialImpulseConstraintSolver*        m_mySequentialImpulseConstraintSolver;
    btTransform                                 m_myTransform;

    
    //btDefaultMotionState*                       m_myMotionState;
	
    btScalar                                    m_matrix[16];
    //btRigidBody*                                m_body;


    DrawSpace::Utils::Vector                    m_fwd;
    DrawSpace::Utils::Vector                    m_right;
    DrawSpace::Utils::Vector                    m_left;
    DrawSpace::Utils::Vector                    m_up;
    DrawSpace::Utils::Vector                    m_down;

    DrawSpace::Utils::Vector                    m_transformed_fwd;
    DrawSpace::Utils::Vector                    m_transformed_left;
    DrawSpace::Utils::Vector                    m_transformed_right;
    DrawSpace::Utils::Vector                    m_transformed_up;
    DrawSpace::Utils::Vector                    m_transformed_down;



    btTransform                                 m_myTransform_mem;
    btVector3                                   m_linearspeed_mem;
    btVector3                                   m_angularspeed_mem;




    btDiscreteDynamicsWorld*                    m_myWorld_2;
    btBroadphaseInterface*                      m_myBroadphase_2;
    btCollisionDispatcher*                      m_myDispatcher_2;
    btDefaultCollisionConfiguration*            m_myCollisionConfiguration_2;
    btSequentialImpulseConstraintSolver*        m_mySequentialImpulseConstraintSolver_2;


    bool                                        m_attached;



    typedef struct box
    {
        DrawSpace::Drawable*            drawable;
        btRigidBody*                    body;
        btDefaultMotionState*           motion;

        bool                            kinematic;

        dsreal                          k_angle;
        DrawSpace::Utils::Vector        k_pos;
        
        DrawSpace::Utils::Matrix        mat_b;

    } Box;

    std::vector<Box>                            m_boxes;

    void create_box( const dsstring& p_meshe, dsreal p_dims, const DrawSpace::Utils::Vector& p_pos, bool p_kinematic, bool p_world2 );


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

