#ifndef _LUAGENERATOR_H_
#define _LUAGENERATOR_H_

#include <stdlib.h>
#include <string>
#include <map>
using namespace std;
#include "core/geom.h"

struct Scene;
struct lua_State;
struct BadMesh;
struct Image;
typedef unsigned int C32;

struct LuaGenerator {
	lua_State *L;
	Scene *scene;
	BadMesh *mesh;
	Image *texture;
	
	time_t lastFileTime;
	time_t currentFileTime;
	char m_filename[128];

	LuaGenerator();
	void ClearoutLuaState();
	~LuaGenerator();
	bool IsValid();
	void Load( const char *filename );
	void Update();
	bool FunctionExists( const char *functionName );
	int GetIntFromLua( const char *functionName );
	Vec3 GetVec3FromLua( const char *functionName, int vindex, Vec3 defaultValue = gZeroVec3 );
	Vec2 GetVec2FromLua( const char *functionName, int vindex );
	Mat44 GetMat44FromLua( const char *functionName );
	C32 GetColourFromLua( const char *functionName, int x, int y );
	string GetStringFromLua( const char *functionName );
	string GetStringFromLua( const char *functionName, int n );
	void PushParams( const char *params );

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
	string GetNodeOrientationFunction( int n ) { return GetStringFromLua( "GetSceneNodeOrientation", n ); }
	string GetNodeMesh( int n ) { return GetStringFromLua( "GetSceneNodeMesh", n ); }
	string GetNodeTexture( int n ) { return GetStringFromLua( "GetSceneNodeTexture", n ); }
	string GetNodeParams( int n ) { return GetStringFromLua( "GetSceneNodeParams", n ); }

	void GenScene( const char *params = 0 );
	void GenMesh( const char *params = 0 );
	void GenTexture( const char *params = 0 );
};

LuaGenerator * AddSceneGenerator( const char *guessName, const char *params = 0 );
LuaGenerator * AddMeshGenerator( const char *guessName, const char *params = 0 );
LuaGenerator * AddTextureGenerator( const char *guessName, const char *params = 0 );

void UpdateProcGenTexture();

extern LuaGenerator *gCurrentRenderGenerator;
typedef std::map<std::string, LuaGenerator*> GenMap;
extern GenMap gGenerators;
extern LuaGenerator *gRootScene;


#endif

