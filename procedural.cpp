#include "core/util.h"
#include "core/graphics.h"
#include "core/Shader.h"
#include "core/BadMesh.h"
#include "core/FontRender.h"

#include "lua/lua.hpp"

#include "GameTextures.h"
#include "GameMeshes.h"

#include "LuaFuncs.h"
#include <map>
#include <vector>
#include <string>
using namespace std;

struct Scene {
	struct Node {
		Node( string mesh, string texture, string params, const Mat44 &orientation ) : o(orientation), m(mesh), t(texture), p(params) {}
		Mat44 o;
		string m,t,p;
	};
	typedef vector<Node> NodeVec;
	NodeVec m_nodes;
	void PushNode( string mesh, string texture, string params, const Mat44 &orientation ) {
		m_nodes.push_back( Node( mesh, texture, params, orientation ) );
	}
	void Render();
};

struct LuaGenerator {
	lua_State *L;
	Scene *scene;
	BadMesh *mesh;
	typedef map<string,BadMesh*> ParamMeshes;
	ParamMeshes paramMeshes;
	Image *texture;
	
	time_t lastFileTime;
	time_t currentFileTime;
	char m_filename[128];

	LuaGenerator() : L(0), scene(0), mesh(0), texture(0), lastFileTime(0), currentFileTime(0) { m_filename[0] = 0; }
	void ClearoutLuaState() { if( L ) { lua_close( L ); L = 0; } }
	~LuaGenerator() { ClearoutLuaState(); }
	bool IsValid() { return L != 0; }
	void Load( const char *filename ) {
		ClearoutLuaState();
		if( char * configBuffer = ReadFileToString( filename ) ) {
			L = luaL_newstate();
			luaL_openlibs( L );
			
			RegisterLuaFuncs( L );
			int r = luaL_loadbuffer( L, configBuffer, strlen( configBuffer ), "exec" );
			if( r ) {
				Log(3,"Lua error [%s] loading from buffer from file \"%s\"\n", lua_tostring(L,-1), filename );
				Log(3,"SCRIPT:>>>\n%s\n<<<END\n", configBuffer );
				ClearoutLuaState();
			} else {
				Log(3,"procgen mesh script loaded %x\n", L );
				if( lua_pcall(L,0,0,0) ) {
					Log(3,"Lua error executing [%s] from script \"%s\"\n", lua_tostring(L,-1), filename );
					ClearoutLuaState();
				} else {
					Log(3,"procgen mesh script executed\n" );
				}
			}
			free( configBuffer );
		}
		if( m_filename != filename ) strcpy( m_filename, filename );
		lastFileTime = currentFileTime;
	}
	void Update() {
		if( m_filename[0] ) {
			if( L ) {
				currentFileTime = get_mtime(m_filename, 0);
				if( lastFileTime ) {
					if( currentFileTime == lastFileTime ) {
						return;
					}
				} else {
					lastFileTime = currentFileTime;
					return;
				}
			}
			Load( m_filename );
			if( L && ( mesh || paramMeshes.size() ) ) {
				GenMesh();
			}
			if( L && texture ) {
				GenTexture();
			}
			if( L && scene ) {
				GenScene();
			}
		}
	}
#define L_ARGSINOUT( IN, OUT ) const int args_in = IN; const int args_out = OUT;
#define L_CALL() const int retval = lua_pcall(L, args_in, args_out, 0);
#define CHECK_ERROR( X, RET ) if( X ) { Log(3,"Lua error (%s) %s -> %x[%s]\n", m_filename, functionName, L,lua_tostring(L,-1) ); exit(0); return RET; }
#define L_GETGLOBAL( X ) lua_getglobal( L, X );
#define L_PUSHNUM( X ) lua_pushnumber( L, X )
#define L_CALLRETURNNUMBER( IN, OUT ) L_ARGSINOUT( IN, OUT ); L_CALL(); CHECK_ERROR( retval, 0.0f )
#define L_CALLRETURNSTRING( IN, OUT ) L_ARGSINOUT( IN, OUT ); L_CALL(); CHECK_ERROR( retval, "" )
#define L_CALLRETURNTHIS( IN, OUT, RET ) L_ARGSINOUT( IN, OUT ); L_CALL(); CHECK_ERROR( retval, RET )
#define L_RETVAL(X) lua_tonumber(L, X)
	int GetIntFromLua( const char *functionName ) {
		L_GETGLOBAL( functionName );
		L_CALLRETURNNUMBER( 0, 1 );
		double count = L_RETVAL(1);
		lua_pop(L,1);
		return (int)count;
	}
	Vec3 GetVec3FromLua( const char *functionName, int vindex, Vec3 defaultValue = gZeroVec3 ) {
		L_GETGLOBAL( functionName );
		L_PUSHNUM( vindex );
		L_CALLRETURNTHIS( 1, 3, defaultValue );
		double x = L_RETVAL(1);
		double y = L_RETVAL(2);
		double z = L_RETVAL(3);
		lua_pop(L,args_out);
		return Vec3( x, y, z );
	}
	Vec2 GetVec2FromLua( const char *functionName, int vindex ) {
		L_GETGLOBAL( functionName );
		L_PUSHNUM( vindex );
		L_CALLRETURNNUMBER( 1, 2 );
		double x = L_RETVAL(1);
		double y = L_RETVAL(2);
		lua_pop(L,args_out);
		return Vec2( x, y );
	}
	C32 GetColourFromLua( const char *functionName, int x, int y ) {
		L_GETGLOBAL( functionName );
		L_PUSHNUM( x );
		L_PUSHNUM( y );
		L_CALLRETURNNUMBER( 2, 4 );
		double r = clamp( L_RETVAL(1), 0.0, 255.0 );
		double g = clamp( L_RETVAL(2), 0.0, 255.0 );
		double b = clamp( L_RETVAL(3), 0.0, 255.0 );
		double a = clamp( L_RETVAL(4), 0.0, 255.0 );
		lua_pop(L,args_out);

		C32 value = ((int)a << 24) + ((int)b << 16) + ((int)g << 8) + (int)r;
		return value;
	}

	string GetStringFromLua( const char *functionName ) {
		L_GETGLOBAL( functionName );
		L_CALLRETURNSTRING( 0, 1 );
		const char *r = lua_tostring(L, 1);
		lua_pop(L,args_out);
		return r;
	}

	string GetStringFromLua( const char *functionName, int n ) {
		L_GETGLOBAL( functionName );
		L_PUSHNUM( n );
		L_CALLRETURNSTRING( 1, 1 );
		const char *r = lua_tostring(L, 1);
		lua_pop(L,args_out);
		return r;
	}
	void PushParams( const char *params ) {
			int r = luaL_loadbuffer( L, params, strlen( params ), "exec" );
			if( r ) {
				Log(3,"Lua error [%s] loading from buffer from params\n", lua_tostring(L,-1) );
				Log(3,"SCRIPT:>>>\n%s\n<<<END\n", params );
			} else {
				Log(3,"param script loaded %x\n", L );
				if( lua_pcall(L,0,0,0) ) {
					Log(3,"Lua error executing [%s] from params\n", lua_tostring(L,-1));
					ClearoutLuaState();
				} else {
					Log(3,"procgen mesh params executed\n" );
					RegisterLuaFuncs( L );
				}
			}
	}

	string GetScriptName() { return GetStringFromLua( "ScriptName" ); }
	C32 GetColour( int x, int y ) { return GetColourFromLua( "GetRGBA", x, y ); }
	int GetNumVerts() { return GetIntFromLua( "GetMeshVertCount" ); }
	Vec3 GetVert( int v ) { return GetVec3FromLua( "GetMeshVert", v ); }
	Vec3 GetNormal( int v ) { return GetVec3FromLua( "GetMeshNormal", v ); }
	Vec2 GetUV( int v ) { return GetVec2FromLua( "GetMeshUV", v ); }
	int GetNumNodes() { return GetIntFromLua( "GetSceneNodeCount" ); }
	Vec3 GetNodePos( int n ) { return GetVec3FromLua( "GetSceneNodePos", n ); }
	Vec3 GetNodeRotation( int n ) { return GetVec3FromLua( "GetSceneNodeRotation", n ); }
	Vec3 GetNodeScale( int n ) { return GetVec3FromLua( "GetSceneNodeScale", n); }
	string GetNodeMesh( int n ) { return GetStringFromLua( "GetSceneNodeMesh", n ); }
	string GetNodeTexture( int n ) { return GetStringFromLua( "GetSceneNodeTexture", n ); }
	string GetNodeParams( int n ) { return GetStringFromLua( "GetSceneNodeParams", n ); }

	void GenScene() {
		if( scene ) { delete scene; scene = 0; }

		int nodeCount = GetNumNodes();
		if( nodeCount > 0 ) {
			scene = new Scene();
			for( int v = 0; v < nodeCount; ++v ) {
				Vec3 pos = GetNodePos( v );
				Vec3 rotation = GetNodeRotation( v );
				Vec3 scale = GetNodeScale( v );
				Mat44 orientation;
				orientation.SetIdentity();
				orientation.Translate( pos );
				orientation.Rotation( rotation );
				orientation.Scale( scale );
				string meshName = GetNodeMesh( v );
				string textureName = GetNodeTexture( v );
				string params = GetNodeParams( v );
				Log(3,"SceneNode %s:%s\n", meshName.c_str(), textureName.c_str());
				scene->PushNode( meshName, textureName, params, orientation );
			}
			//mesh->UVsFromBB();
		}
	}
	void GenMesh( const char *params = 0 ) {
		if( params ) {
			PushParams( params );
		} else {
			Log(3,"Clearing all previous generated meshes for %s", m_filename );
			if( mesh ) { delete mesh; mesh = 0; }
			for( ParamMeshes::iterator i = paramMeshes.begin(); i != paramMeshes.end(); ++i ) {
				delete i->second;
			}
			paramMeshes.clear();
		}


		int vertCount = GetNumVerts();
		if( vertCount > 0 ) {
			BadMesh *m = new BadMesh();
			for( int v = 0; v < vertCount; ++v ) {
				Vec3 vert = GetVert( v );
				Vec3 normal = GetNormal( v );
				Vec2 uv = GetUV( v );
				m->PushVNUC( vert, normal, uv );
			}
			//mesh->UVsFromBB();

			if( params ) {
				paramMeshes[params] = m;
			} else {
				mesh = m;
			}
		}
	}
	void GenTexture() {
		if( texture ) { delete texture; texture = 0; }
		Image img;
		img.w = 256;
		img.h = 256;
		//Log( 1, "generating image %ix%i\n", img.w, img.h );
		img.p = new C32[img.w*img.h];
		for( int y = 0; y < img.h; ++y ) {
			for( int x = 0; x < img.w; ++x ) {
				C32 c = GetColourFromLua( "GetRGBA", x, y );
				img.p[x+y*img.w] = c;
			}
		}
		texture = new Image;
		*texture = img;
		//Log( 1, "regenerated image %ix%i\n", texture->w, texture->h );
		AddAsset(GetScriptName(), texture );
		PostInitGraphics();
	}
};

LuaGenerator * AddMeshGenerator( const char *guessName, const char *params = 0 );
LuaGenerator * AddTextureGenerator( const char *guessName );

void Scene::Render() {
	for( NodeVec::iterator i = m_nodes.begin(); i != m_nodes.end(); ++i ) {
		LuaGenerator *m = AddMeshGenerator(i->m.c_str(), i->p.c_str());
		AddTextureGenerator(i->t.c_str());

		SetTexture( i->t.c_str(), 0 );
		SetModel( i->o );
		BadMesh *bm = m->paramMeshes[i->p];
		if( bm ) {
			bm->DrawTriangles();
		}
		//Log(3,"Render mesh:texture %s:%s at %f,%f,%f,%f \n", i->m.c_str(), i->t.c_str(), i->o.w.x, i->o.w.y, i->o.w.z, i->o.w.w );
		//Log(3,"Render %f,%f,%f,%f \n", i->o.x.x, i->o.x.y, i->o.x.z, i->o.x.w );
		//Log(3,"Render %f,%f,%f,%f \n", i->o.y.x, i->o.y.y, i->o.y.z, i->o.y.w );
		//Log(3,"Render %f,%f,%f,%f \n", i->o.z.x, i->o.z.y, i->o.z.z, i->o.z.w );
	}
}

typedef std::map<std::string, LuaGenerator*> GenMap;
GenMap gGenerators;

extern GLShader Shader_Prelit;
BadMesh *cube, *procmesh;
Vec3 from, to;
extern Mat44 gIdentityMat;
extern float g_fGameTime;

void GameUpdate() {
	void UpdateProcGenTexture();
	UpdateProcGenTexture();
	glUniform1f(DefaultShaderProgram.timeLocation, g_fGameTime);
	double drawStart = glfwGetTime();
	const float yAngle = 0.01f * drawStart;
	const float xAngle = 0.4f;
	float sy = sin( yAngle );
	float cy = cos( yAngle );
	float sx = sin( xAngle );
	float cx = cos( xAngle );
	const float dist = 20.0f;
	from = Vec3( cx * sy * dist, sx * dist, cx * cy * dist );

	DefaultOrtho();
	SetCamera(gIdentityMat);
	glDepthFunc(GL_LEQUAL);
	ClearScreen( 0.3f, 0.3f, 0.3f );
	SetTexture( "sword", 0 );
	DrawSquare( 16, 16, 32, 32, 0xFFFFFFFF );

	Mat44 modelMat = Translation(Vec3( 30.0f + from.x, 30.0f + from.z, 0.0f));
	//modelMat.Scale(1.0f);
	//glUniformMatrix4fv(GLShader::Current()->mvLocation, 1, false, modelMat );

	DefaultProjection();

	//from = Vec3( sy * 20.0f, 0.0f, cy * 20.0f );
	from = Vec3( cx * sy * dist, sx * dist, cx * cy * dist );
	to = Vec3( 0.0f, 0.0f, 0.0f );

	Mat44 look = Mat44LookAt( from, to, gYVec4 );
	SetCamera(look);

	modelMat = Translation(Vec3( 0.0f, 0.0f, 0.0f));
	if( false && fmodf( drawStart, 2.0f ) > 1.0f ) {
		SetTexture( "owl", 0 );
	} else {
		//SetTexture( "sheet", 0 );
		SetTexture( "checker", 0 );
	}
	modelMat.Scale(2.0f);
	SetModel( modelMat );
	//if( gGenerators.count( "test" ) ) {
		//gGenerators["test"]->mesh->DrawTriangles();
	//} else {
		//cube->DrawTriangles();
	//}

//	if( gGenerators.count( "land" ) ) {
//		gGenerators["land"]->mesh->DrawTriangles();
//	}

	for( GenMap::iterator i = gGenerators.begin(); i != gGenerators.end(); ++i ) {
		if( i->second->scene ) {
			i->second->scene->Render();
		}
	}

	modelMat = Translation(Vec3( 30.0f + from.x, 30.0f + from.z, 0.0f));

	Ortho( "prelit" );
	glUniform1f(GLShader::Current()->timeLocation, g_fGameTime);
	//DefaultOrtho();
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
}

// lua stuff
lua_State *procGenMeshLua = 0;
int GetIntFromLua( const char *functionName, int default_value = -1 ) {
	lua_getglobal( procGenMeshLua, functionName );

	int args_in = 0;
	int args_out = 1;
	int retval = lua_pcall(procGenMeshLua, args_in, args_out, 0);

	if( retval ) {
		Log(3,"Lua error %s -> %x[%s]\n", functionName, procGenMeshLua,lua_tostring(procGenMeshLua,-1) );
		exit(0);
		return default_value;
	}

	double count = lua_tonumber(procGenMeshLua, 1);
	lua_pop(procGenMeshLua,1);

	return (int)count;
}
Vec3 GetVectorFromLua( const char *functionName, int vindex ) {
	lua_getglobal( procGenMeshLua, functionName );
	lua_pushnumber( procGenMeshLua, vindex );

	int args_in = 1;
	int args_out = 3;
	int retval = lua_pcall(procGenMeshLua, args_in, args_out, 0);

	if( retval ) {
		Log(3,"Lua error %s(%i) -> %x[%s]\n", functionName, vindex, procGenMeshLua,lua_tostring(procGenMeshLua,-1) );
		exit(0);
		return 0.0f;
	}

	double x = lua_tonumber(procGenMeshLua, 1);
	double y = lua_tonumber(procGenMeshLua, 2);
	double z = lua_tonumber(procGenMeshLua, 3);
	lua_pop(procGenMeshLua,3);

	return Vec3( x, y, z );
}
Vec2 GetUVFromLua( const char *functionName, int vindex ) {
	lua_getglobal( procGenMeshLua, functionName );
	lua_pushnumber( procGenMeshLua, vindex );

	int args_in = 1;
	int args_out = 2;
	int retval = lua_pcall(procGenMeshLua, args_in, args_out, 0);

	if( retval ) {
		Log(3,"Lua error %s -> %x[%s]\n", functionName, procGenMeshLua,lua_tostring(procGenMeshLua,-1) );
		exit(0);
		return 0.0f;
	}

	double x = lua_tonumber(procGenMeshLua, 1);
	double y = lua_tonumber(procGenMeshLua, 2);
	lua_pop(procGenMeshLua,2);

	return Vec2( x, y );
}
lua_State *procGenTextureLua = 0;
C32 GetColourFromLua( const char *functionName, int x, int y ) {
	lua_getglobal( procGenTextureLua, functionName );
	lua_pushnumber( procGenTextureLua, x );
	lua_pushnumber( procGenTextureLua, y );

	int args_in = 2;
	int args_out = 4;
	int retval = lua_pcall(procGenTextureLua, args_in, args_out, 0);

	if( retval ) {
		Log(3,"Lua error %s -> %x[%s]\n", functionName, procGenTextureLua,lua_tostring(procGenTextureLua,-1) );
		exit(0);
		return 0.0f;
	}

	double r = clamp( lua_tonumber(procGenTextureLua, 1), 0.0, 255.0 );
	double g = clamp( lua_tonumber(procGenTextureLua, 2), 0.0, 255.0 );
	double b = clamp( lua_tonumber(procGenTextureLua, 3), 0.0, 255.0 );
	double a = clamp( lua_tonumber(procGenTextureLua, 4), 0.0, 255.0 );
	//double g = lua_tonumber(procGenTextureLua, 2);
	//double b = lua_tonumber(procGenTextureLua, 3);
	//double a = lua_tonumber(procGenTextureLua, 4);
	lua_pop(procGenTextureLua,4);

	C32 value = ((int)a << 24) + ((int)b << 16) + ((int)g << 8) + (int)r;
	return value;
}

static const char *procMeshFilename = "data/procmesh.lua";
time_t last_get_luamesh;
bool NewProcGenMesh() {
	time_t current = get_mtime(procMeshFilename, 0);
	if( last_get_luamesh != 0 ) {
		if( current ) {
			if( current > last_get_luamesh ) {
				last_get_luamesh = current;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else {
		last_get_luamesh = current;
	}
	// now test loading a lua function
	if( procGenMeshLua ) {
		lua_close( procGenMeshLua );
	}
	procGenMeshLua = luaL_newstate();
	if( char * configBuffer = ReadFileToString( procMeshFilename ) ) {
		int r = luaL_loadbuffer( procGenMeshLua, configBuffer, strlen( configBuffer ), "exec" );
		if( r ) {
			Log(3,"Lua error [%s]\n", lua_tostring(procGenMeshLua,-1) );
			return false;
		} else {
			Log(3,"procgen mesh script loaded %x\n", procGenMeshLua );
			if( lua_pcall(procGenMeshLua,0,0,0) ) {
				Log(3,"Lua error executing [%s]\n", lua_tostring(procGenMeshLua,-1) );
				return false;
			}
			Log(3,"procgen mesh script executed\n" );
			RegisterLuaFuncs( procGenMeshLua );
		}
		free( configBuffer );
	}
	return true;
}

static const char *procTextureFilename = "data/proctexture.lua";
time_t last_get_luatexture;
bool NewProcGenTexture() {
	time_t current = get_mtime(procTextureFilename, 0);
	if( last_get_luatexture != 0 ) {
		if( current ) {
			if( current > last_get_luatexture ) {
				last_get_luatexture = current;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else {
		last_get_luatexture = current;
	}
	// now test loading a lua function
	if( procGenTextureLua ) {
		lua_close( procGenTextureLua );
	}
	procGenTextureLua = luaL_newstate();
	if( char * configBuffer = ReadFileToString( procTextureFilename ) ) {
		int r = luaL_loadbuffer( procGenTextureLua, configBuffer, strlen( configBuffer ), "exec" );
		if( r ) {
			Log(3,"Lua error [%s]\n", lua_tostring(procGenTextureLua,-1) );
			return false;
		} else {
			Log(3,"procgen texture script loaded %x\n", procGenTextureLua );
			if( lua_pcall(procGenTextureLua,0,0,0) ) {
				Log(3,"Lua error executing [%s]\n", lua_tostring(procGenTextureLua,-1) );
				return false;
			}
			Log(3,"procgen texture script executed\n" );
			RegisterLuaFuncs( procGenTextureLua );
		}
		free( configBuffer );
	}
	return true;
}


void GenLandscape() {
	delete procmesh;

	procmesh = new BadMesh();

	int vertCount = GetIntFromLua( "GetMeshVertCount" );
	for( int v = 0; v < vertCount; ++v ) {
		Vec3 vert = GetVectorFromLua( "GetMeshVert", v );
		Vec3 normal = GetVectorFromLua( "GetMeshNormal", v );
		Vec2 uv = GetUVFromLua( "GetMeshUV", v );
		procmesh->PushVNUC( vert, normal, uv );
	}

	//procmesh->UVsFromBB();
}
Image GenTexture() {
	Image img;
	img.w = 256;
	img.h = 256;
	Log( 1, "generating image %ix%i\n", img.w, img.h );
	img.p = new C32[img.w*img.h];
	for( int y = 0; y < img.h; ++y ) {
		for( int x = 0; x < img.w; ++x ) {
#if 1
			C32 c = GetColourFromLua( "GetRGBA", x, y );
			img.p[x+y*img.w] = c;
#else		
			int r = 255&(x*5);
			int g = 255&(y*5);
			int b = 255&((x+y)*5);
			int a = 255;
			img.p[x+y*img.w] = (a<<24)+(b<<16)+(g<<8)+r;
			//img.p[x+y*img.w] = (r<<24)+(g<<16)+(b<<8)+a;
			//img.p[x+y*img.w] = 0x00FFFFFF;
#endif
		}
	}
	return img; 
}
Image *proctexture;
void UpdateProcGenTexture() {
	//if( NewProcGenTexture() ) {
		//*proctexture = GenTexture();
		//Log( 1, "regenerated image %ix%i\n", proctexture->w, proctexture->h );
		//AddAsset("proctexture", proctexture );
		//PostInitGraphics();
	//}
	for( GenMap::iterator i = gGenerators.begin(); i != gGenerators.end(); ++i ) {
		i->second->Update();	
	}
}

LuaGenerator * AddSceneGenerator( const char *guessName ) {
	LuaGenerator *lg = 0;
	if( gGenerators.count( guessName ) == 0 ) {
		char filename[128];
		lg = new LuaGenerator;
		sprintf( filename, "data/procscene-%s.lua", guessName );
		lg->Load( filename );
		lg->GenScene();
		gGenerators[lg->GetScriptName()] = lg;
	} else {
		lg = gGenerators[guessName];
	}
	return lg;
}
LuaGenerator * AddMeshGenerator( const char *guessName, const char *params ) {
	LuaGenerator *lg = 0;
	if( gGenerators.count( guessName ) == 0 ) {
		char filename[128];
		lg = new LuaGenerator;
		sprintf( filename, "data/procmesh-%s.lua", guessName );
		lg->Load( filename );
		lg->GenMesh(params);
		gGenerators[lg->GetScriptName()] = lg;
	} else {
		lg = gGenerators[guessName];
		if( params && lg->paramMeshes.count( params ) == 0 ) {
			lg->GenMesh(params);
		}
	}
	return lg;
}
LuaGenerator * AddTextureGenerator( const char *guessName ) {
	LuaGenerator *lg = 0;
	if( gGenerators.count( guessName ) == 0 ) {
		char filename[128];
		lg = new LuaGenerator;
		sprintf( filename, "data/proctexture-%s.lua", guessName );
		lg->Load( filename );
		lg->GenTexture();
		string scriptName = lg->GetScriptName();
		Log( 1, "registering texture generator %s\n", scriptName.c_str() );
		gGenerators[lg->GetScriptName()] = lg;
	} else {
		lg = gGenerators[guessName];
	}
	return lg;
}
void GameInit() {
	NewProcGenTexture();

	//AddSceneGenerator( "s_test" );
	AddSceneGenerator( "tree" );

	AddMeshGenerator( "test" );
	AddTextureGenerator( "grass" );
	AddTextureGenerator( "checker" );

	//NewProcGenMesh();
	//GenLandscape();
	Log( 1, "loading images\n" );
	// the cute dungeon spritesheet was
	Image *sheet = LoadImageG("data/sprite1.png"); // (C) Derek Yu 2008 
	AddAsset("sheet", sheet );
	AddSubAsset( "owl", *sheet, 5,7 );
	//proctexture = new Image;
	//*proctexture = GenTexture();
	//Log( 1, "generated image %ix%i\n", proctexture->w, proctexture->h );
	//AddAsset("proctexture", proctexture );

	//GameMeshes::Init();

	cube = new BadMesh();
	cube->SetAsCube();
	cube->UVsFromBB();
}
void GameShutdown() {
}

