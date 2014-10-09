
#include "dsappclient.h"



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;


_DECLARE_DS_LOGGER( logger, "AppClient" )


#define SHIP_MASS 50.0


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

dsAppClient::dsAppClient( void ) : 
m_mouselb( false ), 
m_mouserb( false ), 
m_speed( 0.0 ), 
m_speed_speed( 5.0 ),
m_curr_camera( NULL )
{    
    _INIT_LOGGER( "orbiters2.conf" )  
    m_w_title = "orbiters 2 test";

    m_player_view_linear_acc[0] = m_player_view_linear_acc[1] = m_player_view_linear_acc[2] = 0;
    m_player_view_linear_acc_2[0] = m_player_view_linear_acc_2[1] = m_player_view_linear_acc_2[2] = 0;
    m_player_view_linear_speed[0] = m_player_view_linear_speed[1] = m_player_view_linear_speed[2] = 0;

    m_player_view_angular_acc[0] = m_player_view_angular_acc[1] = m_player_view_angular_acc[2] = 0;
    m_player_view_angular_acc_2[0] = m_player_view_angular_acc_2[1] = m_player_view_angular_acc_2[2] = 0;
    m_player_view_angular_speed[0] = m_player_view_angular_speed[1] = m_player_view_angular_speed[2] = 0;


    m_player_view_angular_speed_clamp_up[0] = 0.0;
    m_player_view_angular_speed_clamp_down[0] = 0.0;

    m_player_view_angular_speed_clamp_up[1] = 0.0;
    m_player_view_angular_speed_clamp_down[1] = 0.0;

    m_player_view_angular_speed_clamp_up[2] = 0.0;
    m_player_view_angular_speed_clamp_down[2] = 0.0;


    m_player_view_linear_speed_clamp_up[0] = 0.0;
    m_player_view_linear_speed_clamp_down[0] = 0.0;

    m_player_view_linear_speed_clamp_up[1] = 0.0;
    m_player_view_linear_speed_clamp_down[1] = 0.0;

    m_player_view_linear_speed_clamp_up[2] = 0.0;
    m_player_view_linear_speed_clamp_down[2] = 0.0;



    m_player_view_theta = m_player_view_phi = m_player_view_rho = 0.0;


    m_mouseleftbuttondown_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mouseleftbuttondown ) );
}

dsAppClient::~dsAppClient( void )
{

}


#define SPEED     1.5

void dsAppClient::compute_player_view_transform( void )
{
    Matrix cam_base_pos;
    cam_base_pos.Translation( 0.0, 2.8, 11.4 );


    /////////////////////////////////////////////////////

    if( m_player_view_linear_acc[2] > 0.0 )
    {
        m_player_view_linear_speed_clamp_up[2] = 10.0;
        m_player_view_linear_speed_clamp_down[2] = 0.0;

        dsreal limit = ( m_player_view_linear_acc[2] / ( 8000.0 / SHIP_MASS ) );

        if( m_player_view_pos[2] < limit )
        {
            m_player_view_linear_acc_2[2] = 5.0;
        }
        else
        {
            m_player_view_linear_acc_2[2] = -5.0;
        }
    }
    else if( m_player_view_linear_acc[2] < 0.0 )
    {
        m_player_view_linear_speed_clamp_up[2] = 0.0;
        m_player_view_linear_speed_clamp_down[2] = -10.0;

        dsreal limit = ( m_player_view_linear_acc[2] / ( 8000.0 / SHIP_MASS ) );

        if( m_player_view_pos[2] > limit )
        {
            m_player_view_linear_acc_2[2] = -5.0;
        }
        else
        {
            m_player_view_linear_acc_2[2] = 5.0;
        }
    }
    else
    {
        if( m_player_view_pos[2] > 0.01 )
        {
            m_player_view_linear_speed_clamp_up[2] = 0.0;
            m_player_view_linear_speed_clamp_down[2] = -10.0;

            m_player_view_linear_acc_2[2] = -5.0;
        }

        else if( m_player_view_pos[2] < -0.01 )
        {
            m_player_view_linear_speed_clamp_up[2] = 10.0;
            m_player_view_linear_speed_clamp_down[2] = 0.0;

            m_player_view_linear_acc_2[2] = 5.0;
        }
        else
        {
            if( m_player_view_pos[2] > 0.0 )
            {
                m_player_view_linear_acc_2[2] = 5.0;

                m_player_view_linear_speed_clamp_up[2] = 0.0;
                m_player_view_linear_speed_clamp_down[2] = -10.0;
            }
            else if( m_player_view_pos[2] < 0.0 )
            {
                m_player_view_angular_acc_2[2] = -5.0;

                m_player_view_linear_speed_clamp_up[2] = 10.0;
                m_player_view_linear_speed_clamp_down[2] = 0.0;
            }
        }
    }


    /////////////////////////////////////////////////////


    dsreal zpos = m_player_view_pos[2];
    m_timer.TranslationSpeedInc( &zpos, m_player_view_linear_speed[2] );
    m_player_view_pos[2] = zpos;


    dsreal curr_speed = m_player_view_linear_speed[2];
    m_timer.TranslationSpeedInc( &curr_speed, m_player_view_linear_acc_2[2] );
    m_player_view_linear_speed[2] = curr_speed;   
    m_player_view_linear_speed[2] = DrawSpace::Utils::Maths::Clamp( m_player_view_linear_speed_clamp_down[2], m_player_view_linear_speed_clamp_up[2], m_player_view_linear_speed[2] );



    Matrix cam_delta_pos;
    cam_delta_pos.Translation( m_player_view_pos );

    /////////////////////////////////////////////////////

    if( m_player_view_angular_acc[0] > 0.0 )
    {
        m_player_view_angular_speed_clamp_up[0] = 10.0;
        m_player_view_angular_speed_clamp_down[0] = 0.0;

        if( m_player_view_phi < 4 * m_player_view_angular_acc[0] )
        {
            m_player_view_angular_acc_2[0] = 5.0;
        }
        else
        {
            m_player_view_angular_acc_2[0] = -5.0;
        }
    }
    else if( m_player_view_angular_acc[0] < 0.0 )
    {
        m_player_view_angular_speed_clamp_up[0] = 0.0;
        m_player_view_angular_speed_clamp_down[0] = -10.0;

        if( m_player_view_phi > 4 * m_player_view_angular_acc[0] )
        {
            m_player_view_angular_acc_2[0] = -5.0;
        }
        else
        {
            m_player_view_angular_acc_2[0] = 5.0;
        }
    }
    else
    {
        if( m_player_view_phi > 0.3 )
        {
            m_player_view_angular_speed_clamp_up[0] = 0.0;
            m_player_view_angular_speed_clamp_down[0] = -10.0;

            m_player_view_angular_acc_2[0] = -5.0;
        }

        else if( m_player_view_phi < -0.3 )
        {
            m_player_view_angular_speed_clamp_up[0] = 10.0;
            m_player_view_angular_speed_clamp_down[0] = 0.0;

            m_player_view_angular_acc_2[1] = 5.0;
        }

        else
        {
            if( m_player_view_phi > 0.0 )
            {
                m_player_view_angular_acc_2[0] = 5.0;

                m_player_view_angular_speed_clamp_up[0] = 0.0;
                m_player_view_angular_speed_clamp_down[0] = -10.0;
            }
            else if( m_player_view_phi < 0.0 )
            {
                m_player_view_angular_acc_2[0] = -5.0;

                m_player_view_angular_speed_clamp_up[0] = 10.0;
                m_player_view_angular_speed_clamp_down[0] = 0.0;
            }
        }
    }






    if( m_player_view_angular_acc[1] > 0.0 )
    {
        m_player_view_angular_speed_clamp_up[1] = 10.0;
        m_player_view_angular_speed_clamp_down[1] = 0.0;

        if( m_player_view_theta < 4 * m_player_view_angular_acc[1] )
        {
            m_player_view_angular_acc_2[1] = 5.0;
        }
        else
        {
            m_player_view_angular_acc_2[1] = -5.0;
        }
    }
    else if( m_player_view_angular_acc[1] < 0.0 )
    {
        m_player_view_angular_speed_clamp_up[1] = 0.0;
        m_player_view_angular_speed_clamp_down[1] = -10.0;

        if( m_player_view_theta > 4 * m_player_view_angular_acc[1] )
        {
            m_player_view_angular_acc_2[1] = -5.0;
        }
        else
        {
            m_player_view_angular_acc_2[1] = 5.0;
        }
    }
    else
    {
        if( m_player_view_theta > 0.3 )
        {
            m_player_view_angular_speed_clamp_up[1] = 0.0;
            m_player_view_angular_speed_clamp_down[1] = -10.0;

            m_player_view_angular_acc_2[1] = -5.0;
        }

        else if( m_player_view_theta < -0.3 )
        {
            m_player_view_angular_speed_clamp_up[1] = 10.0;
            m_player_view_angular_speed_clamp_down[1] = 0.0;

            m_player_view_angular_acc_2[1] = 5.0;
        }

        else
        {
            if( m_player_view_theta > 0.0 )
            {
                m_player_view_angular_acc_2[1] = 5.0;

                m_player_view_angular_speed_clamp_up[1] = 0.0;
                m_player_view_angular_speed_clamp_down[1] = -10.0;
            }
            else if( m_player_view_theta < 0.0 )
            {
                m_player_view_angular_acc_2[1] = -5.0;

                m_player_view_angular_speed_clamp_up[1] = 10.0;
                m_player_view_angular_speed_clamp_down[1] = 0.0;
            }            
        }
    }





    if( m_player_view_angular_acc[2] > 0.0 )
    {
        m_player_view_angular_speed_clamp_up[2] = 10.0;
        m_player_view_angular_speed_clamp_down[2] = 0.0;

        if( m_player_view_rho < 3 * m_player_view_angular_acc[2] )
        {
            m_player_view_angular_acc_2[2] = 5.0;
        }
        else
        {
            m_player_view_angular_acc_2[2] = -5.0;
        }
    }
    else if( m_player_view_angular_acc[2] < 0.0 )
    {
        m_player_view_angular_speed_clamp_up[2] = 0.0;
        m_player_view_angular_speed_clamp_down[2] = -10.0;

        if( m_player_view_rho > 3 * m_player_view_angular_acc[2] )
        {          
            m_player_view_angular_acc_2[2] = -5.0;
        }
        else
        {
            m_player_view_angular_acc_2[2] = 5.0;
        }
    }
    else
    {
        if( m_player_view_rho > 0.3 )
        {
            m_player_view_angular_speed_clamp_up[2] = 0.0;
            m_player_view_angular_speed_clamp_down[2] = -10.0;

            m_player_view_angular_acc_2[2] = -5.0;
        }

        else if( m_player_view_rho < -0.3 )
        {
            m_player_view_angular_speed_clamp_up[2] = 10.0;
            m_player_view_angular_speed_clamp_down[2] = 0.0;

            m_player_view_angular_acc_2[2] = 5.0;
        }

        else
        {
            if( m_player_view_rho > 0.0 )
            {
                m_player_view_angular_acc_2[2] = 5.0;

                m_player_view_angular_speed_clamp_up[2] = 0.0;
                m_player_view_angular_speed_clamp_down[2] = -10.0;
            }
            else if( m_player_view_rho < 0.0 )
            {
                m_player_view_angular_acc_2[2] = -5.0;

                m_player_view_angular_speed_clamp_up[2] = 10.0;
                m_player_view_angular_speed_clamp_down[2] = 0.0;
            }
        }
    }





    dsreal phi = m_player_view_phi;
    m_timer.TranslationSpeedInc( &phi, m_player_view_angular_speed[0] );
    m_player_view_phi = phi;


    dsreal rho = m_player_view_rho;
    m_timer.TranslationSpeedInc( &rho, m_player_view_angular_speed[2] );
    m_player_view_rho = rho;


    dsreal theta = m_player_view_theta;
    m_timer.TranslationSpeedInc( &theta, m_player_view_angular_speed[1] );
    m_player_view_theta = theta;




    curr_speed = m_player_view_angular_speed[0];
    m_timer.TranslationSpeedInc( &curr_speed, m_player_view_angular_acc_2[0] );
    m_player_view_angular_speed[0] = curr_speed;

    
    m_player_view_angular_speed[0] = DrawSpace::Utils::Maths::Clamp( m_player_view_angular_speed_clamp_down[0], m_player_view_angular_speed_clamp_up[0], m_player_view_angular_speed[0] );




    curr_speed = m_player_view_angular_speed[1];
    m_timer.TranslationSpeedInc( &curr_speed, m_player_view_angular_acc_2[1] );
    m_player_view_angular_speed[1] = curr_speed;

    
    m_player_view_angular_speed[1] = DrawSpace::Utils::Maths::Clamp( m_player_view_angular_speed_clamp_down[1], m_player_view_angular_speed_clamp_up[1], m_player_view_angular_speed[1] );



    
    curr_speed = m_player_view_angular_speed[2];
    m_timer.TranslationSpeedInc( &curr_speed, m_player_view_angular_acc_2[2] );
    m_player_view_angular_speed[2] = curr_speed;

    
    m_player_view_angular_speed[2] = DrawSpace::Utils::Maths::Clamp( m_player_view_angular_speed_clamp_down[2], m_player_view_angular_speed_clamp_up[2], m_player_view_angular_speed[2] );




    /////////////////////////////////////////////////////

    Matrix rotx;
    rotx.Rotation( Vector( 1.0, 0.0, 0.0, 1.0 ), DrawSpace::Utils::Maths::DegToRad( m_player_view_phi ) );

    Matrix roty;
    roty.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), DrawSpace::Utils::Maths::DegToRad( m_player_view_theta ) );

    Matrix rotz;
    rotz.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), DrawSpace::Utils::Maths::DegToRad( m_player_view_rho ) );


    Transformation tf;

    tf.ClearAll();
    tf.PushMatrix( cam_delta_pos );
    tf.PushMatrix( cam_base_pos );
    tf.PushMatrix( rotx );
    tf.PushMatrix( roty );
    tf.PushMatrix( rotz );
    tf.BuildResult();
    tf.GetResult( &m_player_view_transform );

}

void dsAppClient::OnRenderFrame( void )
{
    

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    
    DrawSpace::Utils::Matrix sbtrans;
    sbtrans.Scale( 20.0, 20.0, 20.0 );
    m_scenegraph.SetNodeLocalTransformation( "spacebox", sbtrans );


    


    //compute_player_view_transform();
    //m_camera2->SetLocalTransform( m_player_view_transform );

    /*
    Matrix id;
    id.Identity();
    m_camera5->SetLocalTransform( id );
    */
    
    Matrix origin;
    origin.Identity();

    m_orbit->OrbitStep( origin );


    m_planet->ApplyGravity();
    m_moon->ApplyGravity();

    m_ship->Update();
    m_cube_body->Update();

    Matrix bld_pos;
    //bld_pos.Translation( 265000000.0, 0.0, -200.0 );
    bld_pos.Identity();


    Matrix cam7_pos;
    cam7_pos.Translation( 0.0, 0.0, 4.0 );
    m_camera7->SetLocalTransform( cam7_pos );


    Matrix cam8_pos;
    cam8_pos.Translation( 0.0, 130.0, 0.0 );
    m_camera8->SetLocalTransform( cam8_pos );


    m_building_collider->Update( m_timer, bld_pos );


    m_socle_collider->Update( m_timer, bld_pos );

           
    m_scenegraph.ComputeTransformations( m_timer );

    //////////////////////////////////////////////////////////////

    
    m_text_widget->SetVirtualTranslation( 100, 75 );
    m_text_widget_2->SetVirtualTranslation( -60, 160 );

    char distance[64];
    sprintf( distance, "%.3f km", m_reticle_widget->GetLastDistance() / 1000.0 );

    m_text_widget_2->SetText( -40, 0, 30, dsstring( distance ), DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
    m_reticle_widget->Transform();
    m_reticle_widget->Draw();
    

    //////////////////////////////////////////////////////////////



    m_planet->Update();
    m_moon->Update();
    

    m_planet->ManageBodies();
    m_moon->ManageBodies();


    


    m_texturepass->GetRenderingQueue()->Draw();

    m_text_widget->Draw();

    m_finalpass->GetRenderingQueue()->Draw();


    long current_fps = m_timer.GetFPS();
    renderer->DrawText( 0, 255, 0, 10, 35, "%d", current_fps );

    dsstring date;
    m_calendar->GetFormatedDate( date );    
    renderer->DrawText( 0, 255, 0, 10, 55, "%s", date.c_str() );

    


    dsreal speed = m_ship->GetLinearSpeedMagnitude();

    renderer->DrawText( 0, 255, 0, 10, 95, "speed = %.1f km/h ( %.1f m/s) - aspeed = %.1f", speed * 3.6, speed, m_ship->GetAngularSpeedMagnitude() );

    renderer->DrawText( 0, 255, 0, 10, 115, "contact = %d", m_ship->GetContactState() );


    int x_reticle, y_reticle;

    m_reticle_widget->GetScreenPos( x_reticle, y_reticle );

    //renderer->DrawText( 0, 255, 0, x_reticle - 40, y_reticle - 40, "%.3f km", m_reticle_widget->GetLastDistance() / 1000.0 );


    
    // current camera infos

    dsstring camera_name;

    if( m_curr_camera )
    {
        m_curr_camera->GetName( camera_name );
        renderer->DrawText( 0, 255, 0, 10, 150, "Camera : %s", camera_name.c_str() );

        CameraPoint::Infos cam_infos;

        m_curr_camera->GetInfos( cam_infos );

        if( cam_infos.attached_to_body )
        {
            renderer->DrawText( 0, 255, 0, 10, 170, "Attached to : %s", cam_infos.attached_body_classname.c_str() );
        }

        if( cam_infos.locked_on_body )
        {
            renderer->DrawText( 0, 255, 0, 10, 190, "Locked on body" );
        }
        else if( cam_infos.locked_on_transformnode )
        {
            renderer->DrawText( 0, 255, 0, 10, 190, "Locked on transform node" );
        }
        else
        {
            renderer->DrawText( 0, 255, 0, 10, 190, "No lock target" );
        }

        if( cam_infos.has_movement )
        {
            renderer->DrawText( 0, 255, 0, 10, 210, "Associated movement : %s", cam_infos.movement_classname.c_str() );
        }
        else
        {
            renderer->DrawText( 0, 255, 0, 10, 210, "No associated movement" );
        }

        if( cam_infos.has_longlatmovement )
        {
            renderer->DrawText( 0, 255, 0, 10, 230, "On longlat movement" );
        }

        if( cam_infos.relative_orbiter )
        {
            renderer->DrawText( 0, 255, 0, 10, 250, "Relative to an orbiter" );
            renderer->DrawText( 0, 255, 0, 10, 270, "Altitude = %.2f m", cam_infos.altitud );
        }
    }

    renderer->DrawText( 0, 255, 0, 10, 300, "reticle distance = %f", m_reticle_widget->GetLastDistance() );

    if( m_curr_camera == m_camera5 || m_curr_camera == m_camera4 )
    {
        renderer->DrawText( 0, 255, 0, 10, 320, "locked object distance = %f", m_curr_camera->GetLockedObjectDistance() );
    }

    long c1, c2, c3;

    DrawSpace::Planetoid::Fragment* ship_planet_fragment = m_planet->GetRegisteredBodyPlanetFragment( m_ship );

    ship_planet_fragment->GetCollisionMesheBuildStats( c1, c2, c3 );

    if( c1 == c2 && c2 == c3 )
    {
        renderer->DrawText( 0, 255, 0, 10, 350, "c1 = %d c2 = %d c3 = %d", c1, c2, c3 );
    }
    else
    {
        renderer->DrawText( 255, 0, 0, 10, 350, "c1 = %d c2 = %d c3 = %d", c1, c2, c3 );
    }


    //renderer->DrawText( 0, 255, 0, 10, 195, "planet01 : %d %d", m_planet->GetCollisionState(), m_planet->GetCollisionMesheBuildCount() );
    //renderer->DrawText( 0, 255, 0, 10, 215, "moon : %d %d", m_moon->GetCollisionState(), m_moon->GetCollisionMesheBuildCount() );

    /*
    Vector tf, tt;
    Vector tf2, tt2;

    Matrix ship_trans;

    m_ship->GetLastLocalWorldTrans( ship_trans );
    ship_trans.ClearTranslation();
    ship_trans.Inverse();

    m_ship->GetTotalForce( tf );
    m_ship->GetTotalTorque( tt );

         
    ship_trans.Transform( &tf, &tf2 );
    ship_trans.Transform( &tt, &tt2 );
        
    m_player_view_linear_acc[2] = -tf2[2] / SHIP_MASS;

    m_player_view_angular_acc[2] = -tt2[2];
    m_player_view_angular_acc[1] = -tt2[1];
    m_player_view_angular_acc[0] = -tt2[0];
    */



    renderer->FlipScreen();

    m_calendar->Run();

    

    m_freemove.SetSpeed( m_speed );
}

bool dsAppClient::OnIdleAppInit( void )
{
    //World::m_scale = 0.5;
    World::m_scale = 1.0;
    DrawSpace::SphericalLOD::Body::BuildMeshe();


    RegisterMouseInputEventsProvider( &m_mouse_input );


    bool status;

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    if( !status )
    {
        _DSEXCEPTION( "Cannot load ac3dmeshe plugin !" )
    }
    _DSDEBUG( logger, "ac3dmeshe plugin successfully loaded..." );

    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );

    /////////////////////////////////////


    status = DrawSpace::Utils::LoadFontImportPlugin( "cbfgfont.dll", "cbfgfont_plugin" );
    m_font_import = DrawSpace::Utils::InstanciateFontImportFromPlugin( "cbfgfont_plugin" );
    m_font = _DRAWSPACE_NEW_( Font, Font );
    m_font->SetImporter( m_font_import );

    status = m_font->Build( "mangalfont.bmp", "mangalfont.csv" );
    if( !status )
    {
        return false;
    }


    /////////////////////////////////////    

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( false );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0 );

    m_scenegraph.RegisterPass( m_texturepass );


    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->CreateViewportQuad();

    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    

    //////////////////////////////////////////////////////////////

    m_spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
    m_spacebox->RegisterPassSlot( "texture_pass" );
    
    m_spacebox->SetName( "spacebox" );


    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass" );

    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_front5.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_back6.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_top3.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_bottom4.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_left2.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_right1.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::FrontQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::RearQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::TopQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::BottomQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::LeftQuad )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", Spacebox::RightQuad )->SetOrderNumber( 200 );


    m_scenegraph.RegisterNode( m_spacebox );
    

    //////////////////////////////////////////////////////////////


    m_world.Initialize();
    
    
    //////////////////////////////////////////////////////////////



    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_chunk->RegisterPassSlot( "texture_pass" );
    m_chunk->SetRenderer( renderer );

    m_chunk->SetName( "box" );
    
    m_chunk->GetMeshe()->SetImporter( m_meshe_import );

    m_chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );    

    m_chunk->GetNodeFromPass( "texture_pass" )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_chunk->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_chunk->GetNodeFromPass( "texture_pass" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );



    m_chunk->GetNodeFromPass( "texture_pass" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( m_chunk );


    DrawSpace::Dynamics::Body::Parameters cube_params;
    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 ); //DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );


    //cube_params.initial_attitude.Translation( 265000000.0, 0.0, -20.0 );

    //cube_params.initial_attitude = llres;


    m_cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, m_chunk, cube_params ) );

    ///////////////////////////////////////////////////////////////


    m_building = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_building->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_building->RegisterPassSlot( "texture_pass" );
    m_building->SetRenderer( renderer );

    m_building->SetName( "building" );
    
    m_building->GetMeshe()->SetImporter( m_meshe_import );

    m_building->GetMeshe()->LoadFromFile( "building3.ac", 0 );  



    m_building->GetNodeFromPass( "texture_pass" )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_building->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_building->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_building->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_building->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_building->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_building->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_building->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_building->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_building->GetNodeFromPass( "texture_pass" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "building3.bmp" ) ), 0 );






    m_building->GetNodeFromPass( "texture_pass" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( m_building );

  
    
    DrawSpace::Dynamics::Body::Parameters bld_params;
    bld_params.mass = 0.0;
    bld_params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
    bld_params.shape_descr.meshe = *( m_building->GetMeshe() );


   
    m_building_collider = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Collider, DrawSpace::Dynamics::Collider( m_building ) );

    m_building_collider->SetKinematic( bld_params );
    

    ///////////////////////////////////////////////////////////////

    m_socle = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_socle->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_socle->RegisterPassSlot( "texture_pass" );
    m_socle->SetRenderer( renderer );

    m_socle->SetName( "socle" );
    
    m_socle->GetMeshe()->SetImporter( m_meshe_import );

    m_socle->GetMeshe()->LoadFromFile( "socle.ac", 0 );  



    m_socle->GetNodeFromPass( "texture_pass" )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_socle->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_socle->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_socle->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_socle->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_socle->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_socle->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_socle->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_socle->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_socle->GetNodeFromPass( "texture_pass" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "socle.bmp" ) ), 0 );






    m_socle->GetNodeFromPass( "texture_pass" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( m_socle );

  
    
    DrawSpace::Dynamics::Body::Parameters socle_params;
    socle_params.mass = 0.0;
    socle_params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
    socle_params.shape_descr.meshe = *( m_socle->GetMeshe() );


   
    m_socle_collider = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Collider, DrawSpace::Dynamics::Collider( m_socle ) );

    m_socle_collider->SetKinematic( socle_params );


    ///////////////////////////////////////////////////////////////


    //m_planet = _DRAWSPACE_NEW_( MyPlanet, MyPlanet( "planet01", 400.0 ) );

    m_planet = _DRAWSPACE_NEW_( DrawSpace::Planetoid::Body, DrawSpace::Planetoid::Body( "planet01", 400.0 ) );


    m_planet->GetDrawable()->RegisterPassSlot( "texture_pass" );

    for( long i = 0; i < 6; i++ )
    {
        m_planet->GetDrawable()->GetNodeFromPass( "texture_pass", i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
        m_planet->GetDrawable()->SetNodeFromPassSpecificFx( "texture_pass", i, "main_fx" );

        /*
        m_planet->GetDrawable()->GetNodeFromPass( "texture_pass", i )->SetVertexTexture( _DRAWSPACE_NEW_( Texture, Texture( "map.bmp" ) ), 0 );
        m_planet->GetDrawable()->GetNodeFromPass( "texture_pass", i )->GetVertexTexture( 0 )->LoadFromFile();


        m_planet->GetDrawable()->GetNodeFromPass( "texture_pass", i )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "mapcolor.bmp" ) ), 0 );
        m_planet->GetDrawable()->GetNodeFromPass( "texture_pass", i )->GetTexture( 0 )->LoadFromFile();
        */

    }
    m_scenegraph.RegisterNode( m_planet->GetDrawable() );


    

    m_centroid = _DRAWSPACE_NEW_( Centroid, Centroid );
    m_centroid->SetOrbiter( m_planet->GetOrbiter() );

    m_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 270000000.0, 0.99, 0.0, 0.0, 0.0, 0.0, 0.333, 25.0, 1.0, m_centroid ) );


    //////////////////////////////////////////////////////////////

    //m_moon = _DRAWSPACE_NEW_( MyPlanet, MyPlanet( "moon", 300.0 ) );

    m_moon = _DRAWSPACE_NEW_( DrawSpace::Planetoid::Body, DrawSpace::Planetoid::Body( "moon", 300.0 ) );


    m_moon->GetDrawable()->RegisterPassSlot( "texture_pass" );

    for( long i = 0; i < 6; i++ )
    {
        m_moon->GetDrawable()->GetNodeFromPass( "texture_pass", i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
        m_moon->GetDrawable()->SetNodeFromPassSpecificFx( "texture_pass", i, "main_fx" );

        /*
        m_moon->GetDrawable()->GetNodeFromPass( "texture_pass", i )->SetVertexTexture( _DRAWSPACE_NEW_( Texture, Texture( "map.bmp" ) ), 0 );
        m_moon->GetDrawable()->GetNodeFromPass( "texture_pass", i )->GetVertexTexture( 0 )->LoadFromFile();

        m_moon->GetDrawable()->GetNodeFromPass( "texture_pass", i )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "mapcolor.bmp" ) ), 0 );
        m_moon->GetDrawable()->GetNodeFromPass( "texture_pass", i )->GetTexture( 0 )->LoadFromFile();
        */

    }
    m_scenegraph.RegisterNode( m_moon->GetDrawable() );

    

    m_centroid2 = _DRAWSPACE_NEW_( Centroid, Centroid );
    m_centroid2->SetOrbiter( m_moon->GetOrbiter() );

    m_orbit2 = _DRAWSPACE_NEW_( Orbit, Orbit( 20000000.0, 0.99, 0.0, 0.0, 0.0, 0.0, 0.002, 0.0, 1.0, m_centroid2 ) );

    m_centroid->RegisterSubOrbit( m_orbit2 );



    //////////////////////////////////////////////////////////////


    m_ship_drawable = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ship_drawable->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_ship_drawable->RegisterPassSlot( "texture_pass" );
    m_ship_drawable->SetRenderer( renderer );

    m_ship_drawable->SetName( "rocket" );
    
    m_ship_drawable->GetMeshe()->SetImporter( m_meshe_import );

    m_ship_drawable->GetMeshe()->LoadFromFile( "survey.ac", 0 );    

    m_ship_drawable->GetNodeFromPass( "texture_pass" )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_ship_drawable->GetNodeFromPass( "texture_pass" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "surveyship_color.jpg" ) ), 0 );



    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( m_ship_drawable );


    //DrawSpace::Dynamics::Body::Parameters cube_params;
    cube_params.mass = SHIP_MASS;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 74.1285 / 2.0, 21.4704 / 2.0, 81.911 / 2.0, 1.0 );
    
    cube_params.initial_attitude.Translation( 265000000.0, 0.0, 0.0 );

    m_ship = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Rocket, DrawSpace::Dynamics::Rocket( &m_world, m_ship_drawable, cube_params ) );


    //////////////////////////////////////////////////////////////


    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera" ) );
    //m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera->SetName( "camera" );
    m_scenegraph.RegisterNode( m_camera );
    
    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera2", m_ship ) );
    //m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera2->SetName( "camera2" );
    //m_camera2->SetRefBody( m_ship );
    m_scenegraph.RegisterNode( m_camera2 );

    m_head_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::HeadMovement, DrawSpace::Core::HeadMovement );
    //m_head_mvt->Init( m_ship, 1.0, 8000.0, Vector( 0.0, 2.8, 11.4, 1.0 ) );

    m_head_mvt->Init( 1.0, 8000.0, Vector( 0.0, 25.0, 110.0, 1.0 ) );
    m_head_mvt->SetRefBody( m_ship );


    m_camera2->RegisterMovement( m_head_mvt );



    m_camera3 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera3", m_ship ) );
    //m_camera3 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera3->SetName( "camera3" );
    //m_camera3->SetRefBody( m_ship );

    m_scenegraph.RegisterNode( m_camera3 );


    m_camera4 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera4", m_ship ) );
    //m_camera4 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera4->SetName( "camera4" );
    //m_camera4->SetRefBody( m_ship );
    m_scenegraph.RegisterNode( m_camera4 );


    m_circular_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::CircularMovement, DrawSpace::Core::CircularMovement );
    m_circular_mvt->Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( 385.0, 90.0, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 0.0, 0.0, 0.0 );

    m_camera3->RegisterMovement( m_circular_mvt );

    m_camera3->LockOnBody( m_ship );

    m_camera4->RegisterMovement( m_circular_mvt );
    m_camera4->LockOnBody( /*m_planet->GetOrbiter()*/ m_cube_body );


    m_camera5 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera5", m_planet->GetOrbiter() ) );
    //m_camera5 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera5->SetName( "camera5" );
    //m_camera5->SetRefBody( m_planet->GetOrbiter() );

    m_scenegraph.RegisterNode( m_camera5 );

    m_longlat_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt->Init( -21.0, 20.0, 400000.5, 0.0, 0.0 );
    m_camera5->RegisterLongLatMovement( m_longlat_mvt );

    m_circular_mvt2 = _DRAWSPACE_NEW_( DrawSpace::Core::CircularMovement, DrawSpace::Core::CircularMovement );
    m_circular_mvt2->Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( 25.0, 0.9, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 0.0, 0.0, 0.0 );
    m_camera5->RegisterMovement( m_circular_mvt2 );
    m_circular_mvt2->SetAngularSpeed( 25.0 );
    


    m_camera6 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera6", m_planet->GetOrbiter() ) );
    //m_camera6 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera6->SetName( "camera6" );
    //m_camera6->SetRefBody( m_planet->GetOrbiter() );

    m_scenegraph.RegisterNode( m_camera6 );

    m_spectator_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::SpectatorMovement, DrawSpace::Core::SpectatorMovement );

    m_spectator_mvt->SetName( "spectator_camera" );
    //m_spectator_mvt->Init( m_ship, 16.0, 8000, true );
    m_spectator_mvt->Init( 16.0, 8000, true );
    m_spectator_mvt->SetRefBody( m_ship );

    m_camera6->RegisterMovement( m_spectator_mvt );
    m_camera6->LockOnBody( m_ship );

    m_camera6->LockOnBody( m_ship );




    m_camera7 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera7", m_cube_body ) );
    //m_camera7 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera7->SetName( "camera7" );
    //m_camera7->SetRefBody( m_cube_body );
    m_scenegraph.RegisterNode( m_camera7 );



    m_camera8 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera8", m_building_collider ) );
    //m_camera8 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera8->SetName( "camera8" );
    //m_camera8->SetRefBody( m_building_collider );
    m_scenegraph.RegisterNode( m_camera8 );

    m_camera8->LockOnBody( m_cube_body );



    ///////////////////////////////////////////////////////////////


    m_reticle_widget = _DRAWSPACE_NEW_( ReticleWidget, ReticleWidget( "reticle_widget", (long)10, (long)10, &m_scenegraph, NULL ) );
    



    m_text_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget", DRAWSPACE_GUI_WIDTH, DRAWSPACE_GUI_HEIGHT, m_font, true, m_reticle_widget ) );


    m_text_widget->GetBackgroundImage()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_text_widget->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );


    m_text_widget->GetBackgroundImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetBackgroundImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget->GetBackgroundImage()->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "reticle.bmp" ) ), 0 );
    m_text_widget->GetBackgroundImage()->GetTexture( 0 )->LoadFromFile();


    m_text_widget->GetImage()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );  
    m_text_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );

    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one"  ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    m_text_widget->GetImage()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_text_widget->GetImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetImage()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_text_widget->GetText()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );      
    m_text_widget->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_text_widget->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_text_widget->GetText()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetText()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_text_widget->GetText()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_text_widget->GetText()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );


    m_text_widget->GetText()->AddShaderParameter( 1, "color", 0 );
    m_text_widget->GetText()->SetShaderRealVector( "color", Utils::Vector( 1.0, 1.0, 0.0, 1.0 ) );

    m_text_widget->GetImage()->SetOrderNumber( 20000 );
    m_text_widget->GetInternalPass()->GetRenderingQueue()->UpdateOutputQueue();

    m_text_widget->RegisterToPass( m_finalpass );








    m_text_widget_2 = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget_2", 1200, 360, m_font, false, m_text_widget ) );
    
    m_text_widget_2->GetImage()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_text_widget_2->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget_2->GetImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );

    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one"  ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    m_text_widget_2->GetImage()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_text_widget_2->GetImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_2->GetImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget_2->GetText()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_text_widget_2->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.vsh", false ) ) );
    m_text_widget_2->GetText()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "text.psh", false ) ) );
    m_text_widget_2->GetText()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget_2->GetText()->GetFx()->GetShader( 1 )->LoadFromFile();
    m_text_widget_2->GetText()->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_text_widget_2->GetText()->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_text_widget_2->GetText()->AddShaderParameter( 1, "color", 0 );
    m_text_widget_2->GetText()->SetShaderRealVector( "color", Utils::Vector( 0.0, 1.0, 0.0, 1.0 ) );


    m_text_widget_2->GetImage()->SetOrderNumber( 20000 );
    m_text_widget_2->GetInternalPass()->GetRenderingQueue()->UpdateOutputQueue();

    m_text_widget_2->RegisterToPass( m_finalpass );














    //m_reticle_widget->LockOnBody( /*m_moon->GetOrbiter()*/ /*m_cube_body*/ m_building_collider );

    m_reticle_widget->LockOnTransformNode( /*m_building*/ m_socle );
    //m_reticle_widget->LockOnTransformNode( m_camera5 );


    DrawSpace::Gui::ReticleWidget::ClippingParams clp;
    clp.clipping_policy = DrawSpace::Gui::ReticleWidget::CLIPPING_HOLD;
    clp.xmin = -0.5;
    clp.xmax = 0.5;
    clp.ymin = -0.375;
    clp.ymax = 0.375;

    m_reticle_widget->SetClippingParams( clp );

    m_reticle_widget->RegisterMouseLeftButtonDownEventHandler( m_mouseleftbuttondown_eventhandler );


    m_mouse_input.RegisterWidget( m_reticle_widget );


    ///////////////////////////////////////////////////////////////




    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_freemove.Init( DrawSpace::Utils::Vector( 265000000.0, 0.0, 50.0, 1.0 ) );


    m_camera->RegisterMovement( &m_freemove );


    m_mouse_circularmode = true;


    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );

    m_calendar->RegisterWorld( &m_world );
    m_calendar->RegisterWorld( m_planet->GetWorld() );
    m_calendar->RegisterWorld( m_moon->GetWorld() );

    m_calendar->RegisterOrbit( m_orbit );
    m_calendar->RegisterOrbit( m_orbit2 );


    m_scenegraph.SetCurrentCamera( "camera7" );

    m_curr_camera = m_camera7;


    

    m_longlat_mvt2 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt2->Init( -21.0000, 20.000, 400014.5, 0.0, 0.0 );
    m_longlat_mvt2->Compute( m_timer );
    Matrix llres;
    m_longlat_mvt2->GetResult( llres );

    m_planet->RegisterIncludedInertBody( "simple_cube", m_cube_body, llres );


    m_longlat_mvt3 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt3->Init( -21.0000, 20.0089, 400114.0, 80.0, 0.0 );

    m_building_collider->RegisterMovement( m_longlat_mvt3 );


    m_planet->RegisterCollider( m_building_collider );






    m_longlat_mvt4 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt4->Init( -21.0929, 20.0000, 400129.0, 40.0, 0.0 );

    m_socle_collider->RegisterMovement( m_longlat_mvt4 );


    m_planet->RegisterCollider( m_socle_collider );





    m_planet->RegisterInertBody( "ship", m_ship );
    m_planet->RegisterCameraPoint( m_camera2 );
    m_planet->RegisterCameraPoint( m_camera3 );
    m_planet->RegisterCameraPoint( m_camera4 );
    m_planet->RegisterCameraPoint( m_camera5 );
    m_planet->RegisterCameraPoint( m_camera );

    m_planet->RegisterCameraPoint( m_camera6 );
    m_planet->RegisterCameraPoint( m_camera7 );
    m_planet->RegisterCameraPoint( m_camera8 );


    m_moon->RegisterInertBody( "ship", m_ship );
    m_moon->RegisterCameraPoint( m_camera2 );
    m_moon->RegisterCameraPoint( m_camera3 );
    m_moon->RegisterCameraPoint( m_camera4 );
    m_moon->RegisterCameraPoint( m_camera );
    m_moon->RegisterCameraPoint( m_camera6 );
    


    m_scenegraph.RegisterCameraEvtHandler( m_planet->GetCameraEvtCb() );
    m_scenegraph.RegisterCameraEvtHandler( m_moon->GetCameraEvtCb() );

    ///////////////////////////////////////////////////////////////



    //m_camera5->LockOnBody( m_moon->GetOrbiter() );
    //m_camera5->LockOnBody( m_ship );
    m_camera5->LockOnBody( m_cube_body );



    /**/
    //m_planet->GetDrawable()->SetCurrentPlanetBody( m_planet->GetPlanetBody() );
    /**/


    //m_calendar->Startup( 162682566 );
    m_calendar->Startup( 0 );


    return true;
}


void dsAppClient::OnAppInit( void )
{

}

void dsAppClient::OnClose( void )
{
}

void dsAppClient::OnKeyPress( long p_key ) 
{
    switch( p_key )
    {
        case 'Q':


            m_timer.TranslationSpeedInc( &m_speed, m_speed_speed );
            m_speed_speed *= 1.83;
          
            break;

        case 'W':

            m_timer.TranslationSpeedDec( &m_speed, m_speed_speed );
            m_speed_speed *= 1.86;
 
            break;


        case 'E':

            m_ship->ApplyUpPitch( 50000.0 );
            break;

        case 'C':

            m_ship->ApplyDownPitch( 50000.0 );
            break;

        case 'S':

            m_ship->ApplyLeftYaw( 50000.0 );
            break;

        case 'F':

            m_ship->ApplyRightYaw( 50000.0 );
            break;


        case 'Z':

            m_ship->ApplyLeftRoll( 50000.0 );
            break;

        case 'R':

            m_ship->ApplyRightRoll( 50000.0 );
            break;

        case 'T':

            m_ship->ApplyFwdForce( 51000000.0 );
            break;



        case VK_SPACE:

            m_ship->ZeroSpeed();
            break;

        case VK_RETURN:

            m_ship->ApplyFwdForce( 110000.0 );
            break;

        case VK_UP:

            m_ship->ApplyFwdForce( -30000.0 );
            break;

        case VK_DOWN:

            m_ship->ApplyDownForce( -10000.0 );
            break;


        case VK_LEFT:

            m_circular_mvt->SetAngularSpeed( 30.0 );
            break;

        case VK_RIGHT:

            m_circular_mvt->SetAngularSpeed( -30.0 );
            break;


    }
}

void dsAppClient::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':

            m_speed_speed = 5.0;
            break;


        case 'W':

            m_speed_speed = 5.0;                        
            break;

        case VK_SPACE:

            m_speed = 0.0;
            break;

        case VK_LEFT:
        case VK_RIGHT:
            m_circular_mvt->SetAngularSpeed( 0.0 );
            break;

    }
}

void dsAppClient::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:

            m_calendar->SetTimeFactor( Calendar::NORMAL_TIME );
            break;

        case VK_F2:

            m_calendar->SetTimeFactor( Calendar::MUL2_TIME );
            break;

        case VK_F3:

            m_calendar->SetTimeFactor( Calendar::MUL10_TIME );
            break;

        case VK_F4:

            m_calendar->SetTimeFactor( Calendar::MUL100_TIME );
            break;

        case VK_F5:

            m_calendar->SetTimeFactor( Calendar::SEC_1HOUR_TIME );
            break;


        case VK_F6:

            m_calendar->SetTimeFactor( Calendar::SEC_1DAY_TIME );
            break;

            /*
        case VK_F8:
            {

                DrawSpace::SphericalLOD::Maps* maps = m_planet->GetDrawable()->GetBody()->GetFaceMapsFactory( DrawSpace::SphericalLOD::Patch::FrontPlanetFace );

                dsreal alt = maps->GetAltFromLocalPoint( Vector( 0.499, 0.499, 0.0, 1.0 ) );

            }

            break;
            */

        case VK_F9:

            if( m_curr_camera == m_camera2 )
            {
                m_scenegraph.SetCurrentCamera( "camera3" );
                m_curr_camera = m_camera3;
            }
            else if( m_curr_camera == m_camera3 )
            {
                m_scenegraph.SetCurrentCamera( "camera4" );
                m_curr_camera = m_camera4;
            }
            else if( m_curr_camera == m_camera4 )
            {
                m_scenegraph.SetCurrentCamera( "camera5" );
                m_curr_camera = m_camera5;
            }
            else if( m_curr_camera == m_camera5 )
            {
                m_scenegraph.SetCurrentCamera( "camera6" );
                m_curr_camera = m_camera6;
            }

            else if( m_curr_camera == m_camera6 )
            {
                m_scenegraph.SetCurrentCamera( "camera7" );
                m_curr_camera = m_camera7;
            }

            else if( m_curr_camera == m_camera7 )
            {
                m_scenegraph.SetCurrentCamera( "camera8" );
                m_curr_camera = m_camera8;
            }

            else if( m_curr_camera == m_camera8 )
            {
                m_scenegraph.SetCurrentCamera( "camera" );
                m_curr_camera = m_camera;
            }

            else if( m_curr_camera == m_camera )
            {
                m_scenegraph.SetCurrentCamera( "camera2" );
                m_curr_camera = m_camera2;
            }


            break;

    }
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
    if( !m_mouserb )
    {  
	    m_freemove.RotateYaw( - p_dx / 4.0, m_timer );
	    m_freemove.RotatePitch( - p_dy / 4.0, m_timer );
    }
    else
    {
        m_freemove.RotateRoll( -p_dx / 4.0, m_timer );
    }

}

void dsAppClient::OnMouseLeftButtonDown( long p_xm, long p_ym )
{
    m_mouselb = true;
}

void dsAppClient::OnMouseLeftButtonUp( long p_xm, long p_ym )
{
    m_mouselb = false;
}

void dsAppClient::OnMouseRightButtonDown( long p_xm, long p_ym )
{
    m_mouserb = true;
}

void dsAppClient::OnMouseRightButtonUp( long p_xm, long p_ym )
{
    m_mouserb = false;
}

void dsAppClient::OnAppEvent( WPARAM p_wParam, LPARAM p_lParam )
{

}

void dsAppClient::on_mouseleftbuttondown( DrawSpace::Gui::Widget* p_widget )
{

}
