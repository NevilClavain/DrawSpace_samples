#include "dsappclient.h"
#include <algorithm>


using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_speed( 0.0 ), m_speed_speed( 5.0 ), m_current_camera( NULL ), m_selected_camera( 0 ), m_recompute_count( 0 ), m_ready( false ), m_sort_running_copy( false )
{    
    _INIT_LOGGER( "logclouds.conf" )  
    m_w_title = "clouds test";

    m_nodesevent_cb = _DRAWSPACE_NEW_( NodesEventCallback, NodesEventCallback( this, &dsAppClient::on_nodes_event ) );

    

    m_update_clouds_meshes = false;
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::on_sort_request( DrawSpace::Core::PropertyPool* p_args )
{

    Matrix ImpostorMat, CamMat;
    ImpostorMat = p_args->GetPropValue<Matrix>( "ImpostorMat" );
    CamMat = p_args->GetPropValue<Matrix>( "CamMat" );

    m_sort_run_mutex.WaitInfinite();

    clouds_execsortz( ImpostorMat, CamMat );

    clouds_impostors_init();

    

    m_mutex.WaitInfinite();

    m_update_clouds_meshes = true;

    m_mutex.Release();

    m_sort_run_mutex.Release();
}

void dsAppClient::on_nodes_event( DrawSpace::Core::SceneNodeGraph::NodesEvent p_event, DrawSpace::Core::BaseSceneNode* p_node )
{


}

void dsAppClient::on_camera_event( DrawSpace::Core::SceneNodeGraph::CameraEvent p_event, DrawSpace::Core::BaseSceneNode* p_node )
{
    if( DrawSpace::Core::SceneNodeGraph::ACTIVE == p_event )
    {
        DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>* prec_camera = m_current_camera;        
        m_current_camera = static_cast< DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>* >( p_node );

        if( prec_camera != m_current_camera && m_ready )
        {

            Matrix CamMat;

            m_current_camera->GetFinalTransform( CamMat );

            Matrix ImpostorMat;

            m_impostor2_node->GetFinalTransform( ImpostorMat );

            
            PropertyPool props;

            props.AddPropValue<Matrix>( "ImpostorMat", ImpostorMat );
            props.AddPropValue<Matrix>( "CamMat", CamMat );


            m_sort_msg->PushMessage( props );
            m_recompute_count++;

            m_previous_camera_pos_avail = false;
        }
    }
}


void dsAppClient::clouds_impostors_init( void )
{
    m_idl.clear();

    for( size_t i = 0; i < m_clouds.size(); i++ )
    {
        //for( size_t j = 0; j < m_clouds[i].idl.size(); j++ )
        for( size_t j = 0; j < m_clouds[i]->idl.size(); j++ )
        {
            //m_idl.push_back( m_clouds[i].idl[j] );
            m_idl.push_back( m_clouds[i]->idl[j] );
        }
    }

    m_impostor2->ImpostorsInit( m_idl );
}

void dsAppClient::OnRenderFrame( void )
{
    Matrix cam2_pos;
    cam2_pos.Translation( 0.0, 1.0, 0.0 );
   
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
           
    m_scenenodegraph.ComputeTransformations( m_timer );



    m_mutex.WaitInfinite();
    if( m_update_clouds_meshes )
    {
        m_impostor2->GetMeshe()->UpdateVertices();
        m_update_clouds_meshes = false;
    }
    m_mutex.Release();


    
    m_texturepass->GetRenderingQueue()->Draw();
    m_texturepass2->GetRenderingQueue()->Draw();
    m_maskpass->GetRenderingQueue()->Draw();
    m_filterpass->GetRenderingQueue()->Draw();
    
    m_finalpass->GetRenderingQueue()->Draw();
    //m_finalpass2->GetRenderingQueue()->Draw();

    long current_fps = m_timer.GetFPS();
    renderer->DrawText( 0, 0, 0, 10, 35, "%d", current_fps );
    renderer->DrawText( 0, 0, 0, 10, 55, "%d", m_recompute_count );

    if( m_sort_running_copy )
    {
        renderer->DrawText( 0, 0, 0, 10, 85, "sorting..." );
    }


    renderer->FlipScreen();



    m_calendar->Run();

    m_freemove.SetSpeed( m_speed );

    m_ready = true;


    if( m_current_camera )
    {
        Matrix CamMat;
        
        m_current_camera->GetFinalTransform( CamMat );

        


        Vector current_camera_pos;

        current_camera_pos[0] = CamMat( 3, 0 );
        current_camera_pos[1] = CamMat( 3, 1 );
        current_camera_pos[2] = CamMat( 3, 2 );
        current_camera_pos[3] = 1.0;


        if( m_previous_camera_pos_avail )
        {
            Vector delta_cam_pos;

            delta_cam_pos[0] = current_camera_pos[0] - m_previous_camera_pos[0];
            delta_cam_pos[0] = current_camera_pos[1] - m_previous_camera_pos[1];
            delta_cam_pos[0] = current_camera_pos[2] - m_previous_camera_pos[2];
            delta_cam_pos[3] = 1.0;

            if( delta_cam_pos.Length() > 500.0 )
            {

                m_sort_running_copy = true;
                if( m_sort_run_mutex.Wait( 0 ) )
                {
                    m_sort_running_copy = m_sort_running;                     
                }
                m_sort_run_mutex.Release();

                if( !m_sort_running_copy )
                {
                    // get clouds node global transform
                    Matrix ImpostorMat;

                    m_impostor2_node->GetFinalTransform( ImpostorMat );

                    PropertyPool props;

                    props.AddPropValue<Matrix>( "ImpostorMat", ImpostorMat );
                    props.AddPropValue<Matrix>( "CamMat", CamMat );

                    m_sort_msg->PushMessage( props );
                    m_recompute_count++;

                    m_previous_camera_pos = current_camera_pos;
                }
                
            }

        }
        else
        {
            m_previous_camera_pos = current_camera_pos;
            m_previous_camera_pos_avail = true;
        }
    }
       
}


void dsAppClient::clouds_addcloud( dsreal p_xpos, dsreal p_zpos, Chunk::ImpostorsDisplayList& p_idl )
{
    Chunk::ImpostorsDisplayListEntry idle;
    Chunk::ImpostorsDisplayList idl;


    idle.width_scale = 1500.0;
    idle.height_scale = 1500.0;

    idle.u1 = 0.5;
    idle.v1 = 0.5;
    idle.u2 = 0.75;
    idle.v2 = 0.5;
    idle.u3 = 0.75;
    idle.v3 = 0.75;
    idle.u4 = 0.5;
    idle.v4 = 0.75;

    idle.localpos[0] = p_xpos;
    idle.localpos[1] = 0.0;
    idle.localpos[2] = p_zpos;
    
    idl.push_back( idle );



    idle.width_scale = 1700.0;
    idle.height_scale = 1700.0;

    idle.u1 = 0.75;
    idle.v1 = 0.75;
    idle.u2 = 1.0;
    idle.v2 = 0.75;
    idle.u3 = 1.0;
    idle.v3 = 1.0;
    idle.u4 = 0.75;
    idle.v4 = 1.0;

    idle.localpos[0] = p_xpos + 950.0;
    idle.localpos[1] = 90.0;
    idle.localpos[2] = p_zpos;
    
    idl.push_back( idle );




    idle.width_scale = 1400.0;
    idle.height_scale = 1400.0;

    idle.u1 = 0.25;
    idle.v1 = 0.75;
    idle.u2 = 0.5;
    idle.v2 = 0.75;
    idle.u3 = 0.5;
    idle.v3 = 1.0;
    idle.u4 = 0.25;
    idle.v4 = 1.0;

    idle.localpos[0] = p_xpos + 1750.0;
    idle.localpos[1] = 190.0;
    idle.localpos[2] = p_zpos - 700.0;
    
    idl.push_back( idle );





    idle.width_scale = 1700.0;
    idle.height_scale = 1700.0;

    idle.u1 = 0.25;
    idle.v1 = 0.75;
    idle.u2 = 0.5;
    idle.v2 = 0.75;
    idle.u3 = 0.5;
    idle.v3 = 1.0;
    idle.u4 = 0.25;
    idle.v4 = 1.0;

    idle.localpos[0] = p_xpos + 1750.0;
    idle.localpos[1] = 190.0;
    idle.localpos[2] = p_zpos + 700.0;
    
    idl.push_back( idle );




    idle.width_scale = 1800.0;
    idle.height_scale = 1800.0;

    idle.u1 = 0.75;
    idle.v1 = 0.0;
    idle.u2 = 1.0;
    idle.v2 = 0.0;
    idle.u3 = 1.0;
    idle.v3 = 0.25;
    idle.u4 = 0.75;
    idle.v4 = 0.25;

    idle.localpos[0] = p_xpos + 250.0;
    idle.localpos[1] = -60.0;
    idle.localpos[2] = p_zpos;
    
    idl.push_back( idle );


    idle.width_scale = 1800.0;
    idle.height_scale = 1800.0;

    idle.u1 = 0.75;
    idle.v1 = 0.0;
    idle.u2 = 1.0;
    idle.v2 = 0.0;
    idle.u3 = 1.0;
    idle.v3 = 0.25;
    idle.u4 = 0.75;
    idle.v4 = 0.25;

    idle.localpos[0] = p_xpos + 700.0;
    idle.localpos[1] = -60.0;
    idle.localpos[2] = p_zpos;
    
    idl.push_back( idle );

    /*
    Cloud cloud;

    cloud.idl = idl;
    cloud.pos[0] = p_xpos;
    cloud.pos[1] = 0.0;
    cloud.pos[2] = p_zpos;
    cloud.pos[3] = 1.0;

    m_clouds.push_back( cloud );
    */

    Cloud* cloud = new Cloud;

    cloud->idl = idl;
    cloud->pos[0] = p_xpos;
    cloud->pos[1] = 0.0;
    cloud->pos[2] = p_zpos;
    cloud->pos[3] = 1.0;

    m_clouds.push_back( cloud );

}


void dsAppClient::clouds_execsortz( const DrawSpace::Utils::Matrix& p_impostor_mat, const DrawSpace::Utils::Matrix& p_cam_mat )
{
    // compute all camera-space z-depth

    for( size_t i = 0; i < m_clouds.size(); i++ )
    {
        Matrix local_trans;
        //local_trans.Translation( m_clouds[i].pos );
        local_trans.Translation( m_clouds[i]->pos );

        Matrix final = local_trans * p_impostor_mat;

        Matrix cam_mat = p_cam_mat;

        Vector point_imp( 0.0, 0.0, 0.0, 1.0 );
        Vector point_cam( 0.0, 0.0, 0.0, 1.0 );

        Vector t_point_imp, t_point_cam;

        final.Transform( &point_imp, &t_point_imp );        
        cam_mat.Transform( &point_cam, &t_point_cam );

        // la distance entre l'impostor et le point camera;
        Vector dist_imp;
        
        dist_imp[0] = t_point_imp[0] - t_point_cam[0];
        dist_imp[1] = t_point_imp[1] - t_point_cam[1];
        dist_imp[2] = t_point_imp[2] - t_point_cam[2];
        dist_imp[3] = 1.0;

        //m_clouds[i].dist_to_cam = dist_imp.Length();
        m_clouds[i]->dist_to_cam = dist_imp.Length();
    }

    std::sort( m_clouds.begin(), m_clouds.end(), dsAppClient::clouds_nodes_comp );
}


//bool dsAppClient::clouds_nodes_comp( Cloud p_n1, Cloud p_n2 )
bool dsAppClient::clouds_nodes_comp( Cloud* p_n1, Cloud* p_n2 )
{
    //return ( p_n1.dist_to_cam > p_n2.dist_to_cam );
    return ( p_n1->dist_to_cam > p_n2->dist_to_cam );
}




bool dsAppClient::OnIdleAppInit( void )
{

    /////////////////////////////////////
           
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" ) );
    m_texturepass->Initialize();

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 145, 230, 230, 255 );




    m_texturepass2 = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass2" ) );

    m_texturepass2->Initialize();
    m_texturepass2->GetRenderingQueue()->EnableDepthClearing( true );
    m_texturepass2->GetRenderingQueue()->EnableTargetClearing( true );
    m_texturepass2->GetRenderingQueue()->SetTargetClearingColor( 145, 230, 230, 255 );


    m_maskpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "mask_pass" ) );

    m_maskpass->Initialize();
    m_maskpass->GetRenderingQueue()->EnableDepthClearing( true );
    m_maskpass->GetRenderingQueue()->EnableTargetClearing( true );
    m_maskpass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 0 );



    m_filterpass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "filter_pass" ) );
    m_filterpass->Initialize();
    m_filterpass->CreateViewportQuad();
    

    m_filterpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_filterpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "clouds_final.vsh", false ) ) );
    m_filterpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "clouds_final.psh", false ) ) );
    m_filterpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_filterpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();


    m_filterpass->GetViewportQuad()->SetTexture( m_maskpass->GetTargetTexture(), 0 );
    m_filterpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 1 );
    m_filterpass->GetViewportQuad()->SetTexture( m_texturepass2->GetTargetTexture(), 2 );



    //////////////////////////////////////////////////////////////

    m_finalpass = _DRAWSPACE_NEW_( FinalPass, FinalPass( "final_pass" ) );
    m_finalpass->Initialize();
    m_finalpass->CreateViewportQuad();
    m_finalpass->GetViewportQuad()->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 0 )->LoadFromFile();
    m_finalpass->GetViewportQuad()->GetFx()->GetShader( 1 )->LoadFromFile();
    

    m_finalpass->GetViewportQuad()->SetTexture( m_filterpass->GetTargetTexture(), 0 );


    m_ground = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );

    m_ground->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    m_ground->RegisterPassSlot( m_texturepass );
    m_ground->RegisterPassSlot( m_texturepass2 );
    m_ground->RegisterPassSlot( m_maskpass );


    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();
    m_ground->GetMeshe()->SetImporter( m_meshe_import );

    m_ground->GetMeshe()->LoadFromFile( "terran.ac", 0 );
    

    m_ground->GetNodeFromPass( m_texturepass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ground->GetNodeFromPass( m_texturepass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    

    m_ground->GetNodeFromPass( m_texturepass )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "mapcolor.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( m_texturepass )->GetTexture( 0 )->LoadFromFile();



    m_ground->GetNodeFromPass( m_texturepass2 )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "texture.psh", false ) ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    

    m_ground->GetNodeFromPass( m_texturepass2 )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "mapcolor.bmp" ) ), 0 );
    m_ground->GetNodeFromPass( m_texturepass2 )->GetTexture( 0 )->LoadFromFile();



    m_ground->GetNodeFromPass( m_maskpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.vsh", false ) ) );
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "color.psh", false ) ) );
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );    
    m_ground->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );

    m_ground->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "color", 0 );
    m_ground->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "color", Vector( 0.0, 0.0, 0.0, 0.0 ) );



    m_ground_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "ground" ) );
    m_ground_node->SetContent( m_ground );

    m_scenenodegraph.AddNode( m_ground_node );
    m_scenenodegraph.RegisterNode( m_ground_node );



    m_impostor2_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "impostor2_transfo" ) );
    m_impostor2_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

    Matrix impostor2_pos;
    impostor2_pos.Translation( 6.0, 1200.0, -450.0 );

    m_impostor2_transfo_node->GetContent()->PushMatrix( impostor2_pos );

    m_scenenodegraph.AddNode( m_impostor2_transfo_node );
    m_scenenodegraph.RegisterNode( m_impostor2_transfo_node );



    m_impostor2 = _DRAWSPACE_NEW_( DrawSpace::Chunk, DrawSpace::Chunk );
    m_impostor2->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );


    /////////////////////////////////////////////////



    dsreal cloudspos_x = 0.0;
    dsreal cloudspos_z = 0.0;

    for( long i = 0; i < 20; i++ )
    {
        cloudspos_z = 0.0;

        for( long j = 0; j < 20; j++ )
        {
            clouds_addcloud( cloudspos_x, cloudspos_z, m_idl );            

            cloudspos_z -= 5500.0;
        }

        cloudspos_x += 5500.0;
    }

    /////////////////////////////////////////////////



    
    clouds_impostors_init();

    m_impostor2->RegisterPassSlot( m_texturepass2 );
    m_impostor2->RegisterPassSlot( m_maskpass );

    
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.vsh", false ) ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.psh", false ) ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );
    

    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    

    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );





    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "clouds.bmp" ) ), 0 );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->GetTexture( 0 )->LoadFromFile();

    
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetOrderNumber( 12000 );


    m_impostor2->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 0, "flags_v", 24 );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "flags_v", Vector( 0.5, 0.0, 0.0, 0.0 ) );

    m_impostor2->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 0, "clouds_dims", 25 );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "clouds_dims", Vector( 900, -500, 1.0, 0.66 ) );



    m_impostor2->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 1, "flags", 0 );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "flags", Vector( 0.5, 0.0, 0.0, 0.0 ) );

    m_impostor2->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 1, "color", 1 );
    m_impostor2->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "color", Vector( 0.99, 0.99, 0.99, 1.0 ) );
    


    m_impostor2->GetNodeFromPass( m_maskpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.vsh", false ) ) );
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.psh", false ) ) );
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_impostor2->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    


    
    m_impostor2->GetNodeFromPass( m_maskpass )->SetOrderNumber( 12000 );


    m_impostor2->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "flags", 0 );
    m_impostor2->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "flags", Vector( 1.0, 0.0, 0.0, 0.0 ) );

    m_impostor2->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "color", 1 );
    m_impostor2->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );



    m_impostor2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Chunk>, SceneNode<DrawSpace::Chunk>( "impostor1" ) );

    m_impostor2_node->SetContent( m_impostor2 );


    m_scenenodegraph.RegisterNode( m_impostor2_node );

    m_impostor2_node->LinkTo( m_impostor2_transfo_node );


    

    //////////////////////////////////////////////////////////////


    m_world.Initialize();
    
    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();





    //////////////////////////////////////////////


    ////////////////////////////////////////////////////


    m_scenenodegraph.RegisterNodesEvtHandler( m_nodesevent_cb );

    
  

    m_calendar = _DRAWSPACE_NEW_( Calendar, Calendar( 0, &m_timer ) );
    m_calendar->RegisterWorld( &m_world );


    //////////////////////////////////////////////////////////////





    m_camera = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );

    
    m_camera_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera" ) );
    m_camera_node->SetContent( m_camera );
    m_scenenodegraph.RegisterNode( m_camera_node );


    m_freemove.Init( DrawSpace::Utils::Vector( 0.0, 5.0, 0.0, 1.0 ) );

    m_freemove_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Core::FreeMovement>, SceneNode<DrawSpace::Core::FreeMovement>( "free_node" ) );
    m_freemove_node->SetContent( &m_freemove );

    m_scenenodegraph.AddNode( m_freemove_node );
    m_scenenodegraph.RegisterNode( m_freemove_node );
    
    m_camera_node->LinkTo( m_freemove_node );



    m_camera2 = _DRAWSPACE_NEW_( DrawSpace::Dynamics::CameraPoint, DrawSpace::Dynamics::CameraPoint );

    
    m_camera2_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Dynamics::CameraPoint>, SceneNode<DrawSpace::Dynamics::CameraPoint>( "camera2" ) );
    m_camera2_node->SetContent( m_camera2 );
    m_scenenodegraph.RegisterNode( m_camera2_node );



    m_camera2_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "camera2_transfo" ) );
    m_camera2_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

    Matrix cam2_pos;
    cam2_pos.Translation( 20000.0, 9000.0, -60000.0 );

    m_camera2_transfo_node->GetContent()->PushMatrix( cam2_pos );

    m_scenenodegraph.AddNode( m_camera2_transfo_node );
    m_scenenodegraph.RegisterNode( m_camera2_transfo_node );






    m_camera2_node->LinkTo( m_camera2_transfo_node );


    m_camera2->Lock( m_impostor2_node );





    m_scenenodegraph.SetCurrentCamera( "camera" );


    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass2->GetRenderingQueue()->UpdateOutputQueue();
    m_maskpass->GetRenderingQueue()->UpdateOutputQueue();
    m_filterpass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();




    m_mouse_circularmode = true;


    m_sort_running = false;

    m_calendar->Startup( 0 );


    m_runner_msg_cb = _DRAWSPACE_NEW_( RunnerMsgCb, RunnerMsgCb( this, &dsAppClient::on_sort_request ) );

    m_runner = _DRAWSPACE_NEW_( Runner, Runner );
    DrawSpace::Core::Mediator* mediator = Mediator::GetInstance();
    m_sort_msg = mediator->CreateMessageQueue( "ReqSortEvent" );
    
    m_runner->RegisterMsgHandler( m_sort_msg, m_runner_msg_cb );

    m_task = _DRAWSPACE_NEW_( Task<Runner>, Task<Runner> );
    m_task->Startup( m_runner );

    m_previous_camera_pos_avail = false;

    
    m_cameraevent_cb = _DRAWSPACE_NEW_( CameraEventCb, CameraEventCb( this, &dsAppClient::on_camera_event ) );

    m_scenenodegraph.RegisterCameraEvtHandler( m_cameraevent_cb );
        
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

            /*
            if( 0.0 == m_speed )
            {
                m_speed = 1.0;
            }
            else
            {
                m_speed *= 1.081;
            }
            */

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
    }
}

void dsAppClient::OnKeyPulse( long p_key )
{
    switch( p_key )
    {
        case VK_F1:

        
            break;

        case VK_F2:
            {              
            }        
            break;

        case VK_F3:

            if( 0 == m_selected_camera )
            {
                m_scenenodegraph.SetCurrentCamera( "camera2" );
                m_selected_camera = 1;
            }
            else
            {
                m_scenenodegraph.SetCurrentCamera( "camera" );
                m_selected_camera = 0;
            }
            break;

        case VK_F4:

            break;

        case VK_F5:


            break;

        case VK_F6:


            break;

        case VK_F7:

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