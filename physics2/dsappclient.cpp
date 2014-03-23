
#include "dsappclient.h"



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;


dsAppClient* dsAppClient::m_instance = NULL;

_DECLARE_DS_LOGGER( logger, "AppClient" )


dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_box_count( 0 ), m_box_texture( 0 ), m_attached( false )
{    
    _INIT_LOGGER( "physics2.conf" )  
    m_w_title = "physics(2) test";

    m_myWorld = NULL;

    m_fwd[0] = 0.0;
    m_fwd[1] = 0.0;
    m_fwd[2] = -1.0;

    m_right[0] = 1.0;
    m_right[1] = 0.0;
    m_right[2] = 0.0;

    m_left[0] = -1.0;
    m_left[1] = 0.0;
    m_left[2] = 0.0;


    m_up[0] = 0.0;
    m_up[1] = 1.0;
    m_up[2] = 0.0;

    m_down[0] = 0.0;
    m_down[1] = -1.0;
    m_down[2] = 0.0;


    m_transformed_fwd = m_fwd;
    m_transformed_left = m_left;
    m_transformed_right = m_right;
    m_transformed_up = m_up;
    m_transformed_down = m_down;
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{

    m_fpsmove.Compute( m_timer, true );

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    DrawSpace::Utils::Matrix sbtrans;

    sbtrans.Scale( 20.0, 20.0, 20.0 );
    m_scenegraph.SetNodeLocalTransformation( "spacebox", sbtrans );



    DrawSpace::Utils::Matrix box_rot;

    for( long i = 0; i < m_boxes.size(); i++ )
    {

        if( false == m_boxes[i].kinematic )
        {
            btTransform tf;
            m_boxes[i].motion->getWorldTransform( tf );
            tf.getOpenGLMatrix( m_matrix );
            

            DrawSpace::Utils::Matrix box_pos;

            box_pos( 0, 0 ) = m_matrix[0];
            box_pos( 0, 1 ) = m_matrix[1];
            box_pos( 0, 2 ) = m_matrix[2];
            box_pos( 0, 3 ) = m_matrix[3];

            box_pos( 1, 0 ) = m_matrix[4];
            box_pos( 1, 1 ) = m_matrix[5];
            box_pos( 1, 2 ) = m_matrix[6];
            box_pos( 1, 3 ) = m_matrix[7];

            box_pos( 2, 0 ) = m_matrix[8];
            box_pos( 2, 1 ) = m_matrix[9];
            box_pos( 2, 2 ) = m_matrix[10];
            box_pos( 2, 3 ) = m_matrix[11];

            box_pos( 3, 0 ) = m_matrix[12];
            box_pos( 3, 1 ) = m_matrix[13];
            box_pos( 3, 2 ) = m_matrix[14];
            box_pos( 3, 3 ) = m_matrix[15];


            if( 1 == i )
            {

                if( !m_attached )
                {
                    m_boxes[i].drawable->SetLocalTransform( box_pos );
                }
                else
                {
                    Matrix res = box_pos * m_boxes[0].mat_b;

                    m_boxes[i].drawable->SetLocalTransform( res );
                }


                box_rot = box_pos;

                box_rot.ClearTranslation();
                box_rot.Transform( &m_fwd, &m_transformed_fwd );
                box_rot.Transform( &m_left, &m_transformed_left );
                box_rot.Transform( &m_right, &m_transformed_right );
                box_rot.Transform( &m_up, &m_transformed_up );
                box_rot.Transform( &m_down, &m_transformed_down );

            }
        }

        else
        {
            

            Matrix object_transform;

            Matrix pos;
            Matrix scale;
            Matrix rot;

            pos.Translation( m_boxes[i].k_pos );
            rot.Rotation( Vector( 0.0, 1.0, 0.0, 1.0 ), Maths::DegToRad( m_boxes[i].k_angle ) );

            DrawSpace::Utils::Transformation trf;
            DrawSpace::Utils::Matrix res;
           
            trf.PushMatrix( pos );
            trf.PushMatrix( rot );
            trf.BuildResult();
            trf.GetResult( &res );

            m_boxes[i].drawable->SetLocalTransform( res );


            if( i == 0 /*&& m_attached*/ )
            {
                m_boxes[i].mat_b = res;
            }



            /////////////////////////////////////////////////////////

            
            if( !m_attached )
            {


                /*
                btScalar kmat[16];    
                btTransform ktf;

                kmat[0] = res( 0, 0 );
                kmat[1] = res( 0, 1 );
                kmat[2] = res( 0, 2 );
                kmat[3] = res( 0, 3 );

                kmat[4] = res( 1, 0 );
                kmat[5] = res( 1, 1 );
                kmat[6] = res( 1, 2 );
                kmat[7] = res( 1, 3 );

                kmat[8] = res( 2, 0 );
                kmat[9] = res( 2, 1 );
                kmat[10] = res( 2, 2 );
                kmat[11] = res( 2, 3 );

                kmat[12] = res( 3, 0 );
                kmat[13] = res( 3, 1 );
                kmat[14] = res( 3, 2 );
                kmat[15] = res( 3, 3 );

                ktf.setFromOpenGLMatrix( kmat );
                m_boxes[i].motion->setWorldTransform( ktf ); 
                */
                

            }
            

            /*
            else
            {

                // debug only : visualize transform content
                btTransform tf;
                m_boxes[i].motion->getWorldTransform( tf );
                tf.getOpenGLMatrix( m_matrix );

                _asm nop
            }
            */

            
        }
    }




    /*
    DrawSpace::Utils::Matrix cam2_pos;
    cam2_pos.Translation( 0.0, 0.0, 2.0 );
    m_scenegraph.SetNodeLocalTransformation( "camera2", cam2_pos );
    */


 
    m_scenegraph.ComputeTransformations();    

    static long last_fps;

    long current_fps = m_timer.GetFPS();
    char fps[256];
    sprintf( fps, "%d fps", m_timer.GetFPS() );
    
    if( last_fps != current_fps )
    {
        m_fpstext_widget->SetText( 0, 0, 70, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
        last_fps = current_fps;		
    }
    


    m_fpstext_widget->SetVirtualTranslation( 10, 5 );
    m_fpstext_widget->Transform();

    m_texturepass->GetRenderingQueue()->Draw();


    m_fpstext_widget->Draw();

    m_finalpass->GetRenderingQueue()->Draw();


    renderer->DrawText( 255, 0, 0, 30, 50, "%d %d", m_texturepass->GetRenderingQueue()->GetSwitchesCost(), m_texturepass->GetRenderingQueue()->GetTheoricalSwitchesCost() );
    

    renderer->FlipScreen();

    m_timer.Update();
    if( m_timer.IsReady() )
    {
        if( m_myWorld && m_myWorld_2 )
        {

	        m_myWorld->stepSimulation( 1.0 / (dsreal)( m_timer.GetFPS() ) );

            m_myWorld_2->stepSimulation( 1.0 / (dsreal)( m_timer.GetFPS() ) );


            for( long i = 0; i < m_boxes.size(); i++ )
            {
                if( m_boxes[i].kinematic )
                {
                    m_timer.AngleSpeedInc( &m_boxes[i].k_angle, 5.0 );
                }
            }
        }
    }
}

void dsAppClient::create_box( const dsstring& p_meshe, dsreal p_dims, const DrawSpace::Utils::Vector& p_pos, bool p_kinematic, bool p_world2 )
{
    DrawSpace::Interface::Drawable* drawable;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    drawable = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    drawable->RegisterPassSlot( "texture_pass" );
    drawable->SetRenderer( renderer );

    char name[32];
    sprintf( name, "box_%d", m_box_count );

    drawable->SetName( name );
    
    drawable->GetMeshe( "" )->SetImporter( m_meshe_import );

    drawable->GetMeshe( "" )->LoadFromFile( p_meshe, 0 );    

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    drawable->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    if( 0 == m_box_texture )
    {
        drawable->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "shelby.jpg" ) ), 0 );
        m_box_texture = 1;
    }
    else
    {
        drawable->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );
        m_box_texture = 0;
    }


    drawable->GetNodeFromPass( "texture_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( drawable );


    ///////////////////////////////////////////////////////////////////

    btRigidBody* body = NULL;
    btDefaultMotionState* motion = NULL;

    m_myTransform.setIdentity();

    if( p_kinematic )
    {
        m_myTransform.setOrigin( btVector3( 0.0, 0.0, 0.0 ) );
    }
    else
    {
        m_myTransform.setOrigin( btVector3( p_pos[0], p_pos[1], p_pos[2] ) );
    }

    btCollisionShape* shape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( p_dims / 2.0, p_dims / 2.0, p_dims / 2.0 ) ) );
    motion = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( m_myTransform ) );


    btVector3 localInertia( 0, 0, 0 );

    btScalar mass = 8.5;
    shape->calculateLocalInertia( mass, localInertia );

    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo( mass, motion, shape, localInertia );

    body = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( myBoxRigidBodyConstructionInfo ) );

    
    if( p_kinematic )
    {
        body->setCollisionFlags( body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
        
    }
    
    if( p_world2 )
    {
        m_myWorld_2->addRigidBody( body );
    }
    else
    {
        m_myWorld->addRigidBody( body );
    }

    body->setActivationState( DISABLE_DEACTIVATION );


    Box box;

    box.drawable = drawable;
    box.body = body;
    box.motion = motion;

    box.kinematic = p_kinematic;

    if( p_kinematic )
    {
        box.k_angle = 0.0;
        box.k_pos = p_pos;
    }
 

    m_boxes.push_back( box );

    m_box_count++;

    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
}

bool dsAppClient::OnIdleAppInit( void )
{
    bool status;

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0 );
    /*
    */



    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
    


    //////////////////////////////////////////////////////////////

    m_scenegraph.RegisterPass( m_texturepass );

    status = DrawSpace::Utils::LoadDrawablePlugin( "chunk.dll", "chunk_plugin" );

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );



    ////////////////////////////////////////////////////////////////

    status = DrawSpace::Utils::LoadDrawablePlugin( "spacebox.dll", "spacebox_plugin" );

    m_spacebox = DrawSpace::Utils::InstanciateDrawableFromPlugin( "spacebox_plugin" );
    m_spacebox->RegisterPassSlot( "texture_pass" );
    m_spacebox->SetRenderer( renderer );
    m_spacebox->SetName( "spacebox" );


    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "front" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "rear" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "top" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "bottom" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "left" );
    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, "texture_pass", "right" );

    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb0.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb2.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb4.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb3.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->GetTexture( 0 )->LoadFromFile();

    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "sb1.bmp" ) ), 0 );
    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( "texture_pass", "front" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "rear" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "top" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "bottom" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "left" )->SetOrderNumber( 200 );
    m_spacebox->GetNodeFromPass( "texture_pass", "right" )->SetOrderNumber( 200 );



    m_scenegraph.RegisterNode( m_spacebox );




    //////////////////////////////////////////////////////////////


    status = DrawSpace::Utils::LoadFontImportPlugin( "cbfgfont.dll", "cbfgfont_plugin" );
    m_font_import = DrawSpace::Utils::InstanciateFontImportFromPlugin( "cbfgfont_plugin" );

    m_font = _DRAWSPACE_NEW_( Font, Font );

    m_font->SetImporter( m_font_import );

    status = m_font->Build( "mangalfont.bmp", "mangalfont.csv" );
    if( !status )
    {
        return false;
    }

    m_fpstext_widget = DrawSpace::Utils::BuildText( m_font, 15, 10, DrawSpace::Utils::Vector( 1.0, 1.0, 1.0, 0.0 ), "fps" );

    m_fpstext_widget->GetImage()->SetOrderNumber( 20000 );

    m_fpstext_widget->RegisterToPass( m_finalpass );



    //////////////////////////////////////////////////////////////

    m_camera = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera" ) );
    m_scenegraph.RegisterNode( m_camera );

    

    m_fpsmove.SetTransformNode( m_camera );
    m_fpsmove.Init( DrawSpace::Utils::Vector( 0.0, 0.75, 12.0, 1.0 ) );

    m_mouse_circularmode = true;


    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Camera, DrawSpace::Camera( "camera2" ) );
    m_scenegraph.RegisterNode( m_camera2 );


    //m_box->AddChild( m_camera2 );

    //m_scenegraph.SetCurrentCamera( "camera2" );
    //m_scenegraph.SetCurrentCamera( "camera" );


    
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();


    //////////////////////////////////////////////////////////////

    m_myCollisionConfiguration = _DRAWSPACE_NEW_( btDefaultCollisionConfiguration, btDefaultCollisionConfiguration );
    m_myDispatcher = _DRAWSPACE_NEW_( btCollisionDispatcher, btCollisionDispatcher( m_myCollisionConfiguration ) );
    m_myBroadphase = _DRAWSPACE_NEW_( btDbvtBroadphase, btDbvtBroadphase );
    m_mySequentialImpulseConstraintSolver = _DRAWSPACE_NEW_( btSequentialImpulseConstraintSolver, btSequentialImpulseConstraintSolver );
    m_myWorld = _DRAWSPACE_NEW_( btDiscreteDynamicsWorld, btDiscreteDynamicsWorld( m_myDispatcher, m_myBroadphase, m_mySequentialImpulseConstraintSolver, m_myCollisionConfiguration ) ); 



    m_myCollisionConfiguration_2 = _DRAWSPACE_NEW_( btDefaultCollisionConfiguration, btDefaultCollisionConfiguration );
    m_myDispatcher_2 = _DRAWSPACE_NEW_( btCollisionDispatcher, btCollisionDispatcher( m_myCollisionConfiguration_2 ) );
    m_myBroadphase_2 = _DRAWSPACE_NEW_( btDbvtBroadphase, btDbvtBroadphase );
    m_mySequentialImpulseConstraintSolver_2 = _DRAWSPACE_NEW_( btSequentialImpulseConstraintSolver, btSequentialImpulseConstraintSolver );
    m_myWorld_2 = _DRAWSPACE_NEW_( btDiscreteDynamicsWorld, btDiscreteDynamicsWorld( m_myDispatcher_2, m_myBroadphase_2, m_mySequentialImpulseConstraintSolver_2, m_myCollisionConfiguration_2 ) ); 





    //m_myWorld->setGravity( btVector3( 0, -9.81, 0 ) );

    m_myWorld->setGravity( btVector3( 0, 0.0, 0 ) );
    m_myWorld_2->setGravity( btVector3( 0, 0.0, 0 ) );


    create_box( "object2.ac", 10.0, Vector( 20.0, 0.0, -10.0, 1.0 ), true, true );

    create_box( "object.ac", 1.0, Vector( 0.0, 0.0, 0.0, 1.0 ), false, false );


    //m_boxes[1].drawable->AddChild( m_camera );
    m_scenegraph.SetCurrentCamera( "camera" );








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

            m_fpsmove.SetSpeed( 6.0 );
            break;

        case 'W':

            m_fpsmove.SetSpeed( -6.0 );
            break;

        case 'A':
            
            m_boxes[1].body->applyTorque( btVector3( m_transformed_up[0], m_transformed_up[1], m_transformed_up[2] ) );
            break;

        case 'E':
            m_boxes[1].body->applyTorque( btVector3( m_transformed_down[0], m_transformed_down[1], m_transformed_down[2] ) );

            break;

        case 'Z':

            m_boxes[1].body->applyForce( btVector3( m_transformed_fwd[0] * 5.0, m_transformed_fwd[1] * 5.0, m_transformed_fwd[2] * 5.0 ), btVector3( 0.0, 0.0, 0.0 ) );
            break;

        case 'S':

            
            break;

        case 'X':
            break;

        case 'D':

            m_boxes[1].body->applyTorque( btVector3( m_transformed_left[0], m_transformed_left[1], m_transformed_left[2] ) );
            break;

        case 'C':

            m_boxes[1].body->applyTorque( btVector3( m_transformed_right[0], m_transformed_right[1], m_transformed_right[2] ) );
            break;
    }

}

void dsAppClient::OnEndKeyPress( long p_key )
{
    switch( p_key )
    {
        case 'Q':

            m_fpsmove.SetSpeed( 0.0 );
            break;


        case 'W':

            m_fpsmove.SetSpeed( 0.0 );
            break;

        case VK_SPACE:

            break;


        case 'A':
            
            break;


        case 'Z':
            
            break;


       case VK_F1:

            {

                // remove & recreate object 

                m_myWorld->removeRigidBody( m_boxes[1].body );

                

                _DRAWSPACE_DELETE_( m_boxes[1].motion );
                _DRAWSPACE_DELETE_( m_boxes[1].body );

                /*
                m_myTransform.setIdentity();
                m_myTransform.setOrigin( btVector3( 0, 3.0, 0 ) );
                */

                btCollisionShape* shape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( 0.5, 0.5, 0.5 ) ) );
                btDefaultMotionState* motion = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( /*m_myTransform*/ m_myTransform_mem ) );


                btVector3 localInertia( 0, 0, 0 );

                btScalar mass = 8.5;
                shape->calculateLocalInertia( mass, localInertia );

                btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo( mass, motion, shape, localInertia );

                btRigidBody* body = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( myBoxRigidBodyConstructionInfo ) );



                m_myWorld->addRigidBody( body );

                body->setActivationState( DISABLE_DEACTIVATION );

                m_boxes[1].motion = motion;
                m_boxes[1].body = body;

                body->setAngularVelocity( m_angularspeed_mem );
                body->setLinearVelocity( m_linearspeed_mem );


            }
            break;


        case VK_F2:

            m_boxes[1].motion->getWorldTransform( m_myTransform_mem );

            m_linearspeed_mem = m_boxes[1].body->getLinearVelocity();
            m_angularspeed_mem = m_boxes[1].body->getAngularVelocity();

            break;


        case VK_F3:
            {
                // attach

                m_attached = true;



               

                btTransform tf;
                btScalar imat[16];


                m_boxes[1].motion->getWorldTransform( tf );
                tf.getOpenGLMatrix( imat );



                DrawSpace::Utils::Matrix mat_a;

                mat_a( 0, 0 ) = imat[0];
                mat_a( 0, 1 ) = imat[1];
                mat_a( 0, 2 ) = imat[2];
                mat_a( 0, 3 ) = imat[3];

                mat_a( 1, 0 ) = imat[4];
                mat_a( 1, 1 ) = imat[5];
                mat_a( 1, 2 ) = imat[6];
                mat_a( 1, 3 ) = imat[7];

                mat_a( 2, 0 ) = imat[8];
                mat_a( 2, 1 ) = imat[9];
                mat_a( 2, 2 ) = imat[10];
                mat_a( 2, 3 ) = imat[11];

                mat_a( 3, 0 ) = imat[12];
                mat_a( 3, 1 ) = imat[13];
                mat_a( 3, 2 ) = imat[14];
                mat_a( 3, 3 ) = imat[15];






                DrawSpace::Utils::Matrix mat_b = m_boxes[0].mat_b;
                mat_b.Inverse();


                DrawSpace::Utils::Matrix mat_a2 = mat_a * mat_b;




                

                btScalar kmat[16];    
                btTransform ktf;

                kmat[0] = mat_a2( 0, 0 );
                kmat[1] = mat_a2( 0, 1 );
                kmat[2] = mat_a2( 0, 2 );
                kmat[3] = mat_a2( 0, 3 );

                kmat[4] = mat_a2( 1, 0 );
                kmat[5] = mat_a2( 1, 1 );
                kmat[6] = mat_a2( 1, 2 );
                kmat[7] = mat_a2( 1, 3 );

                kmat[8] = mat_a2( 2, 0 );
                kmat[9] = mat_a2( 2, 1 );
                kmat[10] = mat_a2( 2, 2 );
                kmat[11] = mat_a2( 2, 3 );

                kmat[12] = mat_a2( 3, 0 );
                kmat[13] = mat_a2( 3, 1 );
                kmat[14] = mat_a2( 3, 2 );
                kmat[15] = mat_a2( 3, 3 );

                ktf.setFromOpenGLMatrix( kmat );
         








                

                m_linearspeed_mem = m_boxes[1].body->getLinearVelocity();
                m_angularspeed_mem = m_boxes[1].body->getAngularVelocity();










                m_myWorld->removeRigidBody( m_boxes[1].body );
                _DRAWSPACE_DELETE_( m_boxes[1].motion );
                _DRAWSPACE_DELETE_( m_boxes[1].body );




                btCollisionShape* shape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( 0.5, 0.5, 0.5 ) ) );
                btDefaultMotionState* motion = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( ktf ) );


                btVector3 localInertia( 0, 0, 0 );

                btScalar mass = 8.5;
                shape->calculateLocalInertia( mass, localInertia );

                btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo( mass, motion, shape, localInertia );

                btRigidBody* body = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( myBoxRigidBodyConstructionInfo ) );



                m_myWorld_2->addRigidBody( body );

                body->setActivationState( DISABLE_DEACTIVATION );

                m_boxes[1].motion = motion;
                m_boxes[1].body = body;


                // m_angularspeed_mem & m_linearspeed_mem a passer dans le repere planete

                Vector angularspeed_mem, linearspeed_mem;
                Vector angularspeed_mem_2, linearspeed_mem_2;
                btVector3 angularspeed_mem_3, linearspeed_mem_3;


                angularspeed_mem[0] = m_angularspeed_mem.x();
                angularspeed_mem[1] = m_angularspeed_mem.y();
                angularspeed_mem[2] = m_angularspeed_mem.z();
                angularspeed_mem[3] = 1.0;


                linearspeed_mem[0] = m_linearspeed_mem.x();
                linearspeed_mem[1] = m_linearspeed_mem.y();
                linearspeed_mem[2] = m_linearspeed_mem.z();
                linearspeed_mem[3] = 1.0;

                mat_b.ClearTranslation();

                mat_b.Transform( &angularspeed_mem, &angularspeed_mem_2 );
                mat_b.Transform( &linearspeed_mem, &linearspeed_mem_2 );
               
                body->setAngularVelocity( btVector3( angularspeed_mem_2[0], angularspeed_mem_2[1], angularspeed_mem_2[2] ) );
                body->setLinearVelocity( btVector3( linearspeed_mem_2[0], linearspeed_mem_2[1], linearspeed_mem_2[2] ) );




            }
            break;


        case VK_F4:
            {
                // detach

                m_attached = false;



                btTransform tf;
                btScalar imat[16];


                m_boxes[1].motion->getWorldTransform( tf );
                tf.getOpenGLMatrix( imat );



                DrawSpace::Utils::Matrix mat_a;

                mat_a( 0, 0 ) = imat[0];
                mat_a( 0, 1 ) = imat[1];
                mat_a( 0, 2 ) = imat[2];
                mat_a( 0, 3 ) = imat[3];

                mat_a( 1, 0 ) = imat[4];
                mat_a( 1, 1 ) = imat[5];
                mat_a( 1, 2 ) = imat[6];
                mat_a( 1, 3 ) = imat[7];

                mat_a( 2, 0 ) = imat[8];
                mat_a( 2, 1 ) = imat[9];
                mat_a( 2, 2 ) = imat[10];
                mat_a( 2, 3 ) = imat[11];

                mat_a( 3, 0 ) = imat[12];
                mat_a( 3, 1 ) = imat[13];
                mat_a( 3, 2 ) = imat[14];
                mat_a( 3, 3 ) = imat[15];



                DrawSpace::Utils::Matrix mat_b = m_boxes[0].mat_b;
                


                DrawSpace::Utils::Matrix mat_a2 = mat_a * mat_b;




                

                btScalar kmat[16];    
                btTransform ktf;

                kmat[0] = mat_a2( 0, 0 );
                kmat[1] = mat_a2( 0, 1 );
                kmat[2] = mat_a2( 0, 2 );
                kmat[3] = mat_a2( 0, 3 );

                kmat[4] = mat_a2( 1, 0 );
                kmat[5] = mat_a2( 1, 1 );
                kmat[6] = mat_a2( 1, 2 );
                kmat[7] = mat_a2( 1, 3 );

                kmat[8] = mat_a2( 2, 0 );
                kmat[9] = mat_a2( 2, 1 );
                kmat[10] = mat_a2( 2, 2 );
                kmat[11] = mat_a2( 2, 3 );

                kmat[12] = mat_a2( 3, 0 );
                kmat[13] = mat_a2( 3, 1 );
                kmat[14] = mat_a2( 3, 2 );
                kmat[15] = mat_a2( 3, 3 );

                ktf.setFromOpenGLMatrix( kmat );
         



                m_linearspeed_mem = m_boxes[1].body->getLinearVelocity();
                m_angularspeed_mem = m_boxes[1].body->getAngularVelocity();










                m_myWorld_2->removeRigidBody( m_boxes[1].body );
                _DRAWSPACE_DELETE_( m_boxes[1].motion );
                _DRAWSPACE_DELETE_( m_boxes[1].body );





                btCollisionShape* shape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( 0.5, 0.5, 0.5 ) ) );
                btDefaultMotionState* motion = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( ktf ) );


                btVector3 localInertia( 0, 0, 0 );

                btScalar mass = 8.5;
                shape->calculateLocalInertia( mass, localInertia );

                btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo( mass, motion, shape, localInertia );

                btRigidBody* body = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( myBoxRigidBodyConstructionInfo ) );



                m_myWorld->addRigidBody( body );

                body->setActivationState( DISABLE_DEACTIVATION );

                m_boxes[1].motion = motion;
                m_boxes[1].body = body;



                // m_angularspeed_mem & m_linearspeed_mem a passer dans le repere global

                Vector angularspeed_mem, linearspeed_mem;
                Vector angularspeed_mem_2, linearspeed_mem_2;
                btVector3 angularspeed_mem_3, linearspeed_mem_3;


                angularspeed_mem[0] = m_angularspeed_mem.x();
                angularspeed_mem[1] = m_angularspeed_mem.y();
                angularspeed_mem[2] = m_angularspeed_mem.z();
                angularspeed_mem[3] = 1.0;


                linearspeed_mem[0] = m_linearspeed_mem.x();
                linearspeed_mem[1] = m_linearspeed_mem.y();
                linearspeed_mem[2] = m_linearspeed_mem.z();
                linearspeed_mem[3] = 1.0;


                

                mat_b.ClearTranslation();
               

                mat_b.Transform( &angularspeed_mem, &angularspeed_mem_2 );
                mat_b.Transform( &linearspeed_mem, &linearspeed_mem_2 );
               
                body->setAngularVelocity( btVector3( angularspeed_mem_2[0], angularspeed_mem_2[1], angularspeed_mem_2[2] ) );
                body->setLinearVelocity( btVector3( linearspeed_mem_2[0], linearspeed_mem_2[1], linearspeed_mem_2[2] ) );

            }
            break;

        case VK_F5:
            {

                m_boxes[1].body->setLinearVelocity( btVector3( 0.0, 0.0, 0.0 ) );
                m_boxes[1].body->setAngularVelocity( btVector3( 0.0, 0.0, 0.0 ) );
            }
            break;

    }

}

void dsAppClient::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
    }
}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{

	m_fpsmove.RotateYaw( - p_dx / 4.0, m_timer );
	m_fpsmove.RotatePitch( - p_dy / 4.0, m_timer );

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
