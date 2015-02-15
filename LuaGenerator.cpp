#include "LuaGenerator.h"
#include "lua/lua.hpp"
#include "core/util.h"
#include "LuaFuncs.h"
#include "Scene.h"
#include "core/BadMesh.h"

LuaGenerator *gCurrentRenderGenerator;
GenMap gGenerators;
LuaGenerator *gRootScene;

LuaGenerator::LuaGenerator() : L(0), scene(0), mesh(0), texture(0), loadedMesh(false), lastFileTime(0), currentFileTime(0) {
	m_filename[0] = 0;
}

void LuaGenerator::ClearoutLuaState() {
	if( L ) {
		lua_close( L ); L = 0;
	}
	loadedMesh = false;
}
LuaGenerator::~LuaGenerator() {
	ClearoutLuaState();
}

bool LuaGenerator::IsValid() {
	return L != 0;
}

void LuaGenerator::Load( const char *filename ) {
	ClearoutLuaState();
	if( EndsWith( filename, ".lua" ) ) {
		LoadLua( filename );
	} else {
		LoadMesh( filename );
	}
	if( m_filename != filename ) strcpy( m_filename, filename );
	lastFileTime = currentFileTime;
}
void LuaGenerator::LoadLua( const char *filename ) {
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
}
void LuaGenerator::LoadMesh( const char *filename ) {
	if( mesh ) {
		delete mesh;
	}
	mesh = new BadMesh();
	Log( 3, "Loading standard mesh : %s\n", filename );
	if( mesh->Load(filename ) ) {
		loadedMesh = true;
	}
}
void LuaGenerator::Update() {
	if( m_filename[0] ) {
		if( L || loadedMesh ) {
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
		if( L && mesh ) {
			Log(3,"regenerating basic mesh\n" );
			GenMesh();
		}
		if( L && texture ) {
			Log(3,"regenerating basic texture\n" );
			GenTexture();
		}
		if( L && scene ) {
			Log(3,"regenerating basic scene\n" );
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

bool LuaGenerator::FunctionExists( const char *functionName ) {
	if( !L ) return false;
	L_GETGLOBAL( functionName );
	int result = lua_isfunction(L, -1);
	lua_pop(L, 1);
	//Log(3,"Checking for %s -> %i\n", functionName, result );
	return result != 0;
}
int LuaGenerator::GetIntFromLua( const char *functionName ) {
	if( !FunctionExists( functionName ) ) return 0;
	L_GETGLOBAL( functionName );
	L_CALLRETURNNUMBER( 0, 1 );
	double count = L_RETVAL(1);
	lua_pop(L,1);
	return (int)count;
}
Vec3 LuaGenerator::GetVec3FromLua( const char *functionName, int vindex, Vec3 defaultValue ) {
	if( !FunctionExists( functionName ) ) return gZeroVec3;
	L_GETGLOBAL( functionName );
	L_PUSHNUM( vindex );
	L_CALLRETURNTHIS( 1, 3, defaultValue );
	double x = L_RETVAL(1);
	double y = L_RETVAL(2);
	double z = L_RETVAL(3);
	lua_pop(L,args_out);
	return Vec3( x, y, z );
}
Vec2 LuaGenerator::GetVec2FromLua( const char *functionName, int vindex ) {
	if( !FunctionExists( functionName ) ) return Vec2(0,0);
	L_GETGLOBAL( functionName );
	L_PUSHNUM( vindex );
	L_CALLRETURNNUMBER( 1, 2 );
	double x = L_RETVAL(1);
	double y = L_RETVAL(2);
	lua_pop(L,args_out);
	return Vec2( x, y );
}
Mat44 LuaGenerator::GetMat44FromLua( const char *functionName ) {
	if( !FunctionExists( functionName ) ) return gIdentityMat;
	L_GETGLOBAL( functionName );
	Mat44 defaultValue;
	defaultValue.SetIdentity();
	L_CALLRETURNTHIS( 0, 16, defaultValue );
	float *m = defaultValue;
	for( int i = 0; i < 16; ++i ) {
		double val = L_RETVAL(i+1);
		m[i] = val;
	}
	lua_pop(L,args_out);
	return defaultValue;
}
C32 LuaGenerator::GetColourFromLua( const char *functionName, int x, int y ) {
	if( !FunctionExists( functionName ) ) return 0;
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
string LuaGenerator::GetStringFromLua( const char *functionName ) {
	if( !FunctionExists( functionName ) ) return "";
	L_GETGLOBAL( functionName );
	L_CALLRETURNSTRING( 0, 1 );
	const char *r = lua_tostring(L, 1);
	lua_pop(L,args_out);
	if( r )
		return r;
	else
		return "";
}

string LuaGenerator::GetStringFromLua( const char *functionName, int n ) {
	if( !FunctionExists( functionName ) ) return "";
	L_GETGLOBAL( functionName );
	L_PUSHNUM( n );
	L_CALLRETURNSTRING( 1, 1 );
	const char *r = lua_tostring(L, 1);
	lua_pop(L,args_out);
	if( r )
		return r;
	else
		return "";
}
void LuaGenerator::CallUpdate() {
	const char *functionName = "Update";
	if( !FunctionExists( functionName ) ) return;
	L_GETGLOBAL( functionName );
	L_CALLRETURNTHIS( 0, 0, );
	lua_pop(L,args_out);
	return;
}
void LuaGenerator::PushParams( const char *params ) {
	if( params ) {
		Log(3,"Pushing params <<%s>> to %x\n", params, L );
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
	} else {
		Log(3,"Calling without actual params\n" );
	}
}

void LuaGenerator::GenScene( const char *params ) {
	Log(3,"Clearing all previous generated scenes for %s\n", m_filename );
	if( params ) { PushParams( params ); }

	if( scene ) { delete scene; scene = 0; }

	int nodeCount = GetNumNodes();
	if( nodeCount > 0 ) {
		Scene *s = new Scene();
		for( int v = 0; v < nodeCount; ++v ) {
			Vec3 pos = GetNodePos( v );
			Vec3 rotation = GetNodeRotation( v );
			Vec3 scale = GetNodeScale( v );
			Mat44 orientation;
			orientation.SetIdentity();
			orientation.Translate( pos );
			orientation.Rotation( rotation );
			orientation.Scale( scale );
			string orientationFunction = GetNodeOrientationFunction( v );
			string meshName = GetNodeMesh( v );
			string textureName = GetNodeTexture( v );
			if( orientationFunction.size() ) {
				Log(3,"SceneNode AnimMesh %s:%s @ %s\n", meshName.c_str(), textureName.c_str(), orientationFunction.c_str());
				if( meshName.size() == 0 ) {
					// camera
					s->PushNode( orientationFunction );
				} else {
					s->PushNode( meshName, textureName, orientationFunction );
				}
			} else {
				if( meshName.size() == 0 ) {
					// doing a camera
					Log(3,"SceneNode Camera\n");
					s->PushNode( orientation );
				} else {
					if( textureName.size() == 0 ) {
						// doing a scene
						Log(3,"SceneNode Scene %s:%s\n", meshName.c_str(), textureName.c_str() );
						s->PushNode( meshName, orientation );
					} else {
						// doing a mesh
						Log(3,"SceneNode Mesh %s:%s\n", meshName.c_str(), textureName.c_str());
						s->PushNode( meshName, textureName, orientation );
					}
				}
			}
		}
		scene = s;
	} else {
		scene = 0;
	}
}
void LuaGenerator::GenMesh( const char *params ) {
	Log(3,"Clearing all previous generated meshes for %s\n", m_filename );
	if( params ) { PushParams( params ); }

	if( mesh ) { delete mesh; mesh = 0; }

	int vertCount = GetNumVerts();
	//if( vertCount > 0 ) {
	BadMesh *m = new BadMesh();
	for( int v = 0; v < vertCount; ++v ) {
		Vec3 vert = GetVert( v );
		Vec3 normal = GetNormal( v );
		Vec2 uv = GetUV( v );
		m->PushVNUC( vert, normal, uv );
	}

	mesh = m;
	//}
}
void LuaGenerator::GenTexture( const char *params ) {
	Log(3,"Clearing all previous generated textures for %s\n", m_filename );
	if( params ) { PushParams( params ); }

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
	if( params ) {
		char fullName[128];
		sprintf( fullName, "%s:%s", GetScriptName().c_str(), params );
		AddAsset(fullName, texture );
	} else {
		AddAsset(GetScriptName(), texture );
	}
	PostInitGraphics();
}


// free functions : Adding Generators

LuaGenerator * AddSceneGenerator( const char *guessName, const char * params ) {
	char name[128];
	char args[128]; args[0] = 0;
	strcpy( name, guessName );
	if( char *mid = strchr( name, ':' ) ) {
		strcpy( args, mid+1 );
		*mid = 0;
		params = args;
		//guessName = name;
	}
	LuaGenerator *lg = 0;
	if( gGenerators.count( guessName ) == 0 ) {
		char filename[128];
		lg = new LuaGenerator;
		sprintf( filename, "data/procscene-%s.lua", name );
		lg->Load( filename );
		lg->GenScene(params);
		//gGenerators[lg->GetScriptName()] = lg;
		gGenerators[guessName] = lg;
	} else {
		lg = gGenerators[guessName];
	}
	return lg;
}
LuaGenerator * AddMeshGenerator( const char *guessName, const char *params ) {
	char name[128];
	char args[128]; args[0] = 0;
	strcpy( name, guessName );
	if( char *mid = strchr( name, ':' ) ) {
		strcpy( args, mid+1 );
		*mid = 0;
		params = args;
	}
	LuaGenerator *lg = 0;
	if( gGenerators.count( guessName ) == 0 ) {
		char filename[128];
		lg = new LuaGenerator;
		sprintf( filename, "data/procmesh-%s.lua", name );
		if( FILE *fp = fopen( filename, "r" ) ) {
			fclose( fp );
			lg->Load( filename );
			lg->GenMesh(params);
		} else {
			strcpy( filename, guessName );
			lg->Load( filename );
		}
		gGenerators[guessName] = lg;
	} else {
		lg = gGenerators[guessName];
	}
	return lg;
}
LuaGenerator * AddTextureGenerator( const char *guessName, const char *params ) {
	char name[128];
	char args[128]; args[0] = 0;
	strcpy( name, guessName );
	if( char *mid = strchr( name, ':' ) ) {
		strcpy( args, mid+1 );
		*mid = 0;
		params = args;
		//guessName = name;
	}
	LuaGenerator *lg = 0;
	if( gGenerators.count( guessName ) == 0 ) {
		char filename[128];
		lg = new LuaGenerator;
		sprintf( filename, "data/proctexture-%s.lua", name );
		lg->Load( filename );
		lg->GenTexture(params);
		string scriptName = lg->GetScriptName();
		Log( 1, "registering texture generator %s at %s because params (%s)\n", scriptName.c_str(), guessName, params?params:"<NULL>" );
		gGenerators[guessName] = lg;
	} else {
		lg = gGenerators[guessName];
	}
	return lg;
}

void UpdateProcGenTexture() {
	for( GenMap::iterator i = gGenerators.begin(); i != gGenerators.end(); ++i ) {
		i->second->Update();	
	}
}

