
#include "dsappclient.h"



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;

_DECLARE_DS_LOGGER( logger, "AppClient" )


#define SHIP_MASS 5.0


MyPlanet::MyPlanet( const dsstring& p_name ) : 
m_name( p_name ),
m_ray( /*12000000.0*/ 600000.0 ),
m_collision_state( false ),
m_buildmeshe_collision_state( false ),
m_player_relative( false ),
m_player_body( NULL ),
m_suspend_update( false )
{
    m_world.Initialize();

    m_drawable = _DRAWSPACE_NEW_( DrawSpace::Planet::Body, DrawSpace::Planet::Body( m_ray * 2.0 ) );
    m_drawable->SetName( p_name );
    m_drawable->SetRenderer( DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );

    m_orbiter = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Orbiter, DrawSpace::Dynamics::Orbiter( &m_world, m_drawable ) );

    m_planet_evt_cb = _DRAWSPACE_NEW_( PlanetEvtCb, PlanetEvtCb( this, &MyPlanet::on_planet_event ) );
    m_drawable->RegisterEventHandler( m_planet_evt_cb );

    m_task = _DRAWSPACE_NEW_( Task<MyPlanet>, Task<MyPlanet> );

    dsstring reqevtname = p_name + dsstring( "_ReqBuildMesheEvent" );
    dsstring doneevtname = p_name + dsstring( "_DoneBuildMesheEvent" );

    m_buildmeshe_request_event = CreateEvent( 
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        reqevtname.c_str() // object name
        );

    m_buildmeshe_done_event = CreateEvent( 
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        doneevtname.c_str()  // object name
        );

    m_task->Startup( this );
}

MyPlanet::~MyPlanet( void )
{
    _DRAWSPACE_DELETE_( m_orbiter );
    _DRAWSPACE_DELETE_( m_drawable );
    _DRAWSPACE_DELETE_( m_task );
}

DrawSpace::Planet::Body* MyPlanet::GetDrawable( void )
{
    return m_drawable;
}

DrawSpace::Dynamics::Orbiter* MyPlanet::GetOrbiter( void )
{
    return m_orbiter;
}

bool MyPlanet::GetCollisionState( void )
{
    return m_collision_state;
}

void MyPlanet::on_planet_event( int p_currentface )
{
    long tri_index = 0;

    dsreal alt = m_drawable->GetAltitud();

    if( alt < 1000.0 )
    {
        if( !m_suspend_update )
        {
            Planet::Patch* curr_patch = m_drawable->GetFaceCurrentLeaf( p_currentface );

            m_buildmeshe_inputs_mutex.WaitInfinite();

            m_buildmeshe_patchmeshe[0] = *( m_drawable->GetPatcheMeshe() );

            m_buildmeshe_sidelength[0] = curr_patch->GetSideLength() / m_ray;

            dsreal xpos, ypos;
            curr_patch->GetPos( xpos, ypos );

            m_buildmeshe_xpos[0] = xpos / m_ray;
            m_buildmeshe_ypos[0] = ypos / m_ray;

            m_buildmeshe_patch_orientation[0] = curr_patch->GetOrientation();



            m_buildmeshe_inputs_mutex.Release();

            // suspend planet update until meshe build is terminated

            m_suspend_update = true;

            
            if( m_collision_state )
            {
                m_orbiter->RemoveFromWorld();
                m_orbiter->UnsetKinematic();
            }
            

            SetEvent( m_buildmeshe_request_event );

            ////////////////////////////////////////////////
        }
    }
    else
    {
        if( m_collision_state )
        {
            if( !m_suspend_update )
            {
                m_orbiter->RemoveFromWorld();
                m_orbiter->UnsetKinematic();
            }
            m_collision_state = false;
            m_buildmeshe_collision_state = false;
        }
    }
}

dsreal MyPlanet::GetAltitud( void )
{
    return m_drawable->GetAltitud();
}

DrawSpace::Dynamics::World* MyPlanet::GetWorld( void )
{
    return &m_world;
}

void MyPlanet::GetName( dsstring& p_name )
{
    p_name = m_name;
}

void MyPlanet::AttachBody( DrawSpace::Dynamics::InertBody* p_body )
{
    p_body->Attach( m_orbiter );

    m_attached_bodies.push_back( p_body );
}

void MyPlanet::DetachBody( DrawSpace::Dynamics::InertBody* p_body )
{
    p_body->Detach();

    for( std::vector<DrawSpace::Dynamics::InertBody*>::iterator it = m_attached_bodies.begin(); it != m_attached_bodies.end(); ++it )
    {
        if( (*it) == p_body )
        {
            m_attached_bodies.erase( it );
            break;
        }
    }
}

void MyPlanet::ApplyGravity( void )
{
    for( size_t i = 0; i < m_attached_bodies.size(); i++ )
    {
        DrawSpace::Utils::Matrix local_pos;
        m_attached_bodies[i]->GetLastLocalWorldTrans( local_pos );

        Vector gravity;
        gravity[0] = -local_pos( 3, 0 );
        gravity[1] = -local_pos( 3, 1 );
        gravity[2] = -local_pos( 3, 2 );
        gravity[3] = 1.0;
        gravity.Normalize();
        gravity.Scale( SHIP_MASS * 9.81 );

        m_attached_bodies[i]->ApplyForce( gravity );

        if( m_attached_bodies[i] == dsAppClient::GetInstance()->GetPlayerShip() )
        {
            dsAppClient::GetInstance()->SetLastPlayerShipGravity( gravity );
        }
    }
}

void MyPlanet::Update( DrawSpace::Dynamics::InertBody* p_player_body )
{
    if( m_player_relative )
    {
        DrawSpace::Utils::Matrix playerbodypos;
        p_player_body->GetLastLocalWorldTrans( playerbodypos );

        DrawSpace::Utils::Vector playerbodypos2;
        playerbodypos2[0] = playerbodypos( 3, 0 );
        playerbodypos2[1] = playerbodypos( 3, 1 );
        playerbodypos2[2] = playerbodypos( 3, 2 );

        dsreal rel_alt = ( playerbodypos2.Length() / m_ray );

        if( rel_alt >= 2.5 )
        {
            DetachBody( p_player_body );
            m_player_relative = false;
            m_player_body = NULL;

            dsAppClient::GetInstance()->SetRelativePlanet( NULL );
        }
        else
        {
            if( m_suspend_update && WAIT_OBJECT_0 == WaitForSingleObject( m_buildmeshe_done_event, 0 ) )
            {
                // bullet meshe build done

                m_orbiter->AddToWorld();

                ResetEvent( m_buildmeshe_done_event );
                m_collision_state = true;

                m_suspend_update = false;
            }
            else
            {   

                DrawSpace::Utils::Matrix camera_pos;

                m_player_body->GetLastLocalWorldTrans( camera_pos );

                DrawSpace::Utils::Vector hotpoint;

                hotpoint[0] = camera_pos( 3, 0 );
                hotpoint[1] = camera_pos( 3, 1 );
                hotpoint[2] = camera_pos( 3, 2 );

                m_drawable->UpdateHotPoint( hotpoint );
                m_drawable->Compute();
            }
        }
    }
    else
    {
        DrawSpace::Utils::Matrix playerbodypos;
        p_player_body->GetLastLocalWorldTrans( playerbodypos );

        DrawSpace::Utils::Vector playerbodypos2;
        playerbodypos2[0] = playerbodypos( 3, 0 );
        playerbodypos2[1] = playerbodypos( 3, 1 );
        playerbodypos2[2] = playerbodypos( 3, 2 );

        DrawSpace::Utils::Matrix planetbodypos;
        m_orbiter->GetLastWorldTransformation( planetbodypos );

        DrawSpace::Utils::Vector planetbodypos2;
        planetbodypos2[0] = planetbodypos( 3, 0 );
        planetbodypos2[1] = planetbodypos( 3, 1 );
        planetbodypos2[2] = planetbodypos( 3, 2 );

        Vector delta;

        delta[0] = planetbodypos2[0] - playerbodypos2[0];
        delta[1] = planetbodypos2[1] - playerbodypos2[1];
        delta[2] = planetbodypos2[2] - playerbodypos2[2];
        delta[3] = 1.0;

        if( ( delta.Length() / m_ray ) < 2.0 )
        {
            AttachBody( p_player_body );
            m_player_relative = true;
            m_suspend_update = false;
            m_player_body = p_player_body;

            dsAppClient::GetInstance()->SetRelativePlanet( this );
        }
    }
}

void MyPlanet::build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, int p_patch_orientation, dsreal p_sidelength, dsreal p_xpos, dsreal p_ypos, DrawSpace::Core::Meshe& p_outmeshe )
{
    for( long i = 0; i < p_patchmeshe.GetVertexListSize(); i++ )
    {                

        Vertex v, v2, v3;
        p_patchmeshe.GetVertex( i, v );

        v.x = v.x * p_sidelength / 2.0;
        v.y = v.y * p_sidelength / 2.0;
        v.z = v.z * p_sidelength / 2.0;

        v.x += p_xpos;
        v.y += p_ypos;

        switch( p_patch_orientation )
        {
            case Planet::Patch::FrontPlanetFace:

                v2.x = v.x;
                v2.y = v.y;
                v2.z = 1.0;
                break;

            case Planet::Patch::RearPlanetFace:

                v2.x = -v.x;
                v2.y = v.y;
                v2.z = -1.0;
                break;

            case Planet::Patch::LeftPlanetFace:

                v2.x = -1.0;
                v2.y = v.y;
                v2.z = v.x;
                break;

            case Planet::Patch::RightPlanetFace:

                v2.x = 1.0;
                v2.y = v.y;
                v2.z = -v.x;
                break;

            case Planet::Patch::TopPlanetFace:

                v2.x = v.x;
                v2.y = 1.0;
                v2.z = -v.y;
                break;

            case Planet::Patch::BottomPlanetFace:

                v2.x = v.x;
                v2.y = -1.0;
                v2.z = v.y;
                break;
        }

        dsreal xtemp = v2.x;
        dsreal ytemp = v2.y;
        dsreal ztemp = v2.z;

        v2.x = xtemp * sqrt( 1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp / 3.0 );
        v2.y = ytemp * sqrt( 1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp / 3.0 );
        v2.z = ztemp * sqrt( 1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp / 3.0 );

        v3.x = v2.x * m_ray;
        v3.y = v2.y * m_ray;
        v3.z = v2.z * m_ray;

        p_outmeshe.AddVertex( v3 );
    }

    for( long i = 0; i < p_patchmeshe.GetTrianglesListSize(); i++ )
    {
        Triangle t;
        p_patchmeshe.GetTriangles( i, t );
        p_outmeshe.AddTriangle( t );
    }
}

void MyPlanet::Run( void )
{
    while( 1 )
    {
        DWORD wait = WaitForSingleObject( m_buildmeshe_request_event, INFINITE );

        if( WAIT_OBJECT_0 == wait )
        {

            m_buildmeshe_inputs_mutex.WaitInfinite();

            //localy copy inputs

            DrawSpace::Core::Meshe patchmeshe[9];
            int patch_orientation[9];
            dsreal sidelength[9];
            dsreal xpos[9], ypos[9];
            
            /*
            for( long i = 0; i < 9; i++ )
            {
                patchmeshe[i] = m_buildmeshe_patchmeshe[i];
                sidelength[i] = m_buildmeshe_sidelength[i];
                xpos[i] = m_buildmeshe_xpos[i];
                ypos[i] = m_buildmeshe_ypos[i];
                patch_orientation[i] = m_buildmeshe_patch_orientation[i];
            }
            */

            patchmeshe[0] = m_buildmeshe_patchmeshe[0];
            sidelength[0] = m_buildmeshe_sidelength[0];
            xpos[0] = m_buildmeshe_xpos[0];
            ypos[0] = m_buildmeshe_ypos[0];
            patch_orientation[0] = m_buildmeshe_patch_orientation[0];


            m_buildmeshe_inputs_mutex.Release();


            ////////////////////////////// do the work

            Meshe final_meshe;

            build_meshe( patchmeshe[0], patch_orientation[0], sidelength[0], xpos[0], ypos[0], final_meshe );


            Body::Parameters params;


            params.mass = 0.0;
            params.initial_pos = DrawSpace::Utils::Vector( 0.0, 0.0, 0.0, 1.0 );
            params.initial_rot.Identity();
            
            params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
            params.shape_descr.meshe = final_meshe;

        

            m_orbiter->SetKinematic( params );

            ////////////////////////////////////////////


            ResetEvent( m_buildmeshe_request_event );
            SetEvent( m_buildmeshe_done_event );

            m_buildmeshe_collision_state = true;

        }

        Sleep( 25 );
    }
}

bool MyPlanet::IsPlayerRelative( void )
{
    return m_player_relative;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

dsAppClient::dsAppClient( void ) : 
m_mouselb( false ), 
m_mouserb( false ), 
m_speed( 0.0 ), 
m_speed_speed( 5.0 ),
m_relative_planet( NULL )
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


    /*
    if( m_player_view_linear_acc[2] > 0.0 )
    {
        dsreal limit = ( m_player_view_linear_acc[2] / ( 8000.0 / SHIP_MASS ) );
        if( m_player_view_pos[2] < limit )
        {
            m_player_view_linear_speed[2] = SPEED;
        }
        else
        {
            m_player_view_linear_speed[2] = 0.0;
        }
    }
    else if( m_player_view_linear_acc[2] < 0.0 )
    {
        dsreal limit = ( m_player_view_linear_acc[2] / ( 8000.0 / SHIP_MASS ) );
        if( m_player_view_pos[2] > limit )
        {
            m_player_view_linear_speed[2] = -SPEED;
        }
        else
        {
            m_player_view_linear_speed[2] = 0.0;
        }
    }
    else
    {
        if( m_player_view_pos[2] > 0.1 )
        {
            m_player_view_linear_speed[2] = -SPEED;
        }

        else if( m_player_view_pos[2] < -0.1 )
        {
            m_player_view_linear_speed[2] = SPEED;
        }

        else
        {
            m_player_view_linear_speed[2] = 0.0;
        }
    }
    */

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
    m_freemove.Compute( m_timer );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    
    DrawSpace::Utils::Matrix sbtrans;
    sbtrans.Scale( 20.0, 20.0, 20.0 );
    m_scenegraph.SetNodeLocalTransformation( "spacebox", sbtrans );


    compute_player_view_transform();
    m_camera2->SetLocalTransform( m_player_view_transform );
    
    Matrix origin;
    origin.Identity();

    m_orbit->OrbitStep( origin );


    m_planet->ApplyGravity();
    m_moon->ApplyGravity();

    m_ship->Update();



           
    m_scenegraph.ComputeTransformations();


    m_planet->Update( m_ship );
    m_moon->Update( m_ship );




    m_texturepass->GetRenderingQueue()->Draw();
    m_finalpass->GetRenderingQueue()->Draw();


    long current_fps = m_timer.GetFPS();
    renderer->DrawText( 0, 255, 0, 10, 35, "%d", current_fps );

    dsstring date;
    m_calendar->GetFormatedDate( date );    
    renderer->DrawText( 0, 255, 0, 10, 55, "%s", date.c_str() );

    


    dsreal speed = m_ship->GetLinearSpeedMagnitude();

    renderer->DrawText( 0, 255, 0, 10, 95, "speed = %.1f km/h ( %.1f m/s) - aspeed = %.1f", speed * 3.6, speed, m_ship->GetAngularSpeedMagnitude() );

    renderer->DrawText( 0, 255, 0, 10, 115, "contact = %d", m_ship->GetContactState() );

    if( m_relative_planet )
    {
        

        dsstring planet_name;
        m_relative_planet->GetName( planet_name );
        renderer->DrawText( 0, 255, 0, 10, 135, "relative to : %s altitud = %f", planet_name.c_str(), m_relative_planet->GetAltitud() );
        renderer->DrawText( 0, 255, 0, 10, 155, "collision state %d", m_relative_planet->GetCollisionState() );
    }

    Vector tf, tt;
    Vector tf2, tt2;

    Matrix ship_trans;

    m_ship->GetLastLocalWorldTrans( ship_trans );
    ship_trans.ClearTranslation();
    ship_trans.Inverse();

    m_ship->GetTotalForce( tf );
    m_ship->GetTotalTorque( tt );

    
    if( m_relative_planet )
    {
        // remove gravity effect

        // had to scale my ship gravity vector by 0.5, but dont understand why...

        tf[0] = tf[0] - 0.5 * m_player_ship_gravity[0];
        tf[1] = tf[1] - 0.5 * m_player_ship_gravity[1];
        tf[2] = tf[2] - 0.5 * m_player_ship_gravity[2];
    }

    
    ship_trans.Transform( &tf, &tf2 );
    ship_trans.Transform( &tt, &tt2 );
        
    m_player_view_linear_acc[2] = -tf2[2] / SHIP_MASS;

    m_player_view_angular_acc[2] = -tt2[2];
    m_player_view_angular_acc[1] = -tt2[1];
    m_player_view_angular_acc[0] = -tt2[0];







    renderer->FlipScreen();

    m_calendar->Run();

    m_freemove.SetSpeed( m_speed );
}

bool dsAppClient::OnIdleAppInit( void )
{
    World::m_scale = 0.5;


    bool status;

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    if( !status )
    {
        _DSEXCEPTION( "Cannot load ac3dmeshe plugin !" )
    }
    _DSDEBUG( logger, "ac3dmeshe plugin successfully loaded..." );

    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );

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
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    

    //////////////////////////////////////////////////////////////

    m_spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
    m_spacebox->RegisterPassSlot( "texture_pass" );
    m_spacebox->SetRenderer( renderer );
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


    m_planet = _DRAWSPACE_NEW_( MyPlanet, MyPlanet( "planet01" ) );


    m_planet->GetDrawable()->RegisterPassSlot( "texture_pass" );

    for( long i = 0; i < 6; i++ )
    {
        m_planet->GetDrawable()->SetNodeFromPassSpecificFx( "texture_pass", i, "main_fx" );
    }
    m_scenegraph.RegisterNode( m_planet->GetDrawable() );

    m_planet->GetDrawable()->Initialize();

    

    m_centroid = _DRAWSPACE_NEW_( Centroid, Centroid );
    m_centroid->SetOrbiter( m_planet->GetOrbiter() );

    m_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 270000000.0, 0.99, 0.0, 0.0, 0.0, 0.0, 0.333, 25.0, 1.0, m_centroid ) );


    //////////////////////////////////////////////////////////////

    m_moon = _DRAWSPACE_NEW_( MyPlanet, MyPlanet( "moon" ) );


    m_moon->GetDrawable()->RegisterPassSlot( "texture_pass" );

    for( long i = 0; i < 6; i++ )
    {
        m_moon->GetDrawable()->SetNodeFromPassSpecificFx( "texture_pass", i, "main_fx" );
    }
    m_scenegraph.RegisterNode( m_moon->GetDrawable() );

    m_moon->GetDrawable()->Initialize();

    m_centroid2 = _DRAWSPACE_NEW_( Centroid, Centroid );
    m_centroid2->SetOrbiter( m_moon->GetOrbiter() );

    m_orbit2 = _DRAWSPACE_NEW_( Orbit, Orbit( 20000000.0, 0.99, 0.0, 0.0, 0.0, 0.0, 0.002, 0.0, 1.0, m_centroid2 ) );

    m_centroid->RegisterSubOrbit( m_orbit2 );



    //////////////////////////////////////////////////////////////


    m_ship_drawable = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ship_drawable->RegisterPassSlot( "texture_pass" );
    m_ship_drawable->SetRenderer( renderer );

    m_ship_drawable->SetName( "rocket" );
    
    m_ship_drawable->GetMeshe()->SetImporter( m_meshe_import );

    m_ship_drawable->GetMeshe()->LoadFromFile( "object3.ac", 0 );    

    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_ship_drawable->GetNodeFromPass( "texture_pass" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );



    m_ship_drawable->GetNodeFromPass( "texture_pass" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( m_ship_drawable );


    DrawSpace::Dynamics::Body::Parameters cube_params;
    cube_params.mass = SHIP_MASS;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 2.0, 0.5, 4.0, 1.0 );
    cube_params.initial_pos = DrawSpace::Utils::Vector( 265000000.0, 0.0, -10.0, 1.0 );
    cube_params.initial_rot.Identity();

    m_ship = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Rocket, DrawSpace::Dynamics::Rocket( &m_world, m_ship_drawable, cube_params ) );





    //////////////////////////////////////////////////////////////

    m_camera = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera" ) );
    m_scenegraph.RegisterNode( m_camera );

    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera2" ) );
    m_scenegraph.RegisterNode( m_camera2 );

    m_ship_drawable->AddChild( m_camera2 );


    //m_scenegraph.SetCurrentCamera( "camera" );
    m_scenegraph.SetCurrentCamera( "camera2" );

    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_freemove.SetTransformNode( m_camera );
    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 0.0, 0.0, 1.0 ) );


    m_mouse_circularmode = true;


    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );

    m_calendar->RegisterWorld( &m_world );
    m_calendar->RegisterWorld( m_planet->GetWorld() );
    m_calendar->RegisterWorld( m_moon->GetWorld() );

    m_calendar->RegisterOrbit( m_orbit );
    m_calendar->RegisterOrbit( m_orbit2 );

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
            m_speed_speed *= 1.03;
          
            break;

        case 'W':

            /*
            if( m_speed < 1.0 )
            {
                m_speed = 0.0;
            }
            */

            m_timer.TranslationSpeedDec( &m_speed, m_speed_speed );
            m_speed_speed *= 1.06;
 
            break;


        case 'E':

            m_ship->ApplyUpPitch( 1.0 );
            break;

        case 'C':

            m_ship->ApplyDownPitch( 1.0 );
            break;

        case 'S':

            m_ship->ApplyLeftYaw( 1.0 );
            break;

        case 'F':

            m_ship->ApplyRightYaw( 1.0 );
            break;


        case 'Z':

            m_ship->ApplyLeftRoll( 1.0 );
            break;

        case 'R':

            m_ship->ApplyRightRoll( 1.0 );
            break;


        case VK_SPACE:

            m_ship->ZeroSpeed();
            break;

        case VK_RETURN:

            m_ship->ApplyFwdForce( 3000.0 );
            break;

        case VK_UP:

            m_ship->ApplyFwdForce( -3000.0 );
            break;

        case VK_DOWN:

            m_ship->ApplyDownForce( -100.0 );
            break;


        case 'I':
            m_player_view_linear_acc[2] = 500.0;
            break;

        case 'K':
            m_player_view_linear_acc[2] = -500.0;
            break;

        case 'U':
            m_player_view_linear_acc[1] = 500.0;
            break;

        case 'J':
            m_player_view_linear_acc[1] = -500.0;
            break;

        case 'Y':

            m_player_view_angular_acc[0] = 1.0;
            break;

        case 'H':

            m_player_view_angular_acc[0] = -1.0;
            break;


        case 'T':

            m_player_view_angular_acc[2] = 1.0;
            break;

        case 'G':

            m_player_view_angular_acc[2] = -1.0;
            break;


        case 'N':

            m_player_view_angular_acc[1] = 1.0;
            break;

        case 'V':

            m_player_view_angular_acc[1] = -1.0;
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


        case 'I':
        case 'K':

            m_player_view_linear_acc[2] = 0.0;
            break;


        case 'U':
        case 'J':

            m_player_view_linear_acc[1] = 0.0;
            break;

        case 'Y':
        case 'H':

            m_player_view_angular_acc[0] = 0.0;
            break;


        case 'T':
        case 'G':

            m_player_view_angular_acc[2] = 0.0;
            break;

        case 'V':
        case 'N':

            m_player_view_angular_acc[1] = 0.0;
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

        case VK_F9:
            break;

        case VK_F10:

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

DrawSpace::Dynamics::Rocket* dsAppClient::GetPlayerShip( void )
{
    return m_ship;
}

void dsAppClient::SetLastPlayerShipGravity( const DrawSpace::Utils::Vector& p_gravity )
{
    m_player_ship_gravity = p_gravity;
}

void dsAppClient::SetRelativePlanet( MyPlanet* p_planet )
{
    m_relative_planet = p_planet;
}