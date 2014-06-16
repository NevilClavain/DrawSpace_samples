
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




MyPlanet::MyPlanet( const dsstring& p_name, dsreal p_ray ) : 
m_name( p_name ),
m_ray( /*12000000.0*/ /*600000.0*/ p_ray * 1000.0 ),
m_collision_state( false ),
//m_player_relative( false ),
//m_player_body( NULL ),
m_suspend_update( false ),
m_collisionmeshebuild_counter( 0 )
{
    m_mediator = Mediator::GetInstance();

    m_world.Initialize();

    m_drawable = _DRAWSPACE_NEW_( DrawSpace::Planet::Body, DrawSpace::Planet::Body( m_ray * 2.0 ) );
    m_drawable->SetName( p_name );
    m_drawable->SetRenderer( DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );

    m_orbiter = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Orbiter, DrawSpace::Dynamics::Orbiter( &m_world, m_drawable ) );

    m_planet_evt_cb = _DRAWSPACE_NEW_( PlanetEvtCb, PlanetEvtCb( this, &MyPlanet::on_planet_event ) );
    m_drawable->RegisterEventHandler( m_planet_evt_cb );


    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &MyPlanet::on_camera_event ) );

    dsstring reqevtname = p_name + dsstring( "_ReqBuildMesheEvent" );
    dsstring doneevtname = p_name + dsstring( "_DoneBuildMesheEvent" );



    m_buildmeshe_event = m_mediator->CreateEvent( reqevtname );
    
    m_buildmeshe_event->args->AddProp<Meshe*>( "patchmeshe" );
    m_buildmeshe_event->args->AddProp<dsreal>( "sidelength" );
    m_buildmeshe_event->args->AddProp<dsreal>( "xpos" );
    m_buildmeshe_event->args->AddProp<dsreal>( "ypos" );
    m_buildmeshe_event->args->AddProp<int>( "orientation" );


    m_runner_evt_cb = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &MyPlanet::on_meshebuild_request ) );
    m_runner = _DRAWSPACE_NEW_( Runner, Runner );

    m_runner->RegisterEventHandler( m_buildmeshe_event, m_runner_evt_cb );

    m_task = _DRAWSPACE_NEW_( Task<Runner>, Task<Runner> );
    m_task->Startup( m_runner );
}

MyPlanet::~MyPlanet( void )
{
    _DRAWSPACE_DELETE_( m_orbiter );
    _DRAWSPACE_DELETE_( m_drawable );
    _DRAWSPACE_DELETE_( m_task );
}

MyPlanet::CameraEvtCb* MyPlanet::GetCameraEvtCb( void )
{
    return m_camera_evt_cb;
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

void MyPlanet::on_planet_event( Planet::Body* p_body, int p_currentface )
{
    if( "" == m_current_camerapoint )
    {
        return;
    }

    if( !m_registered_camerapoints[m_current_camerapoint].update_meshe )
    {
        return;
    }

    long tri_index = 0;
    dsreal alt = p_body->GetAltitud();

    if( alt < 1000.0 )
    {
        if( !m_suspend_update )
        {
            Planet::Patch* curr_patch = p_body->GetFaceCurrentLeaf( p_currentface );

            dsreal xpos, ypos;
            curr_patch->GetPos( xpos, ypos );


            if( m_collision_state )
            {
                m_orbiter->RemoveFromWorld();
                m_orbiter->UnsetKinematic();
            }

            m_suspend_update = true;

            m_meshe_ready_mutex.WaitInfinite();
            m_meshe_ready = false;
            m_meshe_ready_mutex.Release();


            m_buildmeshe_event->args->SetPropValue<Meshe*>( "patchmeshe", p_body->GetPatcheMeshe() );
            m_buildmeshe_event->args->SetPropValue<dsreal>( "sidelength", curr_patch->GetSideLength() / m_ray );
            m_buildmeshe_event->args->SetPropValue<dsreal>( "xpos", xpos / m_ray );
            m_buildmeshe_event->args->SetPropValue<dsreal>( "ypos", ypos / m_ray );
            m_buildmeshe_event->args->SetPropValue<int>( "orientation", curr_patch->GetOrientation() );

            m_buildmeshe_event->Notify();            

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
        }
    }
}


void MyPlanet::on_meshebuild_request( PropertyPool* p_args )
{
    //localy copy inputs

    DrawSpace::Core::Meshe patchmeshe;
    int patch_orientation;
    dsreal sidelength;
    dsreal xpos, ypos;


    patchmeshe = *( p_args->GetPropValue<Meshe*>( "patchmeshe" ) );
    patch_orientation = p_args->GetPropValue<int>( "orientation" );
    sidelength = p_args->GetPropValue<dsreal>( "sidelength" );
    xpos = p_args->GetPropValue<dsreal>( "xpos" );
    ypos = p_args->GetPropValue<dsreal>( "ypos" );


    ////////////////////////////// do the work

    Meshe final_meshe;

    build_meshe( patchmeshe, patch_orientation, sidelength, xpos, ypos, final_meshe );


    Body::Parameters params;


    params.mass = 0.0;

    params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
    params.shape_descr.meshe = final_meshe;

    m_orbiter->SetKinematic( params );

    ////////////////////////////////////////////

    m_meshe_ready_mutex.WaitInfinite();
    m_meshe_ready = true;
    m_meshe_ready_mutex.Release();

    Sleep( 25 );
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

void MyPlanet::attach_body( DrawSpace::Dynamics::InertBody* p_body )
{
    p_body->Attach( m_orbiter );
    m_registered_bodies[p_body].attached = true;
}

void MyPlanet::detach_body( DrawSpace::Dynamics::InertBody* p_body )
{
    p_body->Detach();
    m_registered_bodies[p_body].attached = false;
}

// PROVISOIRE
/*
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
*/

void MyPlanet::on_camera_event( DrawSpace::Scenegraph::CameraEvent p_event, DrawSpace::Core::TransformNode* p_node )
{
    if( DrawSpace::Scenegraph::ACTIVE == p_event )
    {
        if( !p_node )
        {
            return;
        }

        dsstring current_camera_name;
        p_node->GetName( current_camera_name );

        if( m_registered_camerapoints.count( current_camera_name ) > 0 )
        {
            m_current_camerapoint = current_camera_name;
            
            switch( m_registered_camerapoints[m_current_camerapoint].type )
            {
                case INERTBODY_LINKED:
                    {
                        if( m_registered_bodies[m_registered_camerapoints[m_current_camerapoint].attached_body].attached )
                        {
                            m_registered_camerapoints[m_current_camerapoint].hot = true;
                        }
                        else
                        {
                            m_registered_camerapoints[m_current_camerapoint].hot = false;
                        }
                    }
                    break;

                case FREE_ON_PLANET:
                    {
                        // TODO
                    }
                    break;

                case FREE:
                    {
                        // TODO
                    }
                    break;
            }
        }
        else
        {
            // camera non enregistree
            m_current_camerapoint = "";
        }
    }
}


void MyPlanet::Update( void )
{
    if( m_current_camerapoint != "" )
    {
        if( m_registered_camerapoints[m_current_camerapoint].hot )
        {
            if( m_suspend_update )
            {
                bool read_status = m_meshe_ready_mutex.Wait( 0 );

                if( read_status )
                {
                    bool meshe_ready = m_meshe_ready;
                    m_meshe_ready_mutex.Release();

                    if( meshe_ready )
                    {
                        // bullet meshe build done
                        m_orbiter->AddToWorld();
                        m_collision_state = true;
                        m_suspend_update = false;

                        m_collisionmeshebuild_counter++;
                    }
                }
            }

            DrawSpace::Utils::Matrix camera_pos;

            switch( m_registered_camerapoints[m_current_camerapoint].type )
            {
                case INERTBODY_LINKED:
                    {
                        m_registered_camerapoints[m_current_camerapoint].attached_body->GetLastLocalWorldTrans( camera_pos );

                        DrawSpace::Utils::Vector hotpoint;

                        hotpoint[0] = camera_pos( 3, 0 );
                        hotpoint[1] = camera_pos( 3, 1 );
                        hotpoint[2] = camera_pos( 3, 2 );

                        m_drawable->UpdateHotPoint( hotpoint );
                        m_drawable->Compute();
                        
                    }
                    break;

                case FREE_ON_PLANET:
                    {
                        // TODO
                    }
                    break;

                case FREE:
                    {
                        // TODO
                    }
                    break;
            }
        }
    }
}

long MyPlanet::GetCollisionMesheBuildCount( void )
{
    return m_collisionmeshebuild_counter;
}

/*
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

        if( rel_alt >= 1.2 )
        {
            DetachBody( p_player_body );
            m_player_relative = false;
            m_player_body = NULL;

            dsAppClient::GetInstance()->SetRelativePlanet( NULL );
        }
        else
        {
            if( m_suspend_update )
            {
                bool read_status = m_meshe_ready_mutex.Wait( 0 );

                if( read_status )
                {
                    bool meshe_ready = m_meshe_ready;
                    m_meshe_ready_mutex.Release();

                    if( meshe_ready )
                    {
                        // bullet meshe build done
                        m_orbiter->AddToWorld();
                        m_collision_state = true;

                        m_suspend_update = false;
                       
                    }

                }
            }

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

        if( ( delta.Length() / m_ray ) < 1.1 )
        {
            AttachBody( p_player_body );
            m_player_relative = true;
            m_suspend_update = false;
            m_player_body = p_player_body;

            dsAppClient::GetInstance()->SetRelativePlanet( this );
        }
    }
}
*/

void MyPlanet::RegisterInertBody( DrawSpace::Dynamics::InertBody* p_body )
{
    RegisteredBody reg_body;

    reg_body.attached = false;
    reg_body.body = p_body;

    m_registered_bodies[p_body] = reg_body;
}

bool MyPlanet::RegisterCameraPoint( DrawSpace::Dynamics::CameraPoint* p_camera, bool p_update_meshe )
{   
    RegisteredCamera reg_camera;

    dsstring camera_name;
    p_camera->GetName( camera_name );

    reg_camera.update_meshe = p_update_meshe;
    reg_camera.hot = false;
    reg_camera.camera = p_camera;

    ////

    Body* attached_body = p_camera->GetAttachedBody();

    if( attached_body )
    {
        InertBody* inert_body = dynamic_cast<InertBody*>( attached_body );
        if( inert_body )
        {
            if( m_registered_bodies.count( inert_body ) > 0 )
            {
                reg_camera.type = INERTBODY_LINKED;
                reg_camera.attached_body = inert_body;
            }
            else
            {
                // body attache n'est pas enregistre, erreur
                return false;
            }
        }
        else
        {
            Orbiter* orbiter = dynamic_cast<Orbiter*>( attached_body );
            if( orbiter )
            {
                // attachee a un orbiter
                if( orbiter == m_orbiter )
                {
                    // la camera est attachee a notre planete !

                    reg_camera.type = FREE_ON_PLANET;
                    reg_camera.attached_body = NULL;
                }
                else
                {
                    // la camera est attachee a un autre orbiter
                    return false;
                }
            }
            else
            {
                // camera attachee a autre chose qu'un orbiter ou un inertbody (???)
                return false;
            }
        }
    }
    else
    {
        // camera libre (attachee a aucun body)
        reg_camera.type = FREE;
        reg_camera.attached_body = NULL;
    }

    ////

    m_registered_camerapoints[camera_name] = reg_camera;
    return true;
}

bool MyPlanet::body_find_attached_camera( DrawSpace::Dynamics::InertBody* p_body, dsstring& p_name )
{
    for( std::map<dsstring, RegisteredCamera>::iterator it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
    {
        if( it->second.camera->GetAttachedBody() == p_body )
        {
            p_name = it->first;
            return true;
        }
    }
    return false;
}

void MyPlanet::ManageBodies( void )
{
    //for( size_t i = 0; i < m_registered_bodies.size(); i++ )

    for( std::map<DrawSpace::Dynamics::InertBody*, RegisteredBody>::iterator it = m_registered_bodies.begin();
        it != m_registered_bodies.end(); ++it )
    {
        //if( m_registered_bodies[i].attached )

        if( it->second.attached )
        {
            DrawSpace::Utils::Matrix bodypos;
            //m_registered_bodies[i].body->GetLastLocalWorldTrans( bodypos );

            it->second.body->GetLastLocalWorldTrans( bodypos );

            DrawSpace::Utils::Vector bodypos2;
            bodypos2[0] = bodypos( 3, 0 );
            bodypos2[1] = bodypos( 3, 1 );
            bodypos2[2] = bodypos( 3, 2 );

            dsreal rel_alt = ( bodypos2.Length() / m_ray );

            if( rel_alt >= 1.2 )
            {
                //detach_body( m_registered_bodies[i].body );
                detach_body( it->second.body );

                // rechercher si une camera enregistree est associee a ce body
                dsstring body_camera_name;
                if( body_find_attached_camera( it->second.body, body_camera_name ) )
                {
                    m_registered_camerapoints[body_camera_name].hot = false;
                }

                //////


                if( m_collision_state )
                {
                    if( !m_suspend_update )
                    {
                        m_orbiter->RemoveFromWorld();
                        m_orbiter->UnsetKinematic();
                    }
                    m_collision_state = false;
                }


                //////

                for( size_t i = 0; i < m_relative_evt_handlers.size(); i++ )
                {
                    ( *( m_relative_evt_handlers[i] ) )( NULL );
                }
            }
        }
        else
        {
            DrawSpace::Utils::Matrix bodypos;
            //m_registered_bodies[i].body->GetLastLocalWorldTrans( bodypos );
            it->second.body->GetLastLocalWorldTrans( bodypos );

            DrawSpace::Utils::Vector bodypos2;
            bodypos2[0] = bodypos( 3, 0 );
            bodypos2[1] = bodypos( 3, 1 );
            bodypos2[2] = bodypos( 3, 2 );

            DrawSpace::Utils::Matrix planetbodypos;
            m_orbiter->GetLastWorldTransformation( planetbodypos );

            DrawSpace::Utils::Vector planetbodypos2;
            planetbodypos2[0] = planetbodypos( 3, 0 );
            planetbodypos2[1] = planetbodypos( 3, 1 );
            planetbodypos2[2] = planetbodypos( 3, 2 );

            Vector delta;

            delta[0] = planetbodypos2[0] - bodypos2[0];
            delta[1] = planetbodypos2[1] - bodypos2[1];
            delta[2] = planetbodypos2[2] - bodypos2[2];
            delta[3] = 1.0;

            dsreal rel_alt = delta.Length() / m_ray;

            if( rel_alt < 1.1 )
            {
                //attach_body( m_registered_bodies[i].body );
                attach_body( it->second.body );

                // rechercher si une camera enregistree est associee a ce body

                dsstring body_camera_name;
                if( body_find_attached_camera( it->second.body, body_camera_name ) )
                {
                    m_registered_camerapoints[body_camera_name].hot = true;
                }

                /////

                for( size_t i = 0; i < m_relative_evt_handlers.size(); i++ )
                {
                    ( *( m_relative_evt_handlers[i] ) )( this );
                }
            }
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

void MyPlanet::RegisterRelativeEventHandler( PlanetRelativeEventHandler* p_handler )
{
    m_relative_evt_handlers.push_back( p_handler );
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

    /*
    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &dsAppClient::on_camera_event ) );
    m_body_evt_cb = _DRAWSPACE_NEW_( BodyEvtCb, BodyEvtCb( this, &dsAppClient::on_body_event ) );
    */

    m_planetrelative_evt_cb = _DRAWSPACE_NEW_( PlanetRelativeEvtCb, PlanetRelativeEvtCb( this, &dsAppClient::on_relative_to_planet ) );
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


    compute_player_view_transform();
    m_camera2->SetLocalTransform( m_player_view_transform );
    
    Matrix origin;
    origin.Identity();

    m_orbit->OrbitStep( origin );


    //m_planet->ApplyGravity();
    //m_moon->ApplyGravity();

    m_ship->Update();



           
    m_scenegraph.ComputeTransformations( m_timer );


    //m_planet->Update( m_ship );
    //m_moon->Update( m_ship );


    m_planet->Update();
    m_moon->Update();

    m_planet->ManageBodies();
    m_moon->ManageBodies();


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
        //renderer->DrawText( 0, 255, 0, 10, 155, "collision state %d", m_relative_planet->GetCollisionState() );
    }


    renderer->DrawText( 0, 255, 0, 10, 195, "planet01 : %d %d", m_planet->GetCollisionState(), m_planet->GetCollisionMesheBuildCount() );
    renderer->DrawText( 0, 255, 0, 10, 215, "moon : %d %d", m_moon->GetCollisionState(), m_moon->GetCollisionMesheBuildCount() );


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
    //World::m_scale = 0.5;
    World::m_scale = 1.0;


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


    m_planet = _DRAWSPACE_NEW_( MyPlanet, MyPlanet( "planet01", 400.0 ) );


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

    m_moon = _DRAWSPACE_NEW_( MyPlanet, MyPlanet( "moon", 300.0 ) );


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
    /*
    cube_params.initial_pos = DrawSpace::Utils::Vector( 265000000.0, 0.0, -10.0, 1.0 );
    cube_params.initial_rot.Identity();
    */

    cube_params.initial_attitude.Translation( 265000000.0, 0.0, -10.0 );

    m_ship = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Rocket, DrawSpace::Dynamics::Rocket( &m_world, m_ship_drawable, cube_params ) );


    //////////////////////////////////////////////////////////////

    //m_scenegraph.RegisterCameraEvtHandler( m_camera_evt_cb );


    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera" ) );
    m_scenegraph.RegisterNode( m_camera );

    
    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera2", m_ship ) );
    m_scenegraph.RegisterNode( m_camera2 );


    m_camera3 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera3", m_ship ) );
    m_scenegraph.RegisterNode( m_camera3 );


    m_camera4 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera4", m_ship ) );
    m_scenegraph.RegisterNode( m_camera4 );


    m_circular_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::CircularMovement, DrawSpace::Core::CircularMovement );
    m_circular_mvt->Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( 35.0, 0.0, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 0.0, 0.0, 0.0 );

    m_camera3->RegisterMovement( m_circular_mvt );

    m_camera3->LockOnBody( m_ship );

    m_camera4->RegisterMovement( m_circular_mvt );
    m_camera4->LockOnBody( m_planet->GetOrbiter() );



    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 0.0, 0.0, 1.0 ) );


    m_camera->RegisterMovement( &m_freemove );


    m_mouse_circularmode = true;


    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );

    m_calendar->RegisterWorld( &m_world );
    m_calendar->RegisterWorld( m_planet->GetWorld() );
    m_calendar->RegisterWorld( m_moon->GetWorld() );

    m_calendar->RegisterOrbit( m_orbit );
    m_calendar->RegisterOrbit( m_orbit2 );



    //m_scenegraph.SetCurrentCamera( "camera" );
    m_scenegraph.SetCurrentCamera( "camera2" );

    m_curr_camera = m_camera2;


    m_planet->RegisterInertBody( m_ship );
    m_planet->RegisterCameraPoint( m_camera2, true );
    m_planet->RegisterCameraPoint( m_camera3, true );
    m_planet->RegisterCameraPoint( m_camera4, true );


    m_moon->RegisterInertBody( m_ship );
    m_moon->RegisterCameraPoint( m_camera2, true );
    m_moon->RegisterCameraPoint( m_camera3, true );
    m_moon->RegisterCameraPoint( m_camera4, true );


    m_scenegraph.RegisterCameraEvtHandler( m_planet->GetCameraEvtCb() );
    m_scenegraph.RegisterCameraEvtHandler( m_moon->GetCameraEvtCb() );

    m_planet->RegisterRelativeEventHandler( m_planetrelative_evt_cb );
    m_moon->RegisterRelativeEventHandler( m_planetrelative_evt_cb );


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

            m_timer.TranslationSpeedDec( &m_speed, m_speed_speed );
            m_speed_speed *= 1.06;
 
            break;


        case 'E':

            m_ship->ApplyUpPitch( 50.0 );
            break;

        case 'C':

            m_ship->ApplyDownPitch( 50.0 );
            break;

        case 'S':

            m_ship->ApplyLeftYaw( 50.0 );
            break;

        case 'F':

            m_ship->ApplyRightYaw( 50.0 );
            break;


        case 'Z':

            m_ship->ApplyLeftRoll( 50.0 );
            break;

        case 'R':

            m_ship->ApplyRightRoll( 50.0 );
            break;


        case VK_SPACE:

            m_ship->ZeroSpeed();
            break;

        case VK_RETURN:

            m_ship->ApplyFwdForce( 30000.0 );
            break;

        case VK_UP:

            m_ship->ApplyFwdForce( -30000.0 );
            break;

        case VK_DOWN:

            m_ship->ApplyDownForce( -1000.0 );
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

DrawSpace::Dynamics::Rocket* dsAppClient::GetPlayerShip( void )
{
    return m_ship;
}

void dsAppClient::SetLastPlayerShipGravity( const DrawSpace::Utils::Vector& p_gravity )
{
    m_player_ship_gravity = p_gravity;
}

/*
void dsAppClient::SetRelativePlanet( MyPlanet* p_planet )
{
    m_relative_planet = p_planet;
}
*/

void dsAppClient::on_relative_to_planet( MyPlanet* p_planet )
{
    m_relative_planet = p_planet;
}