#include "dsappclient.h"
#include <algorithm>

//#include <random>

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;
using namespace DrawSpace::Gui;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;


dsAppClient* dsAppClient::m_instance = NULL;




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false ), m_speed( 0.0 ), m_speed_speed( 5.0 ), m_current_camera( NULL ), m_selected_camera( 0 )
{    
    _INIT_LOGGER( "logclouds.conf" )  
    m_w_title = "clouds test";
}

dsAppClient::~dsAppClient( void )
{

}


void dsAppClient::OnRenderFrame( void )
{
    Matrix cam2_pos;
    cam2_pos.Translation( 0.0, 1.0, 0.0 );
   
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
           
    m_scenenodegraph.ComputeTransformations( m_timer );



   

    
    m_texturepass->GetRenderingQueue()->Draw();
    m_texturepass2->GetRenderingQueue()->Draw();
    m_maskpass->GetRenderingQueue()->Draw();
    m_filterpass->GetRenderingQueue()->Draw();
    
    m_finalpass->GetRenderingQueue()->Draw();    

    long current_fps = m_timer.GetFPS();
    renderer->DrawText( 0, 0, 0, 10, 35, "%d", current_fps );
    //renderer->DrawText( 0, 0, 0, 10, 55, "%d", m_recompute_count );



    renderer->FlipScreen();



    m_calendar->Run();

    m_freemove.SetSpeed( m_speed );

       
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




    m_clouds_transfo_node = _DRAWSPACE_NEW_( DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>, DrawSpace::Core::SceneNode<DrawSpace::Core::Transformation>( "impostor2_transfo" ) );
    m_clouds_transfo_node->SetContent( _DRAWSPACE_NEW_( Transformation, Transformation ) );

    Matrix clouds_pos;
    clouds_pos.Translation( 6.0, 1200.0, -450.0 );

    m_clouds_transfo_node->GetContent()->PushMatrix( clouds_pos );

    m_scenenodegraph.AddNode( m_clouds_transfo_node );
    m_scenenodegraph.RegisterNode( m_clouds_transfo_node );



    m_clouds = _DRAWSPACE_NEW_( DrawSpace::Clouds, DrawSpace::Clouds );
    m_clouds->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );

    m_clouds->EnableDetails( false );

    /////////////////////////////////////////////////

   
    //// declare cloud

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_cloudposx = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>( new std::uniform_real_distribution<dsreal>( -60000.0, 60000.0 ), 222 );

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_cloudposz = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, DrawSpace::Procedural::Real>( new std::uniform_real_distribution<dsreal>( -60000.0, 60000.0 ), 53432 );


    Procedural::Array* declare_cloud_array = new Procedural::Array;

    Procedural::String* declare_cloud_string = new Procedural::String;
    declare_cloud_string->SetValue( "declare_cloud" );

    declare_cloud_array->AddValue( declare_cloud_string );
    declare_cloud_array->AddValue( rand_cloudposx );
    declare_cloud_array->AddValue( rand_cloudposz );
    

    DrawSpace::Procedural::Publisher* pub_declare_cloud = new DrawSpace::Procedural::Publisher;
    pub_declare_cloud->SetChild( declare_cloud_array );
    pub_declare_cloud->RegisterHandler( m_clouds->GetProceduralCallback() );
    ////

    //// add core cloud impostors


    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_core_impostor_posx = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>( new std::uniform_real_distribution<dsreal>( -600.0, 600.0 ), 9915 );

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_core_impostor_posz = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, DrawSpace::Procedural::Real>( new std::uniform_real_distribution<dsreal>( -600.0, 600.0 ), 40090 );

    Procedural::Vector* uvcloud_core0 = new Procedural::Vector;
    uvcloud_core0->SetValue( Utils::Vector( 0.0, 0.0, 0.25, 0.25 ) );

    Procedural::Vector* uvcloud_core1 = new Procedural::Vector;
    uvcloud_core1->SetValue( Utils::Vector( 0.25, 0.0, 0.5, 0.25 ) );

    Procedural::Vector* uvcloud_core2 = new Procedural::Vector;
    uvcloud_core2->SetValue( Utils::Vector( 0.5, 0.0, 0.75, 0.25 ) );


    Procedural::Array* uvcloud_core = new Procedural::Array;
    uvcloud_core->AddValue( uvcloud_core0 );
    uvcloud_core->AddValue( uvcloud_core1 );
    uvcloud_core->AddValue( uvcloud_core2 );


    Procedural::Index* uvcloud_core_index = new Procedural::Index;
    uvcloud_core_index->SetArray( uvcloud_core );

    Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>* rand_cloudcore_uv = 
        new Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>( new std::uniform_int_distribution<int>( 0, 2 ), 27764 );

    uvcloud_core_index->SetIndex( rand_cloudcore_uv );



    Procedural::Real* core_scale = new Procedural::Real;
    core_scale->SetValue( 600.0 );


    Procedural::Array* add_core_impostor_array = new Procedural::Array;

    Procedural::String* add_core_impostor_string = new Procedural::String;
    add_core_impostor_string->SetValue( "add_core_impostor" );

    DrawSpace::Procedural::Publisher* pub_add_core_impostor = new DrawSpace::Procedural::Publisher;
    pub_add_core_impostor->SetChild( add_core_impostor_array );
    pub_add_core_impostor->RegisterHandler( m_clouds->GetProceduralCallback() );

        

    add_core_impostor_array->AddValue( add_core_impostor_string );        
    add_core_impostor_array->AddValue( rand_core_impostor_posx );    
    add_core_impostor_array->AddValue( rand_core_impostor_posz );
    add_core_impostor_array->AddValue( uvcloud_core_index );
    add_core_impostor_array->AddValue( core_scale );


    Procedural::Integer* nb_core_impostors = new Procedural::Integer;
    nb_core_impostors->SetValue( 20 );
    

    
    Procedural::Repeat* cloud_core_loop = new Procedural::Repeat;
    cloud_core_loop->SetChild( pub_add_core_impostor );
    cloud_core_loop->SetNbLoops( nb_core_impostors );



    ////

    //// add bottom cloud impostors

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_bottom_impostor_posx = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>( new std::uniform_real_distribution<dsreal>( -800.0, 800.0 ), 855453 );

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_bottom_impostor_posz = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, DrawSpace::Procedural::Real>( new std::uniform_real_distribution<dsreal>( -800.0, 800.0 ), 237 );

    Procedural::Vector* uv_bottom = new Procedural::Vector;
    uv_bottom->SetValue( Utils::Vector( 0.75, 0.0, 1.0, 0.25 ) );

    Procedural::Real* y_pos = new Procedural::Real;
    y_pos->SetValue( -150.0 );

    Procedural::Real* bottom_scale = new Procedural::Real;
    bottom_scale->SetValue( 800.0 );


    Procedural::Array* add_bottom_impostor_array = new Procedural::Array;

    Procedural::String* add_bottom_impostor_string = new Procedural::String;
    add_bottom_impostor_string->SetValue( "add_bottom_impostor" );

    DrawSpace::Procedural::Publisher* pub_add_bottom_impostor = new DrawSpace::Procedural::Publisher;
    pub_add_bottom_impostor->SetChild( add_bottom_impostor_array );
    pub_add_bottom_impostor->RegisterHandler( m_clouds->GetProceduralCallback() );

        

    add_bottom_impostor_array->AddValue( add_bottom_impostor_string );        
    add_bottom_impostor_array->AddValue( rand_bottom_impostor_posx );
    add_bottom_impostor_array->AddValue( y_pos );
    add_bottom_impostor_array->AddValue( rand_bottom_impostor_posz );
    add_bottom_impostor_array->AddValue( uv_bottom );
    add_bottom_impostor_array->AddValue( bottom_scale );


    

    Procedural::Array* nb_bottom_impostors_array = new Procedural::Array;
    Procedural::Integer* nb_bottom_impostors = new Procedural::Integer;
    nb_bottom_impostors->SetValue( 40 );

    Procedural::Integer* no_bottom_impostors = new Procedural::Integer;
    no_bottom_impostors->SetValue( 0 );

    nb_bottom_impostors_array->AddValue( no_bottom_impostors );
    nb_bottom_impostors_array->AddValue( nb_bottom_impostors );
    
    Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>* rand_nb_bottom_impostors = 
    new Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>( new std::uniform_int_distribution<int>( 0, 1 ), 91123 );

    Procedural::Index* nb_bottom_impostors_index = new Procedural::Index;

    nb_bottom_impostors_index->SetArray( nb_bottom_impostors_array );
    nb_bottom_impostors_index->SetIndex( rand_nb_bottom_impostors );

    
    Procedural::Repeat* cloud_bottom_loop = new Procedural::Repeat;
    cloud_bottom_loop->SetChild( pub_add_bottom_impostor );
    cloud_bottom_loop->SetNbLoops( nb_bottom_impostors_index );



    //// add cloud impostors

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posx = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>( new std::uniform_real_distribution<dsreal>( -600.0, 600.0 ), 133212 );

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posy = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, DrawSpace::Procedural::Real>( new std::uniform_real_distribution<dsreal>( -100.0, 200.0 ), 8766 );

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posz = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, DrawSpace::Procedural::Real>( new std::uniform_real_distribution<dsreal>( -600.0, 600.0 ), 73322 );


    Procedural::Vector* uvcloud_main0 = new Procedural::Vector;
    uvcloud_main0->SetValue( Utils::Vector( 0.5, 0.5, 0.75, 0.75 ) );

    Procedural::Vector* uvcloud_main1 = new Procedural::Vector;
    uvcloud_main1->SetValue( Utils::Vector( 0.75, 0.5, 1.0, 0.75 ) );

    Procedural::Vector* uvcloud_main2 = new Procedural::Vector;
    uvcloud_main2->SetValue( Utils::Vector( 0.0, 0.75, 0.25, 1.0 ) );

    Procedural::Vector* uvcloud_main3 = new Procedural::Vector;
    uvcloud_main3->SetValue( Utils::Vector( 0.25, 0.75, 0.5, 1.0 ) );

    Procedural::Vector* uvcloud_main4 = new Procedural::Vector;
    uvcloud_main4->SetValue( Utils::Vector( 0.5, 0.75, 0.75, 1.0 ) );

    Procedural::Vector* uvcloud_main5 = new Procedural::Vector;
    uvcloud_main5->SetValue( Utils::Vector( 0.75, 0.75, 1.0, 1.0 ) );

    Procedural::Array* uvcloud_main = new Procedural::Array;
    uvcloud_main->AddValue( uvcloud_main0 );
    uvcloud_main->AddValue( uvcloud_main1 );
    uvcloud_main->AddValue( uvcloud_main2 );
    uvcloud_main->AddValue( uvcloud_main3 );
    uvcloud_main->AddValue( uvcloud_main4 );
    uvcloud_main->AddValue( uvcloud_main5 );

    Procedural::Index* uvcloud_main_index = new Procedural::Index;
    uvcloud_main_index->SetArray( uvcloud_main );

    Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>* rand_cloudmain_uv = 
        new Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>( new std::uniform_int_distribution<int>( 0, 5 ), 27764 );

    uvcloud_main_index->SetIndex( rand_cloudmain_uv );


    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostorscale = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>( new std::uniform_real_distribution<dsreal>( 200.0, 800.0 ), 996 );


    Procedural::Array* add_impostor_array = new Procedural::Array;

    Procedural::String* add_impostor_string = new Procedural::String;
    add_impostor_string->SetValue( "add_impostor" );

    DrawSpace::Procedural::Publisher* pub_add_impostor = new DrawSpace::Procedural::Publisher;
    pub_add_impostor->SetChild( add_impostor_array );
    pub_add_impostor->RegisterHandler( m_clouds->GetProceduralCallback() );

        

    add_impostor_array->AddValue( add_impostor_string );        
    add_impostor_array->AddValue( uvcloud_main_index );
    add_impostor_array->AddValue( rand_impostorscale );
    add_impostor_array->AddValue( rand_impostor_posx );
    add_impostor_array->AddValue( rand_impostor_posy );
    add_impostor_array->AddValue( rand_impostor_posz );


    Procedural::Repeat* cloud_loop = new Procedural::Repeat;

    Procedural::Integer* nb_impostors = new Procedural::Integer;
    nb_impostors->SetValue( 7 );
    cloud_loop->SetChild( pub_add_impostor );
    cloud_loop->SetNbLoops( nb_impostors );


    ////



    //// add fragment cloud impostors

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_frag_impostor_posx = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>( new std::uniform_real_distribution<dsreal>( -2600.0, 2600.0 ), 7553 );

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_frag_impostor_posy = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, DrawSpace::Procedural::Real>( new std::uniform_real_distribution<dsreal>( 180.0, 400.0 ), 10998 );

    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_frag_impostor_posz = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, DrawSpace::Procedural::Real>( new std::uniform_real_distribution<dsreal>( -2600.0, 2600.0 ), 17800 );


    Procedural::Vector* uvcloud_frag0 = new Procedural::Vector;
    uvcloud_frag0->SetValue( Utils::Vector( 0.0, 0.25, 0.25, 0.5 ) );

    Procedural::Vector* uvcloud_frag1 = new Procedural::Vector;
    uvcloud_frag1->SetValue( Utils::Vector( 0.25, 0.25, 0.5, 0.5 ) );

    Procedural::Vector* uvcloud_frag2 = new Procedural::Vector;
    uvcloud_frag2->SetValue( Utils::Vector( 0.5, 0.25, 0.75, 0.5 ) );

    Procedural::Vector* uvcloud_frag3 = new Procedural::Vector;
    uvcloud_frag3->SetValue( Utils::Vector( 0.75, 0.25, 1.0, 0.5 ) );

    Procedural::Vector* uvcloud_frag4 = new Procedural::Vector;
    uvcloud_frag4->SetValue( Utils::Vector( 0.0, 0.5, 0.25, 0.75 ) );

    Procedural::Vector* uvcloud_frag5 = new Procedural::Vector;
    uvcloud_frag5->SetValue( Utils::Vector( 0.25, 0.5, 0.5, 0.75 ) );


    Procedural::Array* uvcloud_frag = new Procedural::Array;
    uvcloud_frag->AddValue( uvcloud_frag0 );
    uvcloud_frag->AddValue( uvcloud_frag1 );
    uvcloud_frag->AddValue( uvcloud_frag2 );
    uvcloud_frag->AddValue( uvcloud_frag3 );
    uvcloud_frag->AddValue( uvcloud_frag4 );
    uvcloud_frag->AddValue( uvcloud_frag5 );

    Procedural::Index* uvcloud_frag_index = new Procedural::Index;
    uvcloud_frag_index->SetArray( uvcloud_frag );

    Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>* rand_cloudfrag_uv = 
        new Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>( new std::uniform_int_distribution<int>( 0, 5 ), 64099 );

    uvcloud_frag_index->SetIndex( rand_cloudfrag_uv );


    Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_fragimpostorscale = 
        new Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>( new std::uniform_real_distribution<dsreal>( 100.0, 500.0 ), 18776 );



    Procedural::Array* add_fragimpostor_array = new Procedural::Array;


    DrawSpace::Procedural::Publisher* pub_add_fragimpostor = new DrawSpace::Procedural::Publisher;
    pub_add_fragimpostor->SetChild( add_fragimpostor_array );
    pub_add_fragimpostor->RegisterHandler( m_clouds->GetProceduralCallback() );

        

    add_fragimpostor_array->AddValue( add_impostor_string );        
    add_fragimpostor_array->AddValue( uvcloud_frag_index );
    add_fragimpostor_array->AddValue( rand_fragimpostorscale );
    add_fragimpostor_array->AddValue( rand_frag_impostor_posx );
    add_fragimpostor_array->AddValue( rand_frag_impostor_posy );
    add_fragimpostor_array->AddValue( rand_frag_impostor_posz );


    

    Procedural::Array* nb_frag_impostors_array = new Procedural::Array;

    Procedural::Integer* nb_frag_impostors = new Procedural::Integer;
    nb_frag_impostors->SetValue( 12 );

    Procedural::Integer* no_frag_impostors = new Procedural::Integer;
    no_frag_impostors->SetValue( 0 );

    nb_frag_impostors_array->AddValue( no_frag_impostors );
    nb_frag_impostors_array->AddValue( no_frag_impostors );
    nb_frag_impostors_array->AddValue( no_frag_impostors );
    nb_frag_impostors_array->AddValue( nb_frag_impostors );

    Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>* rand_nb_frag_impostors = 
    new Procedural::RandomDistribution<int, std::uniform_int_distribution<int>, Procedural::Integer>( new std::uniform_int_distribution<int>( 0, 3 ), 10899 );

    Procedural::Index* nb_frag_impostors_index = new Procedural::Index;

    nb_frag_impostors_index->SetArray( nb_frag_impostors_array );
    nb_frag_impostors_index->SetIndex( rand_nb_frag_impostors );

    Procedural::Repeat* cloud_frag_loop = new Procedural::Repeat;
    cloud_frag_loop->SetChild( pub_add_fragimpostor );
    cloud_frag_loop->SetNbLoops( nb_frag_impostors_index );


    ////



    //// push cloud
    Procedural::Array* push_cloud_array = new Procedural::Array;

    Procedural::String* push_cloud_string = new Procedural::String;
    push_cloud_string->SetValue( "push_cloud" );

    push_cloud_array->AddValue( push_cloud_string );

    DrawSpace::Procedural::Publisher* pub_push_cloud = new DrawSpace::Procedural::Publisher;
    pub_push_cloud->SetChild( push_cloud_array );
    pub_push_cloud->RegisterHandler( m_clouds->GetProceduralCallback() );
    /////


    //// root
    Procedural::Batch* main_task = new Procedural::Batch;
    main_task->AddChild( pub_declare_cloud );
    main_task->AddChild( cloud_core_loop );
    main_task->AddChild( cloud_bottom_loop );
    main_task->AddChild( cloud_loop );
    main_task->AddChild( cloud_frag_loop );
    main_task->AddChild( pub_push_cloud );

    Procedural::Integer* nb_clouds = new Procedural::Integer;
    nb_clouds->SetValue( 320 );

    Procedural::Repeat* main_loop = new Procedural::Repeat;
    main_loop->SetChild( main_task );
    main_loop->SetNbLoops( nb_clouds );

    main_loop->Apply();

    /////////////////////////////////////////////////
    

    m_clouds->ImpostorsInit();
    



    m_clouds->RegisterPassSlot( m_texturepass2 );
    m_clouds->RegisterPassSlot( m_maskpass );

    
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.vsh", false ) ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.psh", false ) ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->GetShader( 1 )->LoadFromFile();

    
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "true" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDOP, "add" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDFUNC, "always" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDDEST, "invsrcalpha" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDSRC, "srcalpha" ) );
    

    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    

    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ALPHABLENDENABLE, "false" ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );





    m_clouds->GetNodeFromPass( m_texturepass2 )->SetTexture( _DRAWSPACE_NEW_( Texture, Texture( "clouds.bmp" ) ), 0 );
    m_clouds->GetNodeFromPass( m_texturepass2 )->GetTexture( 0 )->LoadFromFile();

    
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetOrderNumber( 12000 );


    m_clouds->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 0, "flags_v", 24 );
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "flags_v", Vector( 0.5, 0.0, 0.0, 0.0 ) );

    m_clouds->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 0, "clouds_dims", 25 );
    //m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "clouds_dims", Vector( 900, -500, 1.0, 0.66 ) );
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "clouds_dims", Vector( 400, -200, 1.0, 0.76 ) );



    m_clouds->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 1, "flags", 0 );
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "flags", Vector( 0.5, 0.0, 0.0, 0.0 ) );

    m_clouds->GetNodeFromPass( m_texturepass2 )->AddShaderParameter( 1, "color", 1 );
    m_clouds->GetNodeFromPass( m_texturepass2 )->SetShaderRealVector( "color", Vector( 0.99, 0.99, 0.99, 1.0 ) );
    


    m_clouds->GetNodeFromPass( m_maskpass )->SetFx( _DRAWSPACE_NEW_( Fx, Fx ) );
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.vsh", false ) ) );
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "spaceimpostor.psh", false ) ) );
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 0 )->LoadFromFile();
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->GetShader( 1 )->LoadFromFile();

    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_clouds->GetNodeFromPass( m_maskpass )->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    


    
    m_clouds->GetNodeFromPass( m_maskpass )->SetOrderNumber( 12000 );


    m_clouds->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "flags", 0 );
    m_clouds->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "flags", Vector( 1.0, 0.0, 0.0, 0.0 ) );

    m_clouds->GetNodeFromPass( m_maskpass )->AddShaderParameter( 1, "color", 1 );
    m_clouds->GetNodeFromPass( m_maskpass )->SetShaderRealVector( "color", Vector( 1.0, 1.0, 1.0, 1.0 ) );



    m_clouds_node = _DRAWSPACE_NEW_( SceneNode<DrawSpace::Clouds>, SceneNode<DrawSpace::Clouds>( "clouds_1" ) );

    m_clouds_node->SetContent( m_clouds );


    m_scenenodegraph.RegisterNode( m_clouds_node );

    m_clouds_node->LinkTo( m_clouds_transfo_node );




    //////////////////////////////////////////////////////////////


    m_world.Initialize();
    
    m_meshe_import = new DrawSpace::Utils::AC3DMesheImport();





    //////////////////////////////////////////////


    ////////////////////////////////////////////////////

  

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



    m_camera2->Lock( m_clouds_node );

    m_scenenodegraph.SetCurrentCamera( "camera" );


    m_texturepass->GetRenderingQueue()->UpdateOutputQueue();
    m_texturepass2->GetRenderingQueue()->UpdateOutputQueue();
    m_maskpass->GetRenderingQueue()->UpdateOutputQueue();
    m_filterpass->GetRenderingQueue()->UpdateOutputQueue();
    
    m_finalpass->GetRenderingQueue()->UpdateOutputQueue();




    m_mouse_circularmode = true;



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
