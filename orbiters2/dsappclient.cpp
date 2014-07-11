
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



Fragment::Fragment( const dsstring& p_name, DrawSpace::Planet::Body* p_planetbody, DrawSpace::Dynamics::Collider* p_collider, dsreal p_planetray, bool p_collisions ) :
m_planetbody( p_planetbody ), 
m_collider( p_collider ),
m_suspend_update( false ),
m_collision_state( false ),
m_planetray( p_planetray ),
m_hot( false ),
m_camera( NULL ),
m_inertbody( NULL ),
m_collisions( p_collisions )
{
    m_name = p_name;

    if( m_collisions )
    {

        DrawSpace::Core::Mediator* mediator = Mediator::GetInstance();

        m_runner = _DRAWSPACE_NEW_( Runner, Runner );

        m_planet_evt_cb = _DRAWSPACE_NEW_( PlanetEvtCb, PlanetEvtCb( this, &Fragment::on_planet_event ) );
        m_planetbody->RegisterEventHandler( m_planet_evt_cb );

        m_runner_evt_cb = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &Fragment::on_meshebuild_request ) );
       
        dsstring reqevtname = p_name + dsstring( "_ReqBuildMesheEvent" );
        dsstring doneevtname = p_name + dsstring( "_DoneBuildMesheEvent" );

        m_buildmeshe_event = mediator->CreateEvent( reqevtname );
        
        m_buildmeshe_event->args->AddProp<Meshe*>( "patchmeshe" );
        m_buildmeshe_event->args->AddProp<dsreal>( "sidelength" );
        m_buildmeshe_event->args->AddProp<dsreal>( "xpos" );
        m_buildmeshe_event->args->AddProp<dsreal>( "ypos" );
        m_buildmeshe_event->args->AddProp<int>( "orientation" );

        m_runner->RegisterEventHandler( m_buildmeshe_event, m_runner_evt_cb );

        m_task = _DRAWSPACE_NEW_( Task<Runner>, Task<Runner> );
        m_task->Startup( m_runner );

    }
}

Fragment::~Fragment( void )
{
    _DRAWSPACE_DELETE_( m_runner );
    _DRAWSPACE_DELETE_( m_task );
}

void Fragment::SetHotState( bool p_hotstate )
{
    m_hot = p_hotstate;
}

DrawSpace::Planet::Body* Fragment::GetPlanetBody( void )
{
    return m_planetbody;
}

void Fragment::SetCamera( DrawSpace::Dynamics::CameraPoint* p_camera )
{
    m_camera = p_camera;
}

void Fragment::SetInertBody( DrawSpace::Dynamics::InertBody* p_body )
{
    m_inertbody = p_body;
}

void Fragment::on_planet_event( DrawSpace::Planet::Body* p_body, int p_currentface )
{
    if( !m_collisions )
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
                m_collider->RemoveFromWorld();
                m_collider->UnsetKinematic();
            }

            m_suspend_update = true;

            m_meshe_ready_mutex.WaitInfinite();
            m_meshe_ready = false;
            m_meshe_ready_mutex.Release();


            m_buildmeshe_event->args->SetPropValue<Meshe*>( "patchmeshe", p_body->GetPatcheMeshe() );
            m_buildmeshe_event->args->SetPropValue<dsreal>( "sidelength", curr_patch->GetSideLength() / m_planetray );
            m_buildmeshe_event->args->SetPropValue<dsreal>( "xpos", xpos / m_planetray );
            m_buildmeshe_event->args->SetPropValue<dsreal>( "ypos", ypos / m_planetray );
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
                m_collider->RemoveFromWorld();
                m_collider->UnsetKinematic();
            }
            m_collision_state = false;
        }
    }
}

void Fragment::RemoveColliderFromWorld( void )
{
    if( m_collision_state )
    {
        if( !m_suspend_update )
        {
            m_collider->RemoveFromWorld();
            m_collider->UnsetKinematic();
        }
        m_collision_state = false;
    }
}


void Fragment::on_meshebuild_request( PropertyPool* p_args )
{
    if( !m_collisions )
    {
        return;
    }

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

    m_collider->SetKinematic( params );

    ////////////////////////////////////////////

    m_meshe_ready_mutex.WaitInfinite();
    m_meshe_ready = true;
    m_meshe_ready_mutex.Release();

    Sleep( 25 );

}


void Fragment::build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, int p_patch_orientation, dsreal p_sidelength, dsreal p_xpos, dsreal p_ypos, DrawSpace::Core::Meshe& p_outmeshe )
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

        v3.x = v2.x * m_planetray;
        v3.y = v2.y * m_planetray;
        v3.z = v2.z * m_planetray;

        p_outmeshe.AddVertex( v3 );
    }

    for( long i = 0; i < p_patchmeshe.GetTrianglesListSize(); i++ )
    {
        Triangle t;
        p_patchmeshe.GetTriangles( i, t );
        p_outmeshe.AddTriangle( t );
    }
}

void Fragment::Update( MyPlanet* p_owner )
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
                m_collider->AddToWorld();
                m_collision_state = true;
                m_suspend_update = false;                
            }
        }
    }

    if( m_hot )
    {

        Matrix camera_pos;
        bool inject_hotpoint = false;

        if( m_camera )
        {
            dsstring camera_name;

            m_camera->GetName( camera_name );
            p_owner->GetCameraHotpoint( camera_name, camera_pos );
            
            inject_hotpoint = true;
        }
        else if( m_inertbody )
        {
            m_inertbody->GetLastLocalWorldTrans( camera_pos );
            inject_hotpoint = true;
        }

        if( inject_hotpoint )
        {
            DrawSpace::Utils::Vector hotpoint;

            hotpoint[0] = camera_pos( 3, 0 );
            hotpoint[1] = camera_pos( 3, 1 );
            hotpoint[2] = camera_pos( 3, 2 );

            m_planetbody->UpdateHotPoint( hotpoint );
            m_planetbody->Compute();

            /*
            if( m_camera )
            {
                CameraPoint::Infos cam_infos;
                m_camera->GetInfos( cam_infos );
                m_camera->SetRelativeAltitude( m_planetbody->GetAltitud() );
            }
            */
        }
    }    
}


CameraPoint* Fragment::GetCamera( void )
{
    return m_camera;
}

InertBody* Fragment::GetInertBody( void )
{
    return m_inertbody;
}



MyPlanet::MyPlanet( const dsstring& p_name, dsreal p_ray ) : 
m_name( p_name ),
m_ray( /*12000000.0*/ /*600000.0*/ p_ray * 1000.0 )
{

    m_world.Initialize();

       
    m_drawable = _DRAWSPACE_NEW_( DrawSpace::Planet::Drawing, DrawSpace::Planet::Drawing );
    m_drawable->SetName( p_name );
    m_drawable->SetRenderer( DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface );
    
    m_orbiter = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Orbiter, DrawSpace::Dynamics::Orbiter( &m_world, m_drawable ) );



    m_camera_evt_cb = _DRAWSPACE_NEW_( CameraEvtCb, CameraEvtCb( this, &MyPlanet::on_camera_event ) );
}

MyPlanet::~MyPlanet( void )
{
    _DRAWSPACE_DELETE_( m_orbiter );
    _DRAWSPACE_DELETE_( m_drawable );
    
}

MyPlanet::CameraEvtCb* MyPlanet::GetCameraEvtCb( void )
{
    return m_camera_evt_cb;
}

DrawSpace::Planet::Drawing* MyPlanet::GetDrawable( void )
{
    return m_drawable;
}



DrawSpace::Dynamics::Orbiter* MyPlanet::GetOrbiter( void )
{
    return m_orbiter;
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

void MyPlanet::ApplyGravity( void )
{
    for( std::map<DrawSpace::Dynamics::InertBody*, RegisteredBody>::iterator it = m_registered_bodies.begin(); it != m_registered_bodies.end(); ++it )
    {
        if( it->second.attached )
        {
            DrawSpace::Utils::Matrix local_pos;
            it->second.body->GetLastLocalWorldTrans( local_pos );

            Vector gravity;

            gravity[0] = -local_pos( 3, 0 );
            gravity[1] = -local_pos( 3, 1 );
            gravity[2] = -local_pos( 3, 2 );
            gravity[3] = 1.0;
            gravity.Normalize();
            gravity.Scale( SHIP_MASS * 9.81 );

            it->second.body->ApplyForce( gravity );
        }
    }
}

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

            for( std::map<dsstring, RegisteredCamera>::iterator it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
            {
                if( it->second.camera == p_node )
                {
                    Fragment* fragment = it->second.fragment;
                    m_drawable->SetCurrentPlanetBody( fragment->GetPlanetBody() );
                    
                    break;
                }
            }

        }
        else
        {
            // camera non enregistree
            m_current_camerapoint = "";
        }
    }
}


void MyPlanet::GetCameraHotpoint( const dsstring& p_name, DrawSpace::Utils::Matrix& p_outmat )
{
    if( m_registered_camerapoints.count( p_name ) == 0 )
    {
        return;
    }

    if( INERTBODY_LINKED == m_registered_camerapoints[p_name].type )
    {
        m_registered_camerapoints[p_name].attached_body->GetLastLocalWorldTrans( p_outmat );
    }
    else if( FREE_ON_PLANET == m_registered_camerapoints[p_name].type )
    {
        m_registered_camerapoints[p_name].camera->GetLocalTransform( p_outmat );
    }
}

void MyPlanet::Update( void )
{
    for( size_t i = 0; i < m_planetfragments_list.size(); i++ )
    {
        Fragment* curr = m_planetfragments_list[i];

        curr->Update( this );

        InertBody* inertbody = curr->GetInertBody();
        CameraPoint* camera = curr->GetCamera();

        if( inertbody )
        {
            if( inertbody->GetRefBody() == m_orbiter )
            {
                std::vector<dsstring> cameras;
                body_find_attached_camera( inertbody, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeAltitude( curr->GetPlanetBody()->GetAltitud() );
                }
            }
        }
        else if( camera )
        {
            CameraPoint::Infos cam_infos;
            camera->GetInfos( cam_infos );
            camera->SetRelativeAltitude( curr->GetPlanetBody()->GetAltitud() );
        }       
    }
}

void MyPlanet::RegisterInertBody( const dsstring& p_bodyname, DrawSpace::Dynamics::InertBody* p_body )
{
    RegisteredBody reg_body;

    reg_body.attached = false;
    reg_body.body = p_body;

    DrawSpace::Planet::Body* planet_body = _DRAWSPACE_NEW_( DrawSpace::Planet::Body, DrawSpace::Planet::Body( m_ray * 2.0 ) );
    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( &m_world ) );

    dsstring final_name = m_name + dsstring( " " ) + p_bodyname;
    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, planet_body, collider, m_ray, true ) );
    planet_fragment->SetHotState( false );

    m_planetfragments_list.push_back( planet_fragment );
    reg_body.fragment = planet_fragment;

    planet_fragment->SetInertBody( p_body );

    planet_body->Initialize();

    m_registered_bodies[p_body] = reg_body;
}

void MyPlanet::RegisterIncludedInertBody( const dsstring& p_bodyname, DrawSpace::Dynamics::InertBody* p_body, const DrawSpace::Utils::Matrix& p_initmat )
{
    RegisteredBody reg_body;

    reg_body.attached = true;
    reg_body.body = p_body;

    p_body->IncludeTo( m_orbiter, p_initmat );

    DrawSpace::Planet::Body* planet_body = _DRAWSPACE_NEW_( DrawSpace::Planet::Body, DrawSpace::Planet::Body( m_ray * 2.0 ) );
    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( &m_world ) );

    dsstring final_name = m_name + dsstring( " " ) + p_bodyname;
    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, planet_body, collider, m_ray, true ) );
    planet_fragment->SetHotState( true );

    m_planetfragments_list.push_back( planet_fragment );
    reg_body.fragment = planet_fragment;


    planet_fragment->SetInertBody( p_body );

    planet_body->Initialize();

    m_registered_bodies[p_body] = reg_body;
}

bool MyPlanet::RegisterCameraPoint( DrawSpace::Dynamics::CameraPoint* p_camera, bool p_update_meshe )
{   
    RegisteredCamera reg_camera;

    dsstring camera_name;
    p_camera->GetName( camera_name );

    bool collisions = false;

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
                reg_camera.fragment = m_registered_bodies[inert_body].fragment;

                collisions = true;
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


                    DrawSpace::Planet::Body* planet_body = _DRAWSPACE_NEW_( DrawSpace::Planet::Body, DrawSpace::Planet::Body( m_ray * 2.0 ) );
                    Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( &m_world ) );

                    dsstring final_name = m_name + dsstring( " " ) + camera_name;
                    Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, planet_body, collider, m_ray, collisions ) );

                    planet_body->Initialize();

                    planet_fragment->SetHotState( true );
                    planet_fragment->SetCamera( p_camera );

                    reg_camera.fragment = planet_fragment;

                    reg_camera.camera->SetRelativeOrbiter( m_orbiter );

                    m_planetfragments_list.push_back( planet_fragment );
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


        DrawSpace::Planet::Body* planet_body = _DRAWSPACE_NEW_( DrawSpace::Planet::Body, DrawSpace::Planet::Body( m_ray * 2.0 ) );
        Collider* collider = _DRAWSPACE_NEW_( Collider, Collider( &m_world ) );

        dsstring final_name = m_name + dsstring( " " ) + camera_name;
        Fragment* planet_fragment = _DRAWSPACE_NEW_( Fragment, Fragment( final_name, planet_body, collider, m_ray, collisions ) );

        planet_body->Initialize();

        planet_fragment->SetHotState( false );
        planet_fragment->SetCamera( p_camera );

        reg_camera.fragment = planet_fragment;

        m_planetfragments_list.push_back( planet_fragment );

    }

    ////

    m_registered_camerapoints[camera_name] = reg_camera;

    return true;
}

void MyPlanet::body_find_attached_camera( DrawSpace::Dynamics::InertBody* p_body, /*dsstring& p_name*/ std::vector<dsstring>& p_name )
{
    for( std::map<dsstring, RegisteredCamera>::iterator it = m_registered_camerapoints.begin(); it != m_registered_camerapoints.end(); ++it )
    {
        if( it->second.camera->GetAttachedBody() == p_body )
        {
            p_name.push_back( it->first );            
        }
    }    
}

void MyPlanet::ManageBodies( void )
{
    for( std::map<DrawSpace::Dynamics::InertBody*, RegisteredBody>::iterator it = m_registered_bodies.begin(); it != m_registered_bodies.end(); ++it )
    {
        if( it->second.attached )
        {
            DrawSpace::Utils::Matrix bodypos;

            it->second.body->GetLastLocalWorldTrans( bodypos );

            DrawSpace::Utils::Vector bodypos2;
            bodypos2[0] = bodypos( 3, 0 );
            bodypos2[1] = bodypos( 3, 1 );
            bodypos2[2] = bodypos( 3, 2 );

            dsreal rel_alt = ( bodypos2.Length() / m_ray );

            if( rel_alt >= 1.2 )
            {

                detach_body( it->second.body );

                // rechercher si une camera enregistree est associee a ce body
                std::vector<dsstring> cameras;
                body_find_attached_camera( it->second.body, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeOrbiter( NULL );

                    Fragment* fragment = m_registered_camerapoints[cameras[i]].fragment;
                    fragment->SetHotState( false );

                }

                //////

                DrawSpace::Core::TransformNode* node;                
                node = it->second.body->GetDrawable();


                Fragment* fragment = it->second.fragment;
                fragment->RemoveColliderFromWorld();

                //////
            }
        }
        else
        {
            DrawSpace::Utils::Matrix bodypos;

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

                attach_body( it->second.body );

                /////

                std::vector<dsstring> cameras;
                body_find_attached_camera( it->second.body, cameras );

                for( size_t i = 0; i < cameras.size(); i++ )
                {
                    m_registered_camerapoints[cameras[i]].camera->SetRelativeOrbiter( m_orbiter );

                    Fragment* fragment = m_registered_camerapoints[cameras[i]].fragment;

                    fragment->SetHotState( true );
                }
            }
        }
    }
}

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


    


    compute_player_view_transform();
    m_camera2->SetLocalTransform( m_player_view_transform );


    Matrix id;
    id.Identity();
    m_camera5->SetLocalTransform( id );
    
    Matrix origin;
    origin.Identity();

    m_orbit->OrbitStep( origin );


    m_planet->ApplyGravity();
    m_moon->ApplyGravity();

    m_ship->Update();
    m_cube_body->Update();



           
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


    //renderer->DrawText( 0, 255, 0, 10, 195, "planet01 : %d %d", m_planet->GetCollisionState(), m_planet->GetCollisionMesheBuildCount() );
    //renderer->DrawText( 0, 255, 0, 10, 215, "moon : %d %d", m_moon->GetCollisionState(), m_moon->GetCollisionMesheBuildCount() );


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



    renderer->FlipScreen();

    m_calendar->Run();

    

    m_freemove.SetSpeed( m_speed );
}

bool dsAppClient::OnIdleAppInit( void )
{
    //World::m_scale = 0.5;
    World::m_scale = 1.0;
    DrawSpace::Planet::Body::BuildPlanetMeshe();


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



    m_chunk = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_chunk->RegisterPassSlot( "texture_pass" );
    m_chunk->SetRenderer( renderer );

    m_chunk->SetName( "box" );
    
    m_chunk->GetMeshe()->SetImporter( m_meshe_import );

    m_chunk->GetMeshe()->LoadFromFile( "object.ac", 0 );    

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


    m_planet = _DRAWSPACE_NEW_( MyPlanet, MyPlanet( "planet01", 400.0 ) );


    m_planet->GetDrawable()->RegisterPassSlot( "texture_pass" );

    for( long i = 0; i < 6; i++ )
    {
        m_planet->GetDrawable()->SetNodeFromPassSpecificFx( "texture_pass", i, "main_fx" );
    }
    m_scenegraph.RegisterNode( m_planet->GetDrawable() );

    //m_planet->GetDrawable()->Initialize();
    //m_planet->GetPlanetBody()->Initialize();

    

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

    //m_moon->GetPlanetBody()->Initialize();

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


    //DrawSpace::Dynamics::Body::Parameters cube_params;
    cube_params.mass = SHIP_MASS;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 2.0, 0.5, 4.0, 1.0 );

    cube_params.initial_attitude.Translation( 265000000.0, 0.0, 0.0 );

    m_ship = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Rocket, DrawSpace::Dynamics::Rocket( &m_world, m_ship_drawable, cube_params ) );


    //////////////////////////////////////////////////////////////


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
    m_camera4->LockOnBody( /*m_planet->GetOrbiter()*/ m_cube_body );


    m_camera5 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint( "camera5", m_planet->GetOrbiter() ) );
    m_scenegraph.RegisterNode( m_camera5 );

    m_longlat_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt->Init( -21.0, 20.0, 400000.5, 0.0, 0.0 );
    m_camera5->RegisterLongLatMovement( m_longlat_mvt );

    m_circular_mvt2 = _DRAWSPACE_NEW_( DrawSpace::Core::CircularMovement, DrawSpace::Core::CircularMovement );
    m_circular_mvt2->Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( 25.0, 0.9, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 0.0, 0.0, 0.0 );
    m_camera5->RegisterMovement( m_circular_mvt2 );
    m_circular_mvt2->SetAngularSpeed( 25.0 );
    




    ///////////////////////////////////////////////////////////////


    m_reticle_widget = _DRAWSPACE_NEW_( ReticleWidget, ReticleWidget( "reticle_widget", (long)10, (long)10, &m_scenegraph, NULL ) );
    



    m_text_widget = _DRAWSPACE_NEW_( TextWidget, TextWidget( "text_widget", DRAWSPACE_GUI_WIDTH, DRAWSPACE_GUI_HEIGHT, m_font, true, m_reticle_widget ) );

    m_text_widget->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_text_widget->GetBackgroundImage()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );


    m_text_widget->GetBackgroundImage()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_text_widget->GetBackgroundImage()->GetFx()->GetShader( 1 )->LoadFromFile();

    m_text_widget->GetBackgroundImage()->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "reticle.bmp" ) ), 0 );
    m_text_widget->GetBackgroundImage()->GetTexture( 0 )->LoadFromFile();


  
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














    m_reticle_widget->LockOnBody( /*m_moon->GetOrbiter()*/ m_cube_body );
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


    m_scenegraph.SetCurrentCamera( "camera5" );

    m_curr_camera = m_camera5;


    

    m_planet->RegisterInertBody( "ship", m_ship );
    m_planet->RegisterCameraPoint( m_camera2, true );
    m_planet->RegisterCameraPoint( m_camera3, true );
    m_planet->RegisterCameraPoint( m_camera4, true );
    m_planet->RegisterCameraPoint( m_camera5, /*false*/ true );
    m_planet->RegisterCameraPoint( m_camera, false );


    m_moon->RegisterInertBody( "ship", m_ship );
    m_moon->RegisterCameraPoint( m_camera2, true );
    m_moon->RegisterCameraPoint( m_camera3, true );
    m_moon->RegisterCameraPoint( m_camera4, true );
    m_moon->RegisterCameraPoint( m_camera, /*false*/ true );
    


    m_scenegraph.RegisterCameraEvtHandler( m_planet->GetCameraEvtCb() );
    m_scenegraph.RegisterCameraEvtHandler( m_moon->GetCameraEvtCb() );

    ///////////////////////////////////////////////////////////////



    //m_camera5->LockOnBody( m_moon->GetOrbiter() );
    //m_camera5->LockOnBody( m_ship );
    m_camera5->LockOnBody( m_cube_body );



    m_longlat_mvt2 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt2->Init( -21.0000, 20.000, 400002.5, 0.0, 0.0 );
    m_longlat_mvt2->Compute( m_timer );
    Matrix llres;
    m_longlat_mvt2->GetResult( llres );

    m_planet->RegisterIncludedInertBody( "simple_cube", m_cube_body, llres );


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

        case 'T':

            m_ship->ApplyFwdForce( 510000.0 );
            break;



        case VK_SPACE:

            m_ship->ZeroSpeed();
            break;

        case VK_RETURN:

            m_ship->ApplyFwdForce( 11000.0 );
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
                m_scenegraph.SetCurrentCamera( "camera5" );
                m_curr_camera = m_camera5;
            }
            else if( m_curr_camera == m_camera5 )
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
