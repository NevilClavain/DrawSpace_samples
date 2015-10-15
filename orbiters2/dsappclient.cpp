
#include "dsappclient.h"



using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;


_DECLARE_DS_LOGGER( logger, "AppClient", DrawSpace::Logger::Configuration::GetInstance() )


#define SHIP_MASS 50.0


#define HYPERSPACE_SCALE_Z             4000.0
#define HYPERSPACE_SCALE_XY            200.0

#define HYPERSPACE_LAYER2_SCALE_Z      12000.0
#define HYPERSPACE_LAYER2_SCALE_XY     600.0

#define HYPERSPACE_1_ROTZINIT           0.0
#define HYPERSPACE_2_ROTZINIT           90.0

#define HYPERSPACE_LAYER2_1_ROTZINIT    33.0
#define HYPERSPACE_LAYER2_2_ROTZINIT    124.0

#define HYPERSPACE_TRANSLATION_SPEED    7000.0

#define HYPERSPACE_1_ROTZ_SPEED         30.0
#define HYPERSPACE_2_ROTZ_SPEED         35.0

#define HYPERSPACE_LAYER2_1_ROTZ_SPEED  45.0
#define HYPERSPACE_LAYER2_2_ROTZ_SPEED  50.0

#define HYPERSPACE_TRANSITION_SPEED     70000.0

#define HYPERSPACE_ACCELERATION_MAX_SPEED 50000000.0

#define HYPERSPACE_TRANSITION_INIT_Z    -350000.0


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

dsAppClient::dsAppClient( void ) : 
m_mouselb( false ), 
m_mouserb( false ), 
m_speed( 0.0 ), 
m_speed_speed( 5.0 ),
m_curr_camera( NULL ),
m_draw_spacebox( true ),
m_draw_hyperspace( false ),
m_hp( false ),
m_hp_state( HP_NONE )
{    
    _INIT_LOGGER( "logorbiters2.conf" )  
    m_w_title = "orbiters 2 test";
    m_mouseleftbuttondown_eventhandler = _DRAWSPACE_NEW_( WidgetEventHandler, WidgetEventHandler( this, &dsAppClient::on_mouseleftbuttondown ) );

    m_spacebox1hp_transz = -HYPERSPACE_SCALE_Z * 0.5; //-2000.0;
    m_spacebox2hp_transz = -HYPERSPACE_SCALE_Z * 1.5; //-6000.0;

    m_spacebox1bighp_transz = -HYPERSPACE_LAYER2_SCALE_Z * 0.5;//-6000.0;
    m_spacebox2bighp_transz = -HYPERSPACE_LAYER2_SCALE_Z * 1.5;//-18000.0;


    m_spacebox1hp_rotz = HYPERSPACE_1_ROTZINIT;//0.0;
    m_spacebox2hp_rotz = HYPERSPACE_2_ROTZINIT;//90.0;
    m_spacebox1bighp_rotz = HYPERSPACE_LAYER2_1_ROTZINIT;//33.0;
    m_spacebox2bighp_rotz = HYPERSPACE_LAYER2_2_ROTZINIT;//124.0;
    
    m_hp_transition_transz = 0.0;
}

dsAppClient::~dsAppClient( void )
{

}


#define SPEED     1.5



void dsAppClient::manage_hp( void )
{
    switch( m_hp_state )
    {
        case HP_ACCELERATE:
            {
                dsreal speed = m_ship->GetLinearSpeedMagnitude() * 3.6;

                if( speed < HYPERSPACE_ACCELERATION_MAX_SPEED )
                {
                    m_ship->ApplyFwdForce( 200000000.0 ); // acceleration de ouuuuf
                }
                else
                {
                    m_hp_state = HP_IN;
                    m_hp_transition_transz = HYPERSPACE_TRANSITION_INIT_Z;
                    m_hp_transition->SetDrawingState( m_texturepass, true );
                }
            }
            break;

        case HP_IN:
            {
                m_timer.TranslationSpeedInc( &m_hp_transition_transz, HYPERSPACE_TRANSITION_SPEED );

                if( m_hp_transition_transz > 0.0 )
                {
                    m_hp_state = HP_CRUISE;
                    m_ship->ZeroSpeed();

                    m_hp_transition_transz = HYPERSPACE_TRANSITION_INIT_Z;

                    ////////////////////////////

                    m_spacebox1hp.node->GetContent()->SetDrawingState( m_texturepass, true );
                    m_spacebox2hp.node->GetContent()->SetDrawingState( m_texturepass, true );

                    m_spacebox1bighp.node->GetContent()->SetDrawingState( m_texturepass, true );
                    m_spacebox2bighp.node->GetContent()->SetDrawingState( m_texturepass, true );


                    m_spacebox1hp.node->GetContent()->SetQuadDrawingState( m_texturepass, Spacebox::FrontQuad, false );
                    m_spacebox1hp.node->GetContent()->SetQuadDrawingState( m_texturepass, Spacebox::BottomQuad, false );

                    m_spacebox2hp.node->GetContent()->SetQuadDrawingState( m_texturepass, Spacebox::FrontQuad, false );
                    m_spacebox2hp.node->GetContent()->SetQuadDrawingState( m_texturepass, Spacebox::BottomQuad, false );


                    m_spacebox1bighp.node->GetContent()->SetDrawingState( m_texturepass, true );
                    m_spacebox2bighp.node->GetContent()->SetDrawingState( m_texturepass, true );



                    m_spacebox1bighp.node->GetContent()->SetQuadDrawingState( m_texturepass, Spacebox::FrontQuad, false );
                    m_spacebox1bighp.node->GetContent()->SetQuadDrawingState( m_texturepass, Spacebox::BottomQuad, false );

                    m_spacebox2bighp.node->GetContent()->SetQuadDrawingState( m_texturepass, Spacebox::FrontQuad, false );
                    m_spacebox2bighp.node->GetContent()->SetQuadDrawingState( m_texturepass, Spacebox::BottomQuad, false );


                    m_spacebox->SetDrawingState( m_texturepass, false );

                    m_hp_transition->SetDrawingState( m_texturepass, false );

                }
            }
            break;

        case HP_CRUISE:
            break;


        case HP_OUT:
            {
                m_timer.TranslationSpeedInc( &m_hp_transition_transz, HYPERSPACE_TRANSITION_SPEED );

                if( m_hp_transition_transz > 0.0 )
                {

                    m_spacebox1hp.node->GetContent()->SetDrawingState( m_texturepass, false );
                    m_spacebox2hp.node->GetContent()->SetDrawingState( m_texturepass, false );

                    m_spacebox1bighp.node->GetContent()->SetDrawingState( m_texturepass, false );
                    m_spacebox2bighp.node->GetContent()->SetDrawingState( m_texturepass, false );

                    m_spacebox->SetDrawingState( m_texturepass, true );

                    ////////////////////////////

                    m_hp_state = HP_DECELERATE;
                    m_ship->ForceLinearSpeed( Vector( 0.0, 0.0, -HYPERSPACE_ACCELERATION_MAX_SPEED, 1.0 ) );
                    
                    Matrix mat;   
                    mat.Translation( 269000000.0, 0.0, 69000000.0 );
                    m_ship->ForceInitialAttitude( mat );

                    m_hp_transition->SetDrawingState( m_texturepass, false );
                }
            }
            break;

        case HP_DECELERATE:
            {
                dsreal speed = m_ship->GetLinearSpeedMagnitude() * 3.6;

                if( speed > 10000000.0 )
                {
                    m_ship->ApplyRevForce( 1000000000.0 );
                }
                else if( speed > 200000.0 )
                {
                    m_ship->ApplyRevForce( 100000000.0 );
                }
                else if( speed > 10000.0 )
                {
                    m_ship->ApplyRevForce( 1000000.0 );
                }
                else
                {
                    m_hp_state = HP_NONE;
                }
            }
            break;
    }

    if( m_hp_state == HP_IN || m_hp_state == HP_OUT )
    {
        Matrix hptranspos;
        hptranspos.Translation( Vector( 0.0, 0.0, m_hp_transition_transz, 1.0 ) );
        Matrix hptransscale;

        hptransscale.Scale( 1000.0, 1000.0, 1000.0 );

        m_transition_transfo_node->GetContent()->ClearAll();
        m_transition_transfo_node->GetContent()->PushMatrix( hptranspos );
        m_transition_transfo_node->GetContent()->PushMatrix( hptransscale );
    }



    if( m_hp_state == HP_CRUISE || m_hp_state == HP_OUT )
    {

        Matrix spacebox1hp_scale;
        Matrix spacebox1hp_trans;
        Matrix spacebox1hp_rotz;
        spacebox1hp_scale.Scale( HYPERSPACE_SCALE_XY, HYPERSPACE_SCALE_XY, HYPERSPACE_SCALE_Z );
        spacebox1hp_trans.Translation( 0.0, 0.0, m_spacebox1hp_transz );
        spacebox1hp_rotz.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( m_spacebox1hp_rotz ) );

        
        m_spacebox1hp.transfo_node->GetContent()->ClearAll();
        m_spacebox1hp.transfo_node->GetContent()->PushMatrix( spacebox1hp_trans );
        m_spacebox1hp.transfo_node->GetContent()->PushMatrix( spacebox1hp_rotz );
        m_spacebox1hp.transfo_node->GetContent()->PushMatrix( spacebox1hp_scale );
        
        



        Matrix spacebox2hp_trans;
        Matrix spacebox2hp_rotz;
        spacebox2hp_trans.Translation( 0.0, 0.0, m_spacebox2hp_transz );
        spacebox2hp_rotz.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( m_spacebox2hp_rotz ) );

        
        
        m_spacebox2hp.transfo_node->GetContent()->ClearAll();
        m_spacebox2hp.transfo_node->GetContent()->PushMatrix( spacebox2hp_trans );
        m_spacebox2hp.transfo_node->GetContent()->PushMatrix( spacebox2hp_rotz );
        m_spacebox2hp.transfo_node->GetContent()->PushMatrix( spacebox1hp_scale );
        
        
        



        
        Matrix spacebox1bighp_scale;
        Matrix spacebox1bighp_rotz;
        Matrix spacebox1bighp_trans;

        spacebox1bighp_scale.Scale( HYPERSPACE_LAYER2_SCALE_XY, HYPERSPACE_LAYER2_SCALE_XY, HYPERSPACE_LAYER2_SCALE_Z );
        spacebox1bighp_rotz.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( m_spacebox1bighp_rotz ) );
        spacebox1bighp_trans.Translation( 0.0, 0.0, m_spacebox1bighp_transz );

        
        m_spacebox1bighp.transfo_node->GetContent()->ClearAll();
        m_spacebox1bighp.transfo_node->GetContent()->PushMatrix( spacebox1bighp_trans );
        m_spacebox1bighp.transfo_node->GetContent()->PushMatrix( spacebox1bighp_rotz );
        m_spacebox1bighp.transfo_node->GetContent()->PushMatrix( spacebox1bighp_scale );
        
        

        Matrix spacebox2bighp_rotz;
        Matrix spacebox2bighp_trans;
        spacebox2bighp_trans.Translation( 0.0, 0.0, m_spacebox2bighp_transz );
        spacebox2bighp_rotz.Rotation( Vector( 0.0, 0.0, 1.0, 1.0 ), Maths::DegToRad( m_spacebox2bighp_rotz ) );
        
        m_spacebox2bighp.transfo_node->GetContent()->ClearAll();
        m_spacebox2bighp.transfo_node->GetContent()->PushMatrix( spacebox2bighp_trans );
        m_spacebox2bighp.transfo_node->GetContent()->PushMatrix( spacebox2bighp_rotz );
        m_spacebox2bighp.transfo_node->GetContent()->PushMatrix( spacebox1bighp_scale );
        


        m_timer.AngleSpeedInc( &m_spacebox1hp_rotz, HYPERSPACE_1_ROTZ_SPEED );
        m_timer.AngleSpeedInc( &m_spacebox2hp_rotz, HYPERSPACE_2_ROTZ_SPEED );


        m_timer.AngleSpeedInc( &m_spacebox1bighp_rotz, HYPERSPACE_LAYER2_1_ROTZ_SPEED );
        m_timer.AngleSpeedInc( &m_spacebox2bighp_rotz, HYPERSPACE_LAYER2_2_ROTZ_SPEED );

        m_timer.TranslationSpeedInc( &m_spacebox1hp_transz, HYPERSPACE_TRANSLATION_SPEED );

        if( m_spacebox1hp_transz > HYPERSPACE_SCALE_Z )
        {
            m_spacebox1hp_transz = -HYPERSPACE_SCALE_Z;
        }

        m_timer.TranslationSpeedInc( &m_spacebox2hp_transz, HYPERSPACE_TRANSLATION_SPEED );

        if( m_spacebox2hp_transz > HYPERSPACE_SCALE_Z )
        {
            m_spacebox2hp_transz = -HYPERSPACE_SCALE_Z;
        }


        m_timer.TranslationSpeedInc( &m_spacebox1bighp_transz, HYPERSPACE_TRANSLATION_SPEED );

        if( m_spacebox1bighp_transz > HYPERSPACE_LAYER2_SCALE_Z )
        {
            m_spacebox1bighp_transz = -HYPERSPACE_LAYER2_SCALE_Z;
        }

        m_timer.TranslationSpeedInc( &m_spacebox2bighp_transz, HYPERSPACE_TRANSLATION_SPEED );

        if( m_spacebox2bighp_transz > HYPERSPACE_LAYER2_SCALE_Z )
        {
            m_spacebox2bighp_transz = -HYPERSPACE_LAYER2_SCALE_Z;
        }
    }

}


void dsAppClient::OnRenderFrame( void )
{
    

    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;



    manage_hp();
    


    m_scenenodegraph.ComputeTransformations( m_timer );

    m_text_widget->SetVirtualTranslation( 100, 75 );
    m_text_widget_2->SetVirtualTranslation( -60, 160 );

    char distance[64];
    sprintf( distance, "%.3f km", m_reticle_widget->GetLastDistance() / 1000.0 );

    m_text_widget_2->SetText( -40, 0, 30, dsstring( distance ), DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
    m_reticle_widget->Transform();
    m_reticle_widget->Draw();
    

    //////////////////////////////////////////////////////////////


   

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

    //renderer->DrawText( 0, 255, 0, 10, 115, "contact = %d", m_ship->GetContactState() );

    //renderer->DrawText( 0, 255, 0, 10, 300, "reticle distance = %f", m_reticle_widget->GetLastDistance() );
    //renderer->DrawText( 0, 255, 0, 10, 330, "hp state = %d hp_transition_z = %f", m_hp_state, m_hp_transition_transz );



    //int x_reticle, y_reticle;
    //m_reticle_widget->GetScreenPos( x_reticle, y_reticle );

    
    // current camera infos
/*
    dsstring camera_name;

    if( m_curr_camera )
    {
        m_curr_camera->GetSceneName( camera_name );
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
*/
    //renderer->DrawText( 0, 255, 0, 10, 300, "reticle distance = %f", m_reticle_widget->GetLastDistance() );

    /*
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
*/


    renderer->FlipScreen();

    m_calendar->Run();

    

    m_freemove.SetSpeed( m_speed );
}


void dsAppClient::init_hpspacebox( const dsstring& p_scenename, Pass* p_pass, hp_spacebox* p_hpsb, long p_order )
{
    DrawSpace::Spacebox* spacebox;

    spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
    spacebox->RegisterPassSlot( m_texturepass );
    
    for( long i = 0; i < 6; i++ )
    {

        spacebox->GetNodeFromPass( p_pass, i )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );
        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( false ) ) );

        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->GetShader( 0 )->SetText(

            "float4x4 matWorldViewProjection: register(c0);"

            "struct VS_INPUT"
            "{"
               "float4 Position : POSITION0;"
               "float4 TexCoord0: TEXCOORD0;"              
            "};"

            "struct VS_OUTPUT"
            "{"
               "float4 Position : POSITION0;"
               "float4 TexCoord0: TEXCOORD0;"
            "};"

            "VS_OUTPUT vs_main( VS_INPUT Input )"
            "{"
               "VS_OUTPUT Output;"

               "Output.Position = mul( Input.Position, matWorldViewProjection );"
               "Output.TexCoord0 = Input.TexCoord0;"
                  
               "return( Output );"
            "}"

                );

        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->GetShader( 1 )->SetText(

        "float alpha: register(c0);"
        "sampler2D Texture0;"
        "struct PS_INTPUT"
        "{"
           "float4 Position : POSITION0;"
           "float4 TexCoord0: TEXCOORD0;"
        "};"
        "float4 ps_main( PS_INTPUT input ) : COLOR0"
        "{"
           "float4 color = tex2D( Texture0, input.TexCoord0 );"

           "float4 final_color;"

           "final_color.r = color.r;"
           "final_color.g = color.g;"
           "final_color.b = color.b;"
           "final_color.a = alpha;"

           "return final_color;"

        "}"
                );

        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );

        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, /*"invsrcalpha"*/ "one"  ) );
        spacebox->GetNodeFromPass( p_pass, i )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );

        spacebox->GetNodeFromPass( p_pass, i )->AddShaderParameter( 1, "alpha", 0 );
        spacebox->GetNodeFromPass( p_pass, i )->SetShaderReal( "alpha", 1.0 );

    }


    spacebox->GetNodeFromPass( p_pass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_front5.png" ) ), 0 );
    spacebox->GetNodeFromPass( p_pass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();


    spacebox->GetNodeFromPass( p_pass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_back6.png" ) ), 0 );
    spacebox->GetNodeFromPass( p_pass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();


    spacebox->GetNodeFromPass( p_pass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_top3.png" ) ), 0 );
    spacebox->GetNodeFromPass( p_pass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();


    spacebox->GetNodeFromPass( p_pass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_bottom4.png" ) ), 0 );
    spacebox->GetNodeFromPass( p_pass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();


    spacebox->GetNodeFromPass( p_pass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_left2.png" ) ), 0 );
    spacebox->GetNodeFromPass( p_pass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();


    spacebox->GetNodeFromPass( p_pass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_right1.png" ) ), 0 );
    spacebox->GetNodeFromPass( p_pass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    spacebox->GetNodeFromPass( p_pass, Spacebox::FrontQuad )->SetOrderNumber( p_order );
    spacebox->GetNodeFromPass( p_pass, Spacebox::RearQuad )->SetOrderNumber( p_order );
    spacebox->GetNodeFromPass( p_pass, Spacebox::TopQuad )->SetOrderNumber( p_order );
    spacebox->GetNodeFromPass( p_pass, Spacebox::BottomQuad )->SetOrderNumber( p_order );
    spacebox->GetNodeFromPass( p_pass, Spacebox::LeftQuad )->SetOrderNumber( p_order );
    spacebox->GetNodeFromPass( p_pass, Spacebox::RightQuad )->SetOrderNumber( p_order );


    spacebox->EnableTranslations( true );
    spacebox->IgnoreCamera( true );

    /*
    spacebox->SetQuadDrawingState( p_pass, Spacebox::FrontQuad, false );
    spacebox->SetQuadDrawingState( p_pass, Spacebox::RearQuad, false );
    */

    spacebox->SetDrawingState( p_pass, false );
    

    SceneNode<DrawSpace::Spacebox>* node;

    node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Spacebox>, SceneNode<DrawSpace::Spacebox>( p_scenename ) );
    node->SetContent( spacebox );

    //m_scenenodegraph_hyperspace.RegisterNode( node );
    m_scenenodegraph.RegisterNode( node );


    DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>* transfo_node;


    transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( p_scenename + "_transfo" ) );
    transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

    
    //m_scenenodegraph_hyperspace.RegisterNode( transfo_node );
    m_scenenodegraph.RegisterNode( transfo_node );

    node->LinkTo( transfo_node );


    p_hpsb->node = node;
    p_hpsb->transfo_node = transfo_node;
}

bool dsAppClient::OnIdleAppInit( void )
{
    DrawSpace::Dynamics::Body::Parameters cube_params;

    World::m_scale = 1.0;
    DrawSpace::SphericalLOD::Body::BuildMeshe();


    RegisterMouseInputEventsProvider( &m_mouse_input );


    bool status;

    /*
    status = DrawSpace::Utils::LoadMesheImportPlugin( "ac3dmeshe", "ac3dmeshe_plugin" );
    if( !status )
    {
        _DSEXCEPTION( "Cannot load ac3dmeshe plugin !" )
    }
    _DSDEBUG( logger, "ac3dmeshe plugin successfully loaded..." );

    m_meshe_import = DrawSpace::Utils::InstanciateMesheImportFromPlugin( "ac3dmeshe_plugin" );
    */
     m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    /////////////////////////////////////


    //status = DrawSpace::Utils::LoadFontImportPlugin( "cbfgfont", "cbfgfont_plugin" );
    //m_font_import = DrawSpace::Utils::InstanciateFontImportFromPlugin( "cbfgfont_plugin" );
    m_font_import = new DrawSpace::Utils::CBFGFontImport();
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
    m_texturepass->Initialize();

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( /*false*/ true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 255 );
    


    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();

    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );



    //////////////////////////////////////////////////////////////

    /*
    m_texturepass_hyperspace = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass_hyperspace" ) );
    m_texturepass_hyperspace->Initialize();

    m_texturepass_hyperspace->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass_hyperspace->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass_hyperspace->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0 );



    m_finalpass_hyperspace = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass_hyperspace" ) );
    m_finalpass_hyperspace->Initialize();
    m_finalpass_hyperspace->CreateViewportQuad();

    m_finalpass_hyperspace->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) ) ;
    m_finalpass_hyperspace->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass_hyperspace->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass_hyperspace->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass_hyperspace->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass_hyperspace->GetViewportQuad()->SetTexture( m_texturepass_hyperspace->GetTargetTexture(), 0 );
    */

    

    //////////////////////////////////////////////////////////////

    m_spacebox = _DRAWSPACE_NEW_( DrawSpace::Spacebox, DrawSpace::Spacebox );
    m_spacebox->RegisterPassSlot( m_texturepass );
    

    DrawSpace::Utils::BuildSpaceboxFx( m_spacebox, m_texturepass );

    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_front5.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_back6.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_top3.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_bottom4.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_left2.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "spacebox_right1.png" ) ), 0 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->GetTexture( 0 )->LoadFromFile();


    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::FrontQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RearQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::TopQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::BottomQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::LeftQuad )->SetOrderNumber( 90 );
    m_spacebox->GetNodeFromPass( m_texturepass, Spacebox::RightQuad )->SetOrderNumber( 90 );

    

    m_spacebox_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Spacebox>, SceneNode<DrawSpace::Spacebox>( "spacebox" ) );
    m_spacebox_node->SetContent( m_spacebox );

    m_scenenodegraph.RegisterNode( m_spacebox_node );


    m_spacebox_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "spacebox_transfo" ) );
    m_spacebox_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix spacebox_scale;
    spacebox_scale.Scale( 20.0, 20.0, 20.0 );
    m_spacebox_transfo_node->GetContent()->PushMatrix( spacebox_scale );

    m_scenenodegraph.AddNode( m_spacebox_transfo_node );
    m_scenenodegraph.RegisterNode( m_spacebox_transfo_node );
    m_spacebox_node->LinkTo( m_spacebox_transfo_node );

   
    //////////////////////////////////////////////////////////////


    m_world.Initialize();
    
    
    //////////////////////////////////////////////////////////////



    m_hp_transition = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_hp_transition->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_hp_transition->RegisterPassSlot( m_texturepass );
    
    m_hp_transition->GetMeshe()->SetImporter( m_meshe_import );

    m_hp_transition->GetMeshe()->LoadFromFile( "hptransition.ac", 0 );    

    m_hp_transition->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    //m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    //m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    


    
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );

    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add"  ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always"  ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "one"  ) );
    m_hp_transition->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha"  ) );
    


    m_hp_transition->GetNodeFromPass( m_texturepass )->SetOrderNumber( 500 );

    m_hp_transition->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "plasma1.jpg" ) ), 0 );



    m_hp_transition->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    m_hp_transition->IgnoreCamera( true );
    m_hp_transition->SetDrawingState( m_texturepass, false );


    m_hp_transition_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "hptransition" ) );
    m_hp_transition_node->SetContent( m_hp_transition );


    m_scenenodegraph.RegisterNode( m_hp_transition_node );


    m_transition_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "hptransition_transfo" ) );
    m_transition_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

        
    m_scenenodegraph.AddNode( m_transition_transfo_node );
    m_scenenodegraph.RegisterNode( m_transition_transfo_node );

    m_hp_transition_node->LinkTo( m_transition_transfo_node );


    /*
    Matrix hptranspos;
    hptranspos.Translation( Vector( 269000000.0, 0.0, 8999500.0, 1.0 ) );
    Matrix hptransscale;

    hptransscale.Scale( 100.0, 100.0, 100.0 );

    m_transition_transfo_node->GetContent()->PushMatrix( hptranspos );
    m_transition_transfo_node->GetContent()->PushMatrix( hptransscale );
    */









    //////////////////////////////////////////////////////////////////////////////////////////////

    
    m_cube = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_cube->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_cube->RegisterPassSlot( m_texturepass );
    
    m_cube->GetMeshe()->SetImporter( m_meshe_import );

    m_cube->GetMeshe()->LoadFromFile( "object.ac", 0 );    

    m_cube->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_cube->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_cube->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_cube->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_cube->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_cube->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_cube->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_cube->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_cube->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_cube->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "tex07.jpg" ) ), 0 );



    m_cube->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_cube_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "cube_node" ) );
    m_cube_node->SetContent( m_cube );


    m_scenenodegraph.RegisterNode( m_cube_node );



    
    cube_params.mass = 50.0;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 ); //DrawSpace::Utils::Vector( 0.5, 0.5, 0.5, 1.0 );



    m_cube_body = _DRAWSPACE_NEW_( DrawSpace::Dynamics::InertBody, DrawSpace::Dynamics::InertBody( &m_world, cube_params ) );
    
    m_cube_body_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::InertBody>, SceneNode<DrawSpace::Dynamics::InertBody>( "cube_body_node" ) );

    m_cube_body_node->SetContent( m_cube_body );

    m_scenenodegraph.AddNode( m_cube_body_node );
    m_scenenodegraph.RegisterNode( m_cube_body_node );

    m_cube_node->LinkTo( m_cube_body_node );


    m_longlat_mvt2 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt2->Init( -21.0000, 20.000, 400014.5, 0.0, 0.0 );
    m_longlat_mvt2->Compute( m_timer );
    Matrix llres;
    m_longlat_mvt2->GetResult( llres );


    m_cube_body->SetDynamicLinkInitState( true );
    m_cube_body->SetDynamicLinkInitialMatrix( llres );
    



    ///////////////////////////////////////////////////////////////


    
    m_building = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_building->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_building->RegisterPassSlot( m_texturepass );
    
    m_building->GetMeshe()->SetImporter( m_meshe_import );

    m_building->GetMeshe()->LoadFromFile( "building3.ac", 0 );  



    m_building->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_building->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_building->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "building3.bmp" ) ), 0 );
    m_building->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_building_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "building" ) );
    m_building_node->SetContent( m_building );


    m_scenenodegraph.RegisterNode( m_building_node );
  
    
    DrawSpace::Dynamics::Body::Parameters bld_params;
    bld_params.mass = 0.0;
    bld_params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
    bld_params.shape_descr.meshe = *( m_building->GetMeshe() );
   
    m_building_collider = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Collider, DrawSpace::Dynamics::Collider );

    m_building_collider->SetKinematic( bld_params );



    m_building_collider_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Collider>, SceneNode<DrawSpace::Dynamics::Collider>( "building_collider" ) );
    m_building_collider_node->SetContent( m_building_collider );

    m_scenenodegraph.RegisterNode( m_building_collider_node );

    m_building_node->LinkTo( m_building_collider_node );



    m_longlat_mvt3 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt3->Init( -21.0000, 20.0089, 400114.0, 80.0, 0.0 );

    m_longlatmvt3_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::LongLatMovement>, SceneNode<DrawSpace::Core::LongLatMovement>( "longlatmvt3_node" ) );
    m_longlatmvt3_node->SetContent( m_longlat_mvt3 );

    m_scenenodegraph.RegisterNode( m_longlatmvt3_node );

    m_building_collider_node->LinkTo( m_longlatmvt3_node );
    
    

    ///////////////////////////////////////////////////////////////


    m_socle = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_socle->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_socle->RegisterPassSlot( m_texturepass );


    
    m_socle->GetMeshe()->SetImporter( m_meshe_import );

    m_socle->GetMeshe()->LoadFromFile( "socle.ac", 0 );  



    m_socle->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_socle->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_socle->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "socle.bmp" ) ), 0 );

    m_socle->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();


    m_socle_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "socle" ) );
    m_socle_node->SetContent( m_socle );


    m_scenenodegraph.RegisterNode( m_socle_node );

    
  
    
    DrawSpace::Dynamics::Body::Parameters socle_params;
    socle_params.mass = 0.0;
    socle_params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
    socle_params.shape_descr.meshe = *( m_socle->GetMeshe() );


   
    m_socle_collider = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Collider, DrawSpace::Dynamics::Collider );

    m_socle_collider->SetKinematic( socle_params );


    m_socle_collider_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Collider>, SceneNode<DrawSpace::Dynamics::Collider>( "socle_collider" ) );
    m_socle_collider_node->SetContent( m_socle_collider );

    m_scenenodegraph.RegisterNode( m_socle_collider_node );

    m_socle_node->LinkTo( m_socle_collider_node );



    m_longlat_mvt4 = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt4->Init( -21.0929, 20.0000, 400129.0, 40.0, 0.0 );



    m_longlatmvt4_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::LongLatMovement>, SceneNode<DrawSpace::Core::LongLatMovement>( "longlatmvt4_node" ) );
    m_longlatmvt4_node->SetContent( m_longlat_mvt4 );

    m_scenenodegraph.RegisterNode( m_longlatmvt4_node );

    m_socle_collider_node->LinkTo( m_longlatmvt4_node );



    ///////////////////////////////////////////////////////////////




    
    m_planet = _DRAWSPACE_NEW_( DrawSpace::Planetoid::Body, DrawSpace::Planetoid::Body( "planet01", 400.0 ) );

    m_planet->RegisterPassSlot( m_texturepass );

    Texture* texture_planet = _DRAWSPACE_NEW_( Texture, Texture( "mapcolor.bmp" ) );
    texture_planet->LoadFromFile();

    Shader* planet_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planet2.vsh", false ) );
    Shader* planet_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planet2.psh", false ) );
    planet_vshader->LoadFromFile();
    planet_pshader->LoadFromFile();
    
    for( long i = 0; i < 6; i++ )
    {
        m_planet->CreateFx( m_texturepass, i );

        m_planet->AddShader( m_texturepass, i, planet_vshader );
        m_planet->AddShader( m_texturepass, i, planet_pshader );

        m_planet->BindExternalGlobalTexture( texture_planet, m_texturepass, i );
    }
    

    m_planet->SetOrbitDuration( 0.333 );
    m_planet->SetRevolutionTiltAngle( 25.0 );
    m_planet->SetRevolutionDuration( 1.0 );


    m_planet_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Planetoid::Body>, SceneNode<DrawSpace::Planetoid::Body>( "planet01" ) );
    m_planet_node->SetContent( m_planet );

    m_scenenodegraph.RegisterNode( m_planet_node );

    
    m_longlatmvt3_node->LinkTo( m_planet_node );
    m_longlatmvt4_node->LinkTo( m_planet_node );


    m_planet_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 270000000.0, 0.99, 0.0, 0.0, 0.0, 0.0, 0.333, 25.0, 1.0, NULL ) );
    m_planet_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "planet01_orbit" ) );
    m_planet_orbit_node->SetContent( m_planet_orbit );

    m_scenenodegraph.AddNode( m_planet_orbit_node );
    m_scenenodegraph.RegisterNode( m_planet_orbit_node );


    m_planet_node->LinkTo( m_planet_orbit_node );

    


    m_building_collider->SetReferentOrbiter( m_planet );
    m_socle_collider->SetReferentOrbiter( m_planet );
    m_cube_body->SetReferentBody( m_planet );



    //////////////////////////////////////////////////////////////


    m_moon = _DRAWSPACE_NEW_( DrawSpace::Planetoid::Body, DrawSpace::Planetoid::Body( "moon", 300.0 ) );

    m_moon->RegisterPassSlot( m_texturepass );

    
    for( long i = 0; i < 6; i++ )
    {
        m_moon->CreateFx( m_texturepass, i );

        m_moon->AddShader( m_texturepass, i, planet_vshader );
        m_moon->AddShader( m_texturepass, i, planet_pshader );

        m_moon->BindExternalGlobalTexture( texture_planet, m_texturepass, i );
    }
    

    m_moon->SetOrbitDuration( 0.002 );
    m_moon->SetRevolutionTiltAngle( 0.0 );
    m_moon->SetRevolutionDuration( 1.0 );


    m_moon_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Planetoid::Body>, SceneNode<DrawSpace::Planetoid::Body>( "moon" ) );
    m_moon_node->SetContent( m_moon );

    m_scenenodegraph.RegisterNode( m_moon_node );




    m_moon_orbit = _DRAWSPACE_NEW_( Orbit, Orbit( 20000000.0, 0.99, 0.0, 0.0, 0.0, 0.0, 0.002, 0.0, 1.0, NULL ) );
    m_moon_orbit_node = _DRAWSPACE_NEW_( SceneNode<Orbit>, SceneNode<Orbit>( "moon_orbit" ) );
    m_moon_orbit_node->SetContent( m_moon_orbit );

    m_scenenodegraph.RegisterNode( m_moon_orbit_node );


    m_moon_node->LinkTo( m_moon_orbit_node );


    m_moon_orbit_node->LinkTo( m_planet_orbit_node );

    





    //////////////////////////////////////////////////////////////


    m_ship_drawable = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ship_drawable->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_ship_drawable->RegisterPassSlot( m_texturepass );

    
    m_ship_drawable->GetMeshe()->SetImporter( m_meshe_import );

    m_ship_drawable->GetMeshe()->LoadFromFile( "survey.ac", 0 );    

    m_ship_drawable->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

    m_ship_drawable->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "surveyship_color.jpg" ) ), 0 );



    m_ship_drawable->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();

    

    m_ship_drawable_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "rocket" ) );
    m_ship_drawable_node->SetContent( m_ship_drawable );
    

    m_scenenodegraph.RegisterNode( m_ship_drawable_node );



    cube_params.mass = SHIP_MASS;
    cube_params.shape_descr.shape = DrawSpace::Dynamics::Body::BOX_SHAPE;
    cube_params.shape_descr.box_dims = DrawSpace::Utils::Vector( 74.1285 / 2.0, 21.4704 / 2.0, 81.911 / 2.0, 1.0 );
    
    //cube_params.initial_attitude.Translation( 265000000.0, 0.0, 0.0 );
    cube_params.initial_attitude.Translation( 269000000.0, 0.0, 9000000.0 );

    m_ship = _DRAWSPACE_NEW_( DrawSpace::Dynamics::Rocket, DrawSpace::Dynamics::Rocket( &m_world, cube_params ) );
   
    m_ship_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::Rocket>, SceneNode<DrawSpace::Dynamics::Rocket>( "rocket_body" ) );
    m_ship_node->SetContent( m_ship );

    m_scenenodegraph.AddNode( m_ship_node );
    m_scenenodegraph.RegisterNode( m_ship_node );

    m_ship_drawable_node->LinkTo( m_ship_node );

    //////////////////////////////////////////////////////////////


    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );
    m_scenenodegraph.RegisterNode( m_camera_node );


    
    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera2->SetReferentBody( m_ship );
    
    m_camera2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera2" ) );
    m_camera2_node->SetContent( m_camera2 );

    m_scenenodegraph.RegisterNode( m_camera2_node );






    m_head_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::HeadMovement, DrawSpace::Core::HeadMovement );

    m_head_mvt->Init( 1.0, 8000.0, Vector( 0.0, 25.0, 110.0, 1.0 ) );
    m_head_mvt->SetRefBody( m_ship );

    m_headmvt_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::HeadMovement>, SceneNode<DrawSpace::Core::HeadMovement>( "headmvt" ) );
    m_headmvt_node->SetContent( m_head_mvt );
    m_scenenodegraph.RegisterNode( m_headmvt_node );

    m_camera2_node->LinkTo( m_headmvt_node );


    m_headmvt_node->LinkTo( m_ship_node );



    



    m_camera3 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera3->LockOnBody( "ship", m_ship );
    m_camera3->Lock( m_ship_node );
    m_camera3->SetReferentBody( m_ship );
    
    m_camera3_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera3" ) );
    m_camera3_node->SetContent( m_camera3 );
    m_scenenodegraph.RegisterNode( m_camera3_node );


    m_circular_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::CircularMovement, DrawSpace::Core::CircularMovement );
    m_circular_mvt->Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( 385.0, 90.0, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 0.0, 0.0, 0.0 );

    m_circmvt_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::CircularMovement>, SceneNode<DrawSpace::Core::CircularMovement>( "circmvt" ) );


    m_circmvt_node->SetContent( m_circular_mvt );
    m_scenenodegraph.RegisterNode( m_circmvt_node );

    m_camera3_node->LinkTo( m_circmvt_node );


    m_circmvt_node->LinkTo( m_ship_node );



    m_camera4 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    //m_camera4->LockOnBody( "cube", m_cube_body );
    m_camera4->Lock( m_cube_body_node );
    m_camera4->SetReferentBody( m_ship );

    m_camera4_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera4" ) );
    m_camera4_node->SetContent( m_camera4 );
    m_scenenodegraph.RegisterNode( m_camera4_node );

    m_camera4_node->LinkTo( m_circmvt_node );




    m_camera5 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );

    m_camera5->SetReferentBody( m_planet );
    m_camera5_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera5" ) );
    m_camera5_node->SetContent( m_camera5 );

    m_scenenodegraph.RegisterNode( m_camera5_node );


    m_longlat_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::LongLatMovement, DrawSpace::Core::LongLatMovement );
    m_longlat_mvt->Init( -21.0, 20.0, 400000.5, 0.0, 0.0 );



    m_longlatmvt_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::LongLatMovement>, SceneNode<DrawSpace::Core::LongLatMovement>( "longlatmvt_node" ) );
    m_longlatmvt_node->SetContent( m_longlat_mvt );

    m_scenenodegraph.RegisterNode( m_longlatmvt_node );

    






    m_circular_mvt2 = _DRAWSPACE_NEW_( DrawSpace::Core::CircularMovement, DrawSpace::Core::CircularMovement );
    m_circular_mvt2->Init( Vector( 0.0, 0.0, 0.0, 1.0 ), Vector( 25.0, 0.9, 0.0, 1.0 ), Vector( 0.0, 1.0, 0.0, 1.0 ), 0.0, 0.0, 0.0 );

    m_circular_mvt2->SetAngularSpeed( 25.0 );


    m_circmvt2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::CircularMovement>, SceneNode<DrawSpace::Core::CircularMovement>( "circularmvt2_node" ) );
    m_circmvt2_node->SetContent( m_circular_mvt2 );

    m_scenenodegraph.RegisterNode( m_circmvt2_node );

    
    m_camera5_node->LinkTo( m_circmvt2_node );
    m_circmvt2_node->LinkTo( m_longlatmvt_node );
    m_longlatmvt_node->LinkTo( m_planet_node );




    m_camera6 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera6->SetReferentBody( m_planet );
    //m_camera6->LockOnBody( "ship", m_ship );
    m_camera6->Lock( m_ship_node );


    m_camera6_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera6" ) );
    m_camera6_node->SetContent( m_camera6 );

    m_scenenodegraph.RegisterNode( m_camera6_node );



    m_spectator_mvt = _DRAWSPACE_NEW_( DrawSpace::Core::SpectatorMovement, DrawSpace::Core::SpectatorMovement );
    m_spectator_mvt->SetName( "spectator_camera" );
    m_spectator_mvt->Init( 16.0, 8000, true );
    m_spectator_mvt->SetRefBody( m_ship );

    m_spectmvt_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::SpectatorMovement>, SceneNode<DrawSpace::Core::SpectatorMovement>( "spectmvt_node" ) );
    m_spectmvt_node->SetContent( m_spectator_mvt );

    m_scenenodegraph.RegisterNode( m_spectmvt_node );

    m_spectmvt_node->LinkTo( m_planet_node );


    m_camera6_node->LinkTo( m_spectmvt_node );

    m_camera7 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    m_camera7->SetReferentBody( m_cube_body );
    
    m_camera7_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera7" ) );
    m_camera7_node->SetContent( m_camera7 );

    m_scenenodegraph.RegisterNode( m_camera7_node );



    m_camera7_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "cam7_transfo" ) );
    m_camera7_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix cam7_pos;
    cam7_pos.Translation( 0.0, 0.0, 4.0 );
    m_camera7_transfo_node->GetContent()->PushMatrix( cam7_pos );

    
    m_scenenodegraph.RegisterNode( m_camera7_transfo_node );

    m_camera7_transfo_node->LinkTo( m_cube_body_node );
    m_camera7_node->LinkTo( m_camera7_transfo_node );



    
    m_camera8 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );
    

    m_camera8->SetReferentBody( m_planet );
    //m_camera8->LockOnBody( "cube", m_cube_body );
    m_camera8->Lock( m_cube_body_node );

    m_camera8_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera8" ) );
    m_camera8_node->SetContent( m_camera8 );

    m_scenenodegraph.RegisterNode( m_camera8_node );


    //m_camera8_node->LinkTo( m_building_collider_node );


    m_camera8_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "cam8_transfo" ) );
    m_camera8_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );
    Matrix cam8_pos;
    cam8_pos.Translation( 0.0, 130.0, 0.0 );
    m_camera8_transfo_node->GetContent()->PushMatrix( cam8_pos );

    
    m_scenenodegraph.RegisterNode( m_camera8_transfo_node );

    m_camera8_transfo_node->LinkTo( m_building_collider_node );
    m_camera8_node->LinkTo( m_camera8_transfo_node );

    



    ///////////////////////////////////////////////////////////////


    m_reticle_widget = _DRAWSPACE_NEW_( ReticleWidget, ReticleWidget( "reticle_widget", (long)10, (long)10, /*&m_scenegraph,*/ &m_scenenodegraph, NULL ) );
    


    
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

    












    m_reticle_widget->Lock( m_building_node );
    //m_reticle_widget->LockOnTransformNode( m_building );
    //m_reticle_widget->LockOnBody( m_moon );
    //m_reticle_widget->LockOnTransformNode( m_hp_transition );

    
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

    init_hpspacebox( "spacebox1hp", m_texturepass, &m_spacebox1hp, 200 );
    //m_scenenodegraph_hyperspace.AddNode( m_spacebox1hp.transfo_node );
    m_scenenodegraph.AddNode( m_spacebox1hp.transfo_node );


    init_hpspacebox( "spacebox2hp", m_texturepass, &m_spacebox2hp, 200 );
    //m_scenenodegraph_hyperspace.AddNode( m_spacebox2hp.transfo_node );
    m_scenenodegraph.AddNode( m_spacebox2hp.transfo_node );


    init_hpspacebox( "spacebox1bighp", m_texturepass, &m_spacebox1bighp, 100 );
    //m_scenenodegraph_hyperspace.AddNode( m_spacebox1bighp.transfo_node );
    m_scenenodegraph.AddNode( m_spacebox1bighp.transfo_node );


    init_hpspacebox( "spacebox2bighp", m_texturepass, &m_spacebox2bighp, 100 );
    //m_scenenodegraph_hyperspace.AddNode( m_spacebox2bighp.transfo_node );
    m_scenenodegraph.AddNode( m_spacebox2bighp.transfo_node );


    


    ///////////////////////////////////////////////////////////////






    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();

    //m_finalpass_hyperspace->GetRenderingQueue()->UpdateOutputQueue();
    //m_texturepass_hyperspace->GetRenderingQueue()->UpdateOutputQueue();


    
    m_freemove.Init( DrawSpace::Utils::Vector( 265000000.0, 0.0, 50.0, 1.0 ) );

    m_freemove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FreeMovement>, SceneNode<DrawSpace::Core::FreeMovement>( "free_node" ) );
    m_freemove_node->SetContent( &m_freemove );

    m_scenenodegraph.AddNode( m_freemove_node );
    m_scenenodegraph.RegisterNode( m_freemove_node );
    
    m_camera_node->LinkTo( m_freemove_node );



    m_mouse_circularmode = true;


    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );

    m_calendar->RegisterWorld( &m_world );
    m_calendar->RegisterWorld( m_planet->GetWorld() );
    m_calendar->RegisterWorld( m_moon->GetWorld() );

    m_calendar->RegisterOrbit( m_planet_orbit );
    m_calendar->RegisterOrbiter( m_planet );
    m_calendar->RegisterOrbit( m_moon_orbit );
    m_calendar->RegisterOrbiter( m_moon );


    
   
    m_scenenodegraph.SetCurrentCamera( "camera2" );
    m_curr_camera = m_camera2;

       

        
    m_planet->RegisterScenegraphCallbacks( m_scenenodegraph );
    m_moon->RegisterScenegraphCallbacks( m_scenenodegraph );

    ///////////////////////////////////////////////////////////////


    //m_camera5->LockOnBody( "cube", m_cube_body );
    //m_camera5->LockOnTransformNode( "cube", m_cube );
    m_camera5->Lock( m_cube_body_node );





    ///////////////////////////////////////////////////////////////


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

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyUpPitch( 50000.0 );
            }
            break;

        case 'C':

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyDownPitch( 50000.0 );
            }
            break;

        case 'S':

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyLeftYaw( 50000.0 );
            }
            break;

        case 'F':

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyRightYaw( 50000.0 );
            }
            break;


        case 'Z':

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyLeftRoll( 50000.0 );
            }
            break;

        case 'R':

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyRightRoll( 50000.0 );
            }
            break;

        case 'T':

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyFwdForce( 51000000.0 );
            }
            break;


        case 'M':
            if( m_hp_state == HP_NONE )
            {
                m_ship->ZeroASpeed();
            }
            break;


        case 'L':
            if( m_hp_state == HP_NONE )
            {
                m_ship->ZeroLSpeed();
            }
            break;


        case VK_SPACE:

            /*
            if( m_hp_state == HP_NONE )
            {
                m_ship->ZeroSpeed();
            }
            */
            break;

        case VK_RETURN:

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyFwdForce( 110000.0 );
            }
            break;

        case VK_UP:

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyFwdForce( -30000.0 );
            }
            break;

        case VK_DOWN:

            if( m_hp_state == HP_NONE )
            {
                m_ship->ApplyDownForce( -10000.0 );
            }
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

        case VK_F7:

            
            if( m_draw_hyperspace )
            {
                m_draw_hyperspace = false;
            }
            else
            {
                m_draw_hyperspace = true;
            }
            
            //m_finalpass->GetViewportQuad()->SetDrawingState( false );

            break;


        case VK_F8:
            {
                Matrix mat;   
                mat.Translation( 269000000.0, 0.0, 9000000.0 );
                m_ship->ForceInitialAttitude( mat );
            }

            break;


        case VK_F9:

            if( m_curr_camera == m_camera )
            {
                m_scenenodegraph.SetCurrentCamera( "camera2" );
                m_curr_camera = m_camera2;
            }

            else if( m_curr_camera == m_camera2 )
            {
                m_scenenodegraph.SetCurrentCamera( "camera3" );
                m_curr_camera = m_camera3;
            }
            else if( m_curr_camera == m_camera3 )
            {
                m_scenenodegraph.SetCurrentCamera( "camera4" );
                m_curr_camera = m_camera4;
            }
            else if( m_curr_camera == m_camera4 )
            {
                m_scenenodegraph.SetCurrentCamera( "camera5" );
                m_curr_camera = m_camera5;
            }
            else if( m_curr_camera == m_camera5 )
            {
                m_scenenodegraph.SetCurrentCamera( "camera6" );
                m_curr_camera = m_camera6;
            }
            else if( m_curr_camera == m_camera6 )
            {
                m_scenenodegraph.SetCurrentCamera( "camera7" );
                m_curr_camera = m_camera7;
            }
            else if( m_curr_camera == m_camera7 )
            {
                m_scenenodegraph.SetCurrentCamera( "camera8" );
                m_curr_camera = m_camera8;
            }
            else if( m_curr_camera == m_camera8 )
            {
                m_scenenodegraph.SetCurrentCamera( "camera" );
                m_curr_camera = m_camera;
            }
            break;


        case 'H':

            if( !m_hp && m_hp_state == HP_NONE )
            {
                m_hp = true;
                m_hp_state = HP_ACCELERATE;
            }
            else
            {
                if( m_hp_state == HP_CRUISE )
                {
                    m_hp = false;
                    m_hp_state = HP_OUT;
                    m_hp_transition->SetDrawingState( m_texturepass, true );
                }
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
