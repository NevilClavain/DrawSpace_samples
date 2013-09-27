
#include "dsappclient.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;
using namespace DrawSpace::Core;

dsAppClient* dsAppClient::m_instance = NULL;


SceneTransform::SceneTransform( void ) : m_xangle( 0.0 ), m_yangle( 0.0 ), m_zpos( -8.0 )
{

}

SceneTransform::~SceneTransform( void )
{


}

void SceneTransform::Transform( void )
{
	DrawSpace::Utils::Matrix result;

	DrawSpace::Utils::Matrix translate;
	
	m_mutex_angles.WaitInfinite();
	translate.Translation( 0.0, 0.0, m_zpos );
	m_mutex_angles.Release();

	DrawSpace::Utils::Matrix yrotate;

	m_mutex_z.WaitInfinite();
	yrotate.Rotation( DrawSpace::Utils::Vector( 0.0, 1.0, 0.0, 1.0 ), 3.1415927 * m_yangle / 180.0 );
	DrawSpace::Utils::Matrix xrotate;
	xrotate.Rotation( DrawSpace::Utils::Vector( 1.0, 0.0, 0.0, 1.0 ), 3.1415927 * m_xangle / 180.0 );
	m_mutex_z.Release();

	DrawSpace::Utils::Transformation transform;

	transform.PushMatrix( translate );
	transform.PushMatrix( yrotate );
	transform.PushMatrix( xrotate );
	transform.BuildResult();
	transform.GetResult( &result );


	m_mutex_result.WaitInfinite();
	m_result = result;
	m_mutex_result.Release();
}

void SceneTransform::Run( void )
{
	while( 1 )
	{
		Transform();
		//Sleep( 1 );
	}
}


void SceneTransform::OnLeftDrag( DrawSpace::Utils::TimeManager& p_timer, long p_dx, long p_dy )
{
	m_mutex_angles.WaitInfinite();
	p_timer.AngleSpeedInc( &m_yangle, p_dx * 150.0 );
	p_timer.AngleSpeedInc( &m_xangle, p_dy * 150.0 );
	m_mutex_angles.Release();
}

void SceneTransform::OnRightDrag( DrawSpace::Utils::TimeManager& p_timer, long p_dx, long p_dy )
{
	m_mutex_z.WaitInfinite();
	p_timer.TranslationSpeedInc( &m_zpos, p_dy * 40.0 );
	m_mutex_z.Release();
}

void SceneTransform::GetResult( DrawSpace::Utils::Matrix& p_out )
{
	m_mutex_result.WaitInfinite();
	p_out = m_result;
	m_mutex_result.Release();
}




dsAppClient::dsAppClient( void ) : m_mouselb( false ), m_mouserb( false )
{    
    _INIT_LOGGER( "fog.conf" )  
	m_w_title = "fog test";
}

dsAppClient::~dsAppClient( void )
{

}

void dsAppClient::OnRenderFrame( void )
{
	DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::Plugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

	DrawSpace::Utils::Matrix result;
	m_transform->GetResult( result );

    m_scenegraph.SetNodeLocalTransformation( "cube", result );

    m_scenegraph.ComputeTransformations();

	static long last_fps;

	long current_fps = m_timer.GetFPS();
	char fps[256];
	sprintf( fps, "%d"
		" fps", m_timer.GetFPS() );
	if( last_fps != current_fps )
	{
		m_fpstext_widget->SetText( 0, 0, 70, fps, DrawSpace::Text::HorizontalCentering | DrawSpace::Text::VerticalCentering );
		last_fps = current_fps;		
	}


	m_fpstext_widget->SetVirtualTranslation( 15, 5 );



	m_texturepass->GetRenderingQueue()->Draw();
	m_fogintpass->GetRenderingQueue()->Draw();
	
	m_fogblendpass->GetRenderingQueue()->Draw();

	m_fpstext_widget->Draw();

	m_finalpass->GetRenderingQueue()->Draw();

	renderer->FlipScreen();

	m_timer.Update();
	if( m_timer.IsReady() )
	{		
	}
}

bool dsAppClient::OnIdleAppInit( void )
{
	bool status;

	Shader* vertex_shader;
	Shader* pixel_shader;

	DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::Plugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
	renderer->SetRenderState( &DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETCULLING, "cw" ) );


    //m_texturepass = new IntermediatePass( "texture_pass" );
	m_texturepass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( "texture_pass" )

    m_texturepass->GetRenderingQueue()->EnableDepthClearing( true );
	m_texturepass->GetRenderingQueue()->EnableTargetClearing( true );
	m_texturepass->GetRenderingQueue()->SetTargetClearingColor( 145, 230, 230 );

    //////////////////////////////////////////////////////////////

    m_fogintpass = new IntermediatePass( "fogint_pass" );
    m_fogintpass->GetRenderingQueue()->EnableDepthClearing( true );
	m_fogintpass->GetRenderingQueue()->EnableTargetClearing( true );
	m_fogintpass->GetRenderingQueue()->SetTargetClearingColor( 255, 0, 0 );


	//////////////////////////////////////////////////////////////

	vertex_shader = new Shader( "fogblend.vsh", false );
	pixel_shader = new Shader( "fogblend.psh", false );

	vertex_shader->LoadFromFile();
	pixel_shader->LoadFromFile();

	m_fogblendpass = new IntermediatePass( "fogblend_pass" );
	m_fogblendpass->CreateViewportQuad();
	m_fogblendpass->GetViewportQuad()->GetFx()->AddShader( vertex_shader );
	m_fogblendpass->GetViewportQuad()->GetFx()->AddShader( pixel_shader );
	m_fogblendpass->GetViewportQuad()->SetTexture( m_texturepass->GetTargetTexture(), 0 );
	m_fogblendpass->GetViewportQuad()->SetTexture( m_fogintpass->GetTargetTexture(), 1 );

	m_fogblendpass->GetViewportQuad()->LoadAssets();

	m_fogblendpass->GetViewportQuad()->GetFx()->AddShaderRealVectorParameter( 1, "fog_color", 0 );
	m_fogblendpass->GetViewportQuad()->GetFx()->SetShaderRealVector( "fog_color", DrawSpace::Utils::Vector( 145.0 / 255.0, 230.0 / 255.0, 230.0 / 255.0, 1.0 ) );


	//////////////////////////////////////////////////////////////

	vertex_shader = new Shader( "texture.vsh", false );
	pixel_shader = new Shader( "texture.psh", false );

	vertex_shader->LoadFromFile();
	pixel_shader->LoadFromFile();

	m_finalpass = new FinalPass( "final_pass" );
	m_finalpass->CreateViewportQuad();
	m_finalpass->GetViewportQuad()->GetFx()->AddShader( vertex_shader );
	m_finalpass->GetViewportQuad()->GetFx()->AddShader( pixel_shader );
    

	m_finalpass->GetViewportQuad()->SetTexture( m_fogblendpass->GetTargetTexture(), 0 );
	
	m_finalpass->GetViewportQuad()->LoadAssets();



    ///////////////////////////////////////////////////////////////

    m_scenegraph.RegisterPass( m_texturepass );
    m_scenegraph.RegisterPass( m_fogintpass );


    ///////////////////////////////////////////////////////////////
    
    

    DrawSpace::Core::RenderingNode* fogint_rnode = new DrawSpace::Core::RenderingNode;
    
	vertex_shader = new Shader( "fogintensity.vsh", false );
	pixel_shader = new Shader( "fogintensity.psh", false );

	vertex_shader->LoadFromFile();
	pixel_shader->LoadFromFile();

	fogint_rnode->GetFx()->AddShader( vertex_shader );
	fogint_rnode->GetFx()->AddShader( pixel_shader );

	fogint_rnode->GetFx()->AddShaderRealParameter( 0, "fog_intensity", 12 );
	fogint_rnode->GetFx()->SetShaderReal( "fog_intensity", 0.2 );


	fogint_rnode->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
	fogint_rnode->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );


    
    DrawSpace::Core::RenderingNode* texture_rnode = new DrawSpace::Core::RenderingNode;

	vertex_shader = new Shader( "texture.vsh", false );
	pixel_shader = new Shader( "texture.psh", false );

	vertex_shader->LoadFromFile();
	pixel_shader->LoadFromFile();

	texture_rnode->GetFx()->AddShader( vertex_shader );
	texture_rnode->GetFx()->AddShader( pixel_shader );

	texture_rnode->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
	texture_rnode->GetFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
	texture_rnode->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
	texture_rnode->GetFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );

	Texture* texture_metal;
	texture_metal = new Texture( "bellerophon.jpg" );
	texture_metal->LoadFromFile();
	
	texture_rnode->SetTexture( texture_metal, 0 );





    m_chunknode = new DrawSpace::ChunkNode( "cube" );


    DrawSpace::Utils::AC3DMesheImport importer;
    m_chunknode->GetMeshe()->SetImporter( &importer );
	m_chunknode->GetMeshe()->LoadFromFile( "object.ac", 0 );

    m_chunknode->RegisterPassRenderingNode( "fogint_pass", fogint_rnode );
    m_chunknode->RegisterPassRenderingNode( "texture_pass", texture_rnode );


    m_scenegraph.RegisterNode( m_chunknode );
    m_chunknode->LoadAssets();


	//////////////////////////////////////////////////////////////

	m_transform = new SceneTransform();
	m_transformtask = new DrawSpace::Core::Task<SceneTransform>( DrawSpace::Core::Task<SceneTransform>::Kill );
	m_transformtask->Startup( m_transform );


	//////////////////////////////////////////////////////////////

	DrawSpace::Utils::CBFGFontImport fontimporter;
	m_font = new DrawSpace::Core::Font;

	m_font->SetImporter( &fontimporter );

	status = m_font->Build( "mangalfont.bmp", "mangalfont.csv" );
	if( !status )
	{
		return false;
	}



	m_fpstext_widget = new DrawSpace::Gui::TextWidget( "fps_text_widget", 20, 10, m_font, false );

	m_fpstext_widget->GetImageFx()->AddShader( new Shader( "text.vsh", false ) );
	m_fpstext_widget->GetImageFx()->AddShader( new Shader( "text.psh", false ) );
	m_fpstext_widget->GetImageFx()->GetShader( 0 )->LoadFromFile();
	m_fpstext_widget->GetImageFx()->GetShader( 1 )->LoadFromFile();

	m_fpstext_widget->GetTextFx()->AddShader( new Shader( "text.vsh", false ) );
	m_fpstext_widget->GetTextFx()->AddShader( new Shader( "text.psh", false ) );
	m_fpstext_widget->GetTextFx()->GetShader( 0 )->LoadFromFile();
	m_fpstext_widget->GetTextFx()->GetShader( 1 )->LoadFromFile();
	m_fpstext_widget->GetTextFx()->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
	m_fpstext_widget->GetTextFx()->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
	m_fpstext_widget->GetTextFx()->AddShaderRealVectorParameter( 1, "color", 0 );
	m_fpstext_widget->GetTextFx()->SetShaderRealVector( "color", Utils::Vector( 0.4, 0.0, 0.0, 1.0 ) );

	m_fpstext_widget->RegisterToPass( m_finalpass );
	m_fpstext_widget->LoadAssets();



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

}

void dsAppClient::OnEndKeyPress( long p_key )
{

}

void dsAppClient::OnKeyPulse( long p_key )
{

}

void dsAppClient::OnMouseMove( long p_xm, long p_ym, long p_dx, long p_dy )
{
	if( m_mouselb )
	{
		m_transform->OnLeftDrag( m_timer, p_dx, p_dy );
	}
	else if( m_mouserb )
	{
		m_transform->OnRightDrag( m_timer, p_dx, p_dy );
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
