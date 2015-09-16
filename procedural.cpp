#include "core/util.h"
#include "core/core.h"
#include "core/graphics.h"
#include "core/Shader.h"
#include "core/BadMesh.h"
#include "core/FontRender.h"

#include "GameTextures.h"
#include "GameMeshes.h"

#include <string>
#include "gui.h"
using namespace std;

#include "Scene.h"
#include "LuaGenerator.h"

#include "InputHandlers.h"

extern GLShader Shader_Prelit;
BadMesh *cube;
Vec3 from, to;
extern float g_fGameTime;

//float gUserAngleYaw = 0.1f;
//float gUserAnglePitch = 0.4f;

InputHanlder_YawPitch camInput;

void GameUpdate() {
	void UpdateProcGenTexture();
	UpdateProcGenTexture();
	glUniform1f(DefaultShaderProgram.timeLocation, g_fGameTime);
	double drawStart = glfwGetTime();
	camInput.Update( 0.1f );
	//if( glfwGetKey(GLFW_KEY_UP) )
		//gUserAnglePitch += 0.1f;
	//if( glfwGetKey(GLFW_KEY_DOWN) )
		//gUserAnglePitch -= 0.1f;
	//if( glfwGetKey(GLFW_KEY_LEFT) )
		//gUserAngleYaw -= 0.1f;
	//if( glfwGetKey(GLFW_KEY_RIGHT) )
		//gUserAngleYaw += 0.1f;
	//const float yAngle = gUserAngleYaw;//0.1f * drawStart;
	//const float xAngle = gUserAnglePitch;//0.4f;
	const float yAngle = camInput.GetYaw();//0.1f * drawStart;
	const float xAngle = camInput.GetPitch();//0.4f;
	float sy = sin( yAngle );
	float cy = cos( yAngle );
	float sx = sin( xAngle );
	float cx = cos( xAngle );
	const float dist = 20.0f;
	from = Vec3( cx * sy * dist, sx * dist, cx * cy * dist );

	SetDefaultViewport();
	DefaultOrtho();
	SetCamera(gIdentityMat);
	glDepthFunc(GL_LEQUAL);
	ClearScreen( 0.3f, 0.3f, 0.3f );
	SetTexture( "sword", 0 );
	DrawSquare( 16, 16, 32, 32, 0xFFFFFFFF );

	Mat44 modelMat = Translation(Vec3( 30.0f + from.x, 30.0f + from.z, 0.0f));

	DefaultProjection();

	from = Vec3( cx * sy * dist, sx * dist, cx * cy * dist );
	to = Vec3( 0.0f, 0.0f, 0.0f );

	Mat44 look = Mat44LookAt( from, to, gYVec4 );
	SetCamera(look);

	modelMat = Translation(Vec3( 0.0f, 0.0f, 0.0f));
	if( false && fmodf( drawStart, 2.0f ) > 1.0f ) {
		SetTexture( "owl", 0 );
	} else {
		SetTexture( "checker", 0 );
	}
	modelMat.Scale(2.0f);
	SetModel( modelMat );
	
	float xborder = 32.0f / win_width;
	float yborder = 32.0f / win_height;
	SetViewport( Vec2(xborder,yborder), Vec2(1.0f-xborder,1.0f-yborder) );
	ClearScreen( 0.5f, 0.5f, 0.5f );

	if( gRootScene ) {
		gCurrentRenderGenerator = gRootScene;
		if( gRootScene->scene )
			gRootScene->scene->Render(gIdentityMat);
		gCurrentRenderGenerator = 0;
	}
	SetDefaultViewport();

	modelMat = Translation(Vec3( 30.0f + from.x, 30.0f + from.z, 0.0f));

	Ortho( "prelit" );
	glUniform1f(GLShader::Current()->timeLocation, g_fGameTime);
	glDepthFunc(GL_LEQUAL);
	SetCamera( gIdentityMat );
	SetModel( gIdentityMat );

	SetTexture( "sword", 0 );
	DrawSquare( 64, 16, 32, 32, 0xFFFFFFFF );
	SetFontTexture( 'B' );
	DrawSquare( 96, 16, 32, 32, 0xFFFFFFFF );
	DefaultOrtho();
	SetCamera( gIdentityMat );
	SetModel( gIdentityMat );
	SetTexture( "sword", 0 );
	DrawSquare( 128, 16, 32, 32, 0xFFFFFFFF );
	SetFontTexture( 'B' );
	SetFontTexture( -1 );
	DrawSquare( 160, 16, 32, 32, 0xFFFFFFFF );

	glUniformMatrix4fv(GLShader::Current()->viewLocation, 1, false, gIdentityMat );

	modelMat = Translation(Vec3( 0.0f,0.0f,0.0f ));
	modelMat.Scale(1.0f);
	FontPrint( modelMat, "Testing No VAR Water Kerning MMennwwWW" );
	UpdateGUI();
}

void PreBoot() {
	win_width = 600;
	win_height = 600;
}
DO_PREBOOT(PreBoot);

void GameInit() {
	gRootScene = AddSceneGenerator( "s_test" );

	AddMeshGenerator( "test" );
	AddTextureGenerator( "grass" );
	AddTextureGenerator( "checker" );

	Log( 1, "loading images\n" );
	// the cute dungeon spritesheet was
	Image *sheet = LoadImageG("data/sprite1.png"); // (C) Derek Yu 2008 
	AddAsset("sheet", sheet );
	AddSubAsset( "owl", *sheet, 5,7 );

	cube = new BadMesh();
	cube->SetAsCube();
	cube->UVsFromBB();
	InitGUI();
}
void GameShutdown() {
}

