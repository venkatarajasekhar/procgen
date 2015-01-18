#ifndef _LUA_FUNCS_H_
#define _LUA_FUNCS_H_

static int l_Log( lua_State *L ) {
	const char *str = lua_tostring( L, 1 );
	Log( 1, "LUA: %s\n", str );
	return 0;
}
static float Noise( int x, int y, int z ) {
	uint32_t seed = (uint32_t)( x + y * 1024 + z * 1024 * 1024 );
	uint32_t h = jenkins_one_at_a_time_hash( (char *)&seed, sizeof( seed ) );
	float r = (h&65535)/65535.0f;
	return r;
}
static float Easing( float x ) {
	return x*x*(3.0f-2.0f*x);
}
static float PerlinNoiseOctave( int x, int y, int cellsize, int seed ) {
	int dx = x%cellsize;
	int dy = y%cellsize;
	int basex = x - dx;
	int basey = y - dy;
	float fx = Easing( dx/(float)cellsize );
	float fy = Easing( dy/(float)cellsize );
	float a = Noise(basex,basey,seed);
	float b = Noise(basex+cellsize,basey,seed);
	float c = Noise(basex,basey+cellsize,seed);
	float d = Noise(basex+cellsize,basey+cellsize,seed);
	float ab = (a*(1-fx)+b*fx);
	float cd = (c*(1-fx)+d*fx);
	float abcd = (ab*(1-fy)+cd*fy);
	return abcd;
}
static float PerlinNoise( int x, int y, int low, int high, int seed ) {
	float r = 0;
	do {
		r = r/2 + PerlinNoiseOctave( x, y, low, seed );
		seed += 1;
		low *= 2;
	} while( low <= high );
	return r;
}
static int l_Noise( lua_State *L ) {
	uint32_t x = (uint32_t)lua_tonumber(L, 1);  /* get argument */
	uint32_t y = (uint32_t)lua_tonumber(L, 2);  /* get argument */
	uint32_t z = (uint32_t)lua_tonumber(L, 3);  /* get argument */
	lua_pushnumber(L, Noise(x,y,z));  /* push result */
	return 1;  /* number of results */
}
static int l_PerlinOctave( lua_State *L ) {
	uint32_t x = (uint32_t)lua_tonumber(L, 1);  /* get argument */
	uint32_t y = (uint32_t)lua_tonumber(L, 2);  /* get argument */
	uint32_t cellsize = (uint32_t)lua_tonumber(L, 3);  /* get argument */
	uint32_t seed = (uint32_t)lua_tonumber(L, 4);  /* get argument */
	lua_pushnumber(L, PerlinNoiseOctave(x,y,cellsize,seed));  /* push result */
	return 1;  /* number of results */
}
static int l_Perlin( lua_State *L ) {
	uint32_t x = (uint32_t)lua_tonumber(L, 1);  /* get argument */
	uint32_t y = (uint32_t)lua_tonumber(L, 2);  /* get argument */
	uint32_t low = (uint32_t)lua_tonumber(L, 3);  /* get argument */
	uint32_t high = (uint32_t)lua_tonumber(L, 4);  /* get argument */
	uint32_t seed = (uint32_t)lua_tonumber(L, 5);  /* get argument */
	lua_pushnumber(L, PerlinNoise(x,y,low,high,seed));  /* push result */
	return 1;  /* number of results */
}
static int l_Sin( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	lua_pushnumber(L, sin(x));  /* push result */
	return 1;  /* number of results */
}
static int l_Cos( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	lua_pushnumber(L, cos(x));  /* push result */
	return 1;  /* number of results */
}
static int l_Tan( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	lua_pushnumber(L, tan(x));  /* push result */
	return 1;  /* number of results */
}
static int l_ATan2( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	float y = (float)lua_tonumber(L, 2);  /* get argument */
	lua_pushnumber(L, atan2f(x,y));  /* push result */
	return 1;  /* number of results */
}
static int l_Sqrt( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	lua_pushnumber(L, sqrtf(x));  /* push result */
	return 1;  /* number of results */
}
#include "core/graphics.h"
static int l_GetTime( lua_State *L ) {
	double drawStart = glfwGetTime();
	lua_pushnumber(L, drawStart);  /* push result */
	return 1;  /* number of results */
}

#include "core/geom.h"
#include <vector>
static std::vector<Mat44> gMatrixStack;
static Mat44 gCurrentMatrix;
static int l_Identity( lua_State * ) {
	gCurrentMatrix.SetIdentity();
	return 0;  /* number of results */
}
static int l_Push( lua_State * ) {
	gMatrixStack.push_back(gCurrentMatrix);
	return 0;  /* number of results */
}
static int l_Pop( lua_State * ) {
	gCurrentMatrix = gMatrixStack.back();
	gMatrixStack.pop_back();
	return 0;  /* number of results */
}
static int l_Translate( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	float y = (float)lua_tonumber(L, 2);  /* get argument */
	float z = (float)lua_tonumber(L, 3);  /* get argument */
	gCurrentMatrix.Translate( Vec3( x, y, z ) );
	return 0;  /* number of results */
}
static int l_Rotate( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	float y = (float)lua_tonumber(L, 2);  /* get argument */
	float z = (float)lua_tonumber(L, 3);  /* get argument */
	gCurrentMatrix.Rotation( Vec3( x, y, z ) );
	return 0;  /* number of results */
}
static int l_Scale( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	float y = (float)lua_tonumber(L, 2);  /* get argument */
	float z = (float)lua_tonumber(L, 3);  /* get argument */
	gCurrentMatrix.Scale( Vec3( x, y, z ) );
	return 0;  /* number of results */
}
static int l_Transform( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	float y = (float)lua_tonumber(L, 2);  /* get argument */
	float z = (float)lua_tonumber(L, 3);  /* get argument */
	Vec4 r = gCurrentMatrix * Vec4( x, y, z, 1 );
	//Log(3, "Convert %f,%f,%f -> %f,%f,%f\n", x,y,z, r.x,r.y,r.z );
	//Log(3, "Using  %f,%f,%f,%f\n", gCurrentMatrix.x.x, gCurrentMatrix.x.y, gCurrentMatrix.x.z, gCurrentMatrix.x.w );
	//Log(3, "Using  %f,%f,%f,%f\n", gCurrentMatrix.y.x, gCurrentMatrix.y.y, gCurrentMatrix.y.z, gCurrentMatrix.y.w );
	//Log(3, "Using  %f,%f,%f,%f\n", gCurrentMatrix.z.x, gCurrentMatrix.z.y, gCurrentMatrix.z.z, gCurrentMatrix.z.w );
	//Log(3, "Using  %f,%f,%f,%f\n", gCurrentMatrix.w.x, gCurrentMatrix.w.y, gCurrentMatrix.w.z, gCurrentMatrix.w.w );
	lua_pushnumber(L, r.x);  /* push result */
	lua_pushnumber(L, r.y);  /* push result */
	lua_pushnumber(L, r.z);  /* push result */
	return 3;  /* number of results */
}


static void RegisterLuaFuncs( lua_State *L ) {
	lua_pushcfunction(L, l_Log ); lua_setglobal( L, "Log" );
	lua_pushcfunction(L, l_Noise); lua_setglobal(L, "Noise");
	lua_pushcfunction(L, l_PerlinOctave); lua_setglobal(L, "PerlinOctave");
	lua_pushcfunction(L, l_Perlin); lua_setglobal(L, "Perlin");
	lua_pushcfunction(L, l_Sin); lua_setglobal(L, "Sin");
	lua_pushcfunction(L, l_Cos); lua_setglobal(L, "Cos");
	lua_pushcfunction(L, l_Tan); lua_setglobal(L, "Tan");
	lua_pushcfunction(L, l_ATan2); lua_setglobal(L, "ATan2");
	lua_pushcfunction(L, l_Sqrt); lua_setglobal(L, "Sqrt");
	lua_pushcfunction(L, l_GetTime); lua_setglobal(L, "GetTime");

	lua_pushcfunction(L, l_Identity ); lua_setglobal( L, "Identity" );
	lua_pushcfunction(L, l_Push ); lua_setglobal( L, "Push" );
	lua_pushcfunction(L, l_Pop ); lua_setglobal( L, "Pop" );
	lua_pushcfunction(L, l_Translate ); lua_setglobal( L, "Translate" );
	lua_pushcfunction(L, l_Rotate ); lua_setglobal( L, "Rotate" );
	lua_pushcfunction(L, l_Scale ); lua_setglobal( L, "Scale" );
	lua_pushcfunction(L, l_Transform ); lua_setglobal( L, "Transform" );
}

#endif
