
#include "dsappclient.h"



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_box_count( 0 ), m_box_texture( 0 ), m_force( false ), m_compose( false )
{    
    _INIT_LOGGER( "physics2.conf" )  
    m_w_title = "physics(2) test";

    m_myWorld = NULL;

    m_kpos[1] = 0.5;
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


    DrawSpace::Utils::Matrix ground_pos;
    ground_pos.Translation( 0.0, 0.0, 0.0 );
    m_scenegraph.SetNodeLocalTransformation( "ground", ground_pos );


    ////////////////////////////////////////////////////

    Matrix kmat;
    kmat.Translation( m_kpos );

    btScalar kmat2[16];    
    btTransform ktf;

    kmat2[0] = kmat( 0, 0 );
    kmat2[1] = kmat( 0, 1 );
    kmat2[2] = kmat( 0, 2 );
    kmat2[3] = kmat( 0, 3 );

    kmat2[4] = kmat( 1, 0 );
    kmat2[5] = kmat( 1, 1 );
    kmat2[6] = kmat( 1, 2 );
    kmat2[7] = kmat( 1, 3 );

    kmat2[8] = kmat( 2, 0 );
    kmat2[9] = kmat( 2, 1 );
    kmat2[10] = kmat( 2, 2 );
    kmat2[11] = kmat( 2, 3 );

    kmat2[12] = kmat( 3, 0 );
    kmat2[13] = kmat( 3, 1 );
    kmat2[14] = kmat( 3, 2 );
    kmat2[15] = kmat( 3, 3 );


    ktf.setFromOpenGLMatrix( kmat2 );

    m_boxes[0].motion->setWorldTransform( ktf );



    ////////////////////////////////////////////////////

    for( long i = 0; i < m_boxes.size(); i++ )
    {
        
        btTransform tf;
        m_boxes[i].motion->getWorldTransform( tf );
        tf.getOpenGLMatrix( m_matrix );
        

        //m_boxes[i].motion->m_graphicsWorldTrans.getOpenGLMatrix( m_matrix );

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

        m_boxes[i].drawable->SetLocalTransform( box_pos );

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

    renderer->DrawText( 255, 0, 0, 30, 70, "%d", m_boxes[0].body->isActive() );

    renderer->FlipScreen();

    m_timer.Update();
    if( m_timer.IsReady() )
    {
        if( m_myWorld )
        {

            /*
            if( m_force )
            {
                
                m_boxes[0].body->applyCentralForce( btVector3( 0.0, 300.0, 0.0 ) );
                m_boxes[0].body->applyTorque( btVector3( 80.0, 0.0, 0.0 ) );
                
            }

            if( m_compose )
            {
                btVector3 lv = m_boxes[0].body->getLinearVelocity();
                lv[0] = lv[0] + 0.1;
                m_boxes[0].body->setLinearVelocity( lv );

            }
            */

	        m_myWorld->stepSimulation( 1.0 / (dsreal)( m_timer.GetFPS() ) );
        }
    }
}

void dsAppClient::create_box( void )
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

    drawable->GetMeshe( "" )->LoadFromFile( "object.ac", 0 );    

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


    m_myTransform.setIdentity();
    m_myTransform.setOrigin( btVector3( 0, 12.0, 0 ) );

    btCollisionShape* shape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( 0.5, 0.5, 0.5 ) ) );
    btDefaultMotionState* motion = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( m_myTransform ) );


    btVector3 localInertia( 0, 0, 0 );

    btScalar mass = 8.5;
    shape->calculateLocalInertia( mass, localInertia );

    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo( mass, motion, shape, localInertia );

    btRigidBody* body = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( myBoxRigidBodyConstructionInfo ) );

    if( 0 == m_box_count )
    {
        body->setCollisionFlags( body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
        body->setActivationState( DISABLE_DEACTIVATION );
    }

    m_myWorld->addRigidBody( body );

    //body->setActivationState( DISABLE_DEACTIVATION );

    Box box;

    box.drawable = drawable;
    box.body = body;
    box.motion = motion;

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


    //////////////////////////////////////////////////////////////


    status = DrawSpace::Utils::LoadDrawablePlugin( "chunk.dll", "chunk_plugin" );
    m_ground = DrawSpace::Utils::InstanciateDrawableFromPlugin( "chunk_plugin" );

    m_ground->RegisterPassSlot( "texture_pass" );
    m_ground->SetRenderer( renderer );
    m_ground->SetName( "ground" );

    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe.dll", "ac3dmeshe_plugin" );
    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );
    m_ground->GetMeshe( "" )->SetImporter( m_meshe_import );

    m_ground->GetMeshe( "" )->LoadFromFile( "grid.ac", 0 );
    

    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    

    m_ground->GetNodeFromPass( "texture_pass", "" )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "ground.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( "texture_pass", "" )->GetTexture( 0 )->LoadFromFile();

    m_scenegraph.RegisterNode( m_ground );



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
    m_scenegraph.SetCurrentCamera( "camera" );


    
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();


    //////////////////////////////////////////////////////////////

    m_myCollisionConfiguration = _DRAWSPACE_NEW_( btDefaultCollisionConfiguration, btDefaultCollisionConfiguration );
    m_myDispatcher = _DRAWSPACE_NEW_( btCollisionDispatcher, btCollisionDispatcher( m_myCollisionConfiguration ) );
    m_myBroadphase = _DRAWSPACE_NEW_( btDbvtBroadphase, btDbvtBroadphase );
    m_mySequentialImpulseConstraintSolver = _DRAWSPACE_NEW_( btSequentialImpulseConstraintSolver, btSequentialImpulseConstraintSolver );
    m_myWorld = _DRAWSPACE_NEW_( btDiscreteDynamicsWorld, btDiscreteDynamicsWorld( m_myDispatcher, m_myBroadphase, m_mySequentialImpulseConstraintSolver, m_myCollisionConfiguration ) ); 

    m_myWorld->setGravity( btVector3( 0, -9.81, 0 ) );

    /*
    btCollisionShape* shape = _DRAWSPACE_NEW_( btBoxShape, btBoxShape( btVector3( 0.5, 0.5, 0.5 ) ) );

    m_myTransform.setIdentity();
    m_myTransform.setOrigin( btVector3( 0, 5, 0 ) );

    btVector3 localInertia( 0, 0, 0 );

    btScalar mass = 0.5;
    shape->calculateLocalInertia( mass, localInertia );

    m_myMotionState = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( m_myTransform ) );

    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo( mass, m_myMotionState, shape, localInertia );

    m_body = _DRAWSPACE_NEW_(  btRigidBody, btRigidBody( myBoxRigidBodyConstructionInfo ) );

    m_myWorld->addRigidBody( m_body );
    */


    btCollisionShape* shape_sol = new btBoxShape( btVector3( 100, 0.0, 100 ) );

    m_myTransform.setIdentity();

    // Position du sol
    m_myTransform.setOrigin( btVector3(0,0,0) );
    btVector3 localInertiaSol(0,0,0);

    

    m_myMotionState_Sol = _DRAWSPACE_NEW_( btDefaultMotionState, btDefaultMotionState( m_myTransform ) );

    btRigidBody::btRigidBodyConstructionInfo sol_info( 0.0, m_myMotionState_Sol, shape_sol, localInertiaSol );

    m_body_sol = _DRAWSPACE_NEW_( btRigidBody, btRigidBody( sol_info ) );

    m_body_sol->setContactProcessingThreshold( 0.1 );

    // On ajoute le sol dans le monde Bullet
    m_myWorld->addRigidBody( m_body_sol );


    create_box();

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

            //m_force = true;

            {
                dsreal xpos = m_kpos[0];
                m_timer.TranslationSpeedDec( &xpos, 0.9 ); 
                m_kpos[0] = xpos;
            }

            break;

        case 'Z':

            //m_compose = true;

            {
                dsreal xpos = m_kpos[0];
                m_timer.TranslationSpeedInc( &xpos, 0.9 ); 
                m_kpos[0] = xpos;
            }

            break;

        case 'S':
            {
                dsreal ypos = m_kpos[1];
                m_timer.TranslationSpeedInc( &ypos, 0.9 ); 
                m_kpos[1] = ypos;
            }
            break;

        case 'X':
            {
                dsreal ypos = m_kpos[1];
                m_timer.TranslationSpeedDec( &ypos, 0.9 ); 
                m_kpos[1] = ypos;
            }
            break;

        case 'D':
            {
                dsreal zpos = m_kpos[2];
                m_timer.TranslationSpeedDec( &zpos, 0.9 ); 
                m_kpos[2] = zpos;
            }
            break;

        case 'C':
            {
                dsreal zpos = m_kpos[2];
                m_timer.TranslationSpeedInc( &zpos, 0.9 ); 
                m_kpos[2] = zpos;
            }
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

            create_box();
            break;


        case 'A':

            //m_force = false;
            break;


        case 'Z':

            //m_compose = false;
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
