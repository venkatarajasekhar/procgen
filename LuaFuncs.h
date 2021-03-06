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
static float Noise( int x, int y, int z, int w ) {
	uint32_t seed = (uint32_t)( x + y * 1024 + z * 1024 * 1024 + w * 1024 * 1024 * 1024 );
	uint32_t h = jenkins_one_at_a_time_hash( (char *)&seed, sizeof( seed ) );
	float r = (h&65535)/65535.0f;
	return r;
}
static float Easing( float x ) {
	return x*x*(3.0f-2.0f*x);
}
static float PerlinNoiseOctave( float x, float y, int cellsize, int seed ) {
	float dx = fmod(x,cellsize);
	float dy = fmod(y,cellsize);
	int basex = x - dx;
	int basey = y - dy;
	float fx = Easing( dx/cellsize );
	float fy = Easing( dy/cellsize );
	float a = Noise(basex,basey,seed);
	float b = Noise(basex+cellsize,basey,seed);
	float c = Noise(basex,basey+cellsize,seed);
	float d = Noise(basex+cellsize,basey+cellsize,seed);
	float ab = (a*(1-fx)+b*fx);
	float cd = (c*(1-fx)+d*fx);
	float abcd = (ab*(1-fy)+cd*fy);
	return abcd;
}
static float PerlinNoise3Octave( float x, float y, float z, int cellsize, int seed ) {
	float dx = fmod(x,cellsize);
	float dy = fmod(y,cellsize);
	float dz = fmod(z,cellsize);
	int basex = x - dx;
	int basey = y - dy;
	int basez = z - dz;
	float fx = Easing( dx/cellsize );
	float fy = Easing( dy/cellsize );
	float fz = Easing( dz/cellsize );
	float a = Noise(basex,basey,basez,seed);
	float b = Noise(basex+cellsize,basey,basez,seed);
	float c = Noise(basex,basey+cellsize,basez,seed);
	float d = Noise(basex+cellsize,basey+cellsize,basez,seed);
	float e = Noise(basex,basey,basez+cellsize,seed);
	float f = Noise(basex+cellsize,basey,basez+cellsize,seed);
	float g = Noise(basex,basey+cellsize,basez+cellsize,seed);
	float h = Noise(basex+cellsize,basey+cellsize,basez+cellsize,seed);
	float ab = (a*(1-fx)+b*fx);
	float cd = (c*(1-fx)+d*fx);
	float ef = (e*(1-fx)+f*fx);
	float gh = (g*(1-fx)+h*fx);
	float abcd = (ab*(1-fy)+cd*fy);
	float efgh = (ef*(1-fy)+gh*fy);
	float abcdefgh = (abcd*(1-fz)+efgh*fz);
	return abcdefgh;
}
static float PerlinNoise( float x, float y, int low, int high, int seed ) {
	float r = 0;
	do {
		r = r/2 + PerlinNoiseOctave( x, y, low, seed );
		seed += 1;
		low *= 2;
	} while( low <= high );
	return r;
}
static float PerlinNoise3( float x, float y, float z, int low, int high, int seed ) {
	float r = 0;
	do {
		r = r/2 + PerlinNoise3Octave( x, y, z, low, seed );
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
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	float y = (float)lua_tonumber(L, 2);  /* get argument */
	uint32_t low = (uint32_t)lua_tonumber(L, 3);  /* get argument */
	uint32_t high = (uint32_t)lua_tonumber(L, 4);  /* get argument */
	uint32_t seed = (uint32_t)lua_tonumber(L, 5);  /* get argument */
	lua_pushnumber(L, PerlinNoise(x,y,low,high,seed));  /* push result */
	return 1;  /* number of results */
}
static int l_Perlin3( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	float y = (float)lua_tonumber(L, 2);  /* get argument */
	float z = (float)lua_tonumber(L, 3);  /* get argument */
	uint32_t low = (uint32_t)lua_tonumber(L, 4);  /* get argument */
	uint32_t high = (uint32_t)lua_tonumber(L, 5);  /* get argument */
	uint32_t seed = (uint32_t)lua_tonumber(L, 6);  /* get argument */
	lua_pushnumber(L, PerlinNoise3(x,y,z,low,high,seed));  /* push result */
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
static int l_BitWeight( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	int i = x+0.5;
	int weight = bitweight14( i );
	lua_pushnumber(L, weight);  /* push result */
	return 1;  /* number of results */
}
static int l_band( lua_State *L ) {
	float a = (float)lua_tonumber(L, 1);  /* get argument */
	float b = (float)lua_tonumber(L, 2);  /* get argument */
	int ia = a+0.5;
	int ib = b+0.5;
	lua_pushnumber(L, ia&ib);  /* push result */
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
static int l_Mirror( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	int axis = x+0.5;
	switch( axis ) {
		case 0: // x
			gCurrentMatrix.x.x *= -1.0f;
			gCurrentMatrix.y.x *= -1.0f;
			gCurrentMatrix.z.x *= -1.0f;
		break;
		case 1: // y
			gCurrentMatrix.x.y *= -1.0f;
			gCurrentMatrix.y.y *= -1.0f;
			gCurrentMatrix.z.y *= -1.0f;
		break;
		case 2: // z
			gCurrentMatrix.x.z *= -1.0f;
			gCurrentMatrix.y.z *= -1.0f;
			gCurrentMatrix.z.z *= -1.0f;
		break;
	}
	return 0;  /* number of results */
}
static int l_Rotate90( lua_State *L ) {
	float x = (float)lua_tonumber(L, 1);  /* get argument */
	int axis = x+0.5;
	switch( axis ) {
		case 0: // x
			gCurrentMatrix = gCurrentMatrix * Mat44(
					Vec4(1,0,0,0),
					Vec4(0,0,1,0),
					Vec4(0,-1,0,0),
					Vec4(0,0,0,1) );
		break;
		case 1: // y
		{
			gCurrentMatrix = gCurrentMatrix * Mat44(
					Vec4(0,0,-1,0),
					Vec4(0,1,0,0),
					Vec4(1,0,0,0),
					Vec4(0,0,0,1) );
		}
		break;
		case 2: // z
		{
			gCurrentMatrix = gCurrentMatrix * Mat44(
					Vec4(0,1,0,0),
					Vec4(-1,0,0,0),
					Vec4(0,0,1,0),
					Vec4(0,0,0,1) );
		}
		break;
	}
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
	int args = lua_gettop(L);
	if( args == 3 ) {
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
		//Log(3, "Input  %f,%f,%f\n", x,y,z );
		//Log(3, "Output %f,%f,%f,%f\n", r.x,r.y,r.z,r.w );
		return 3;  /* number of results */
	} else if( args == 1 ) {
		luaL_checktype(L, 1, LUA_TTABLE);
		lua_rawgeti(L, 1, 1);
		lua_rawgeti(L, 1, 2);
		lua_rawgeti(L, 1, 3);
		float x = (float)lua_tonumber(L, -3);  /* get argument */
		float y = (float)lua_tonumber(L, -2);  /* get argument */
		float z = (float)lua_tonumber(L, -1);  /* get argument */
		Vec4 r = gCurrentMatrix * Vec4( x, y, z, 1 );

		lua_settop(L,1);
		//lua_newtable(L);
		lua_pushnumber(L,r.x);
		lua_rawseti(L,-2,1);
		lua_pushnumber(L,r.y);
		lua_rawseti(L,-2,2);
		lua_pushnumber(L,r.z);
		lua_rawseti(L,-2,3);

		//Log(3, "Input  %f,%f,%f\n", x,y,z );
		//Log(3, "Output %f,%f,%f,%f\n", r.x,r.y,r.z,r.w );
		return 1;
	} else {
		Log(3, "Invalid argument count in Transform\n" );
		return 0;
	}
}

static int l_PrintCurrentMatrix( lua_State * ) {
		Log(3, "MAT44 X %f,%f,%f,%f\n", gCurrentMatrix.x.x, gCurrentMatrix.x.y, gCurrentMatrix.x.z, gCurrentMatrix.x.w );
		Log(3, "MAT44 Y %f,%f,%f,%f\n", gCurrentMatrix.y.x, gCurrentMatrix.y.y, gCurrentMatrix.y.z, gCurrentMatrix.y.w );
		Log(3, "MAT44 Z %f,%f,%f,%f\n", gCurrentMatrix.z.x, gCurrentMatrix.z.y, gCurrentMatrix.z.z, gCurrentMatrix.z.w );
		Log(3, "MAT44 W %f,%f,%f,%f\n", gCurrentMatrix.w.x, gCurrentMatrix.w.y, gCurrentMatrix.w.z, gCurrentMatrix.w.w );
	return 0;
}

#include "mc.h"

static int l_MCLookup( lua_State *L ) {
	int cubeID = 0.5 + (float)lua_tonumber(L, 1);  /* get argument */
	int mesh = mc_mesh[cubeID&255];
	int orientation = mc_orientation[cubeID&255];
	lua_pushnumber(L, mesh);  /* push result */
	lua_pushnumber(L, orientation);  /* push result */
	return 2;  /* number of results */
}

#include "pbmc.h"

static int l_MCLookup2( lua_State *L ) {
	int cubeID = 0.5 + (float)lua_tonumber(L, 1);  /* get argument */
	const int *triangles = pbmc_triangle[cubeID&255];
	lua_settop(L,1);
	lua_newtable(L);
	int i = 0;
	for( ; i < 16; ++i ) {
		int edge = triangles[i];
		if( edge == -1 )
			break;
		lua_pushnumber(L,edge);
		lua_rawseti(L,-2,i+1);
	}
	lua_pushnumber(L,i/3);
	return 2;  /* number of results */
}

static void RegisterLuaFuncs( lua_State *L ) {
	lua_pushcfunction(L, l_Log ); lua_setglobal( L, "Log" );
	lua_pushcfunction(L, l_Noise); lua_setglobal(L, "Noise");
	lua_pushcfunction(L, l_PerlinOctave); lua_setglobal(L, "PerlinOctave");
	lua_pushcfunction(L, l_Perlin); lua_setglobal(L, "Perlin");
	lua_pushcfunction(L, l_Perlin3); lua_setglobal(L, "Perlin3");
	lua_pushcfunction(L, l_Sin); lua_setglobal(L, "Sin");
	lua_pushcfunction(L, l_Cos); lua_setglobal(L, "Cos");
	lua_pushcfunction(L, l_Tan); lua_setglobal(L, "Tan");
	lua_pushcfunction(L, l_ATan2); lua_setglobal(L, "ATan2");
	lua_pushcfunction(L, l_Sqrt); lua_setglobal(L, "Sqrt");
	lua_pushcfunction(L, l_GetTime); lua_setglobal(L, "GetTime");
	lua_pushcfunction(L, l_BitWeight); lua_setglobal(L, "BitWeight");
	lua_pushcfunction(L, l_band); lua_setglobal(L, "band");
	lua_pushcfunction(L, l_PrintCurrentMatrix); lua_setglobal(L, "PrintCurrentMatrix");

	lua_pushcfunction(L, l_Identity ); lua_setglobal( L, "Identity" );
	lua_pushcfunction(L, l_Push ); lua_setglobal( L, "Push" );
	lua_pushcfunction(L, l_Pop ); lua_setglobal( L, "Pop" );
	lua_pushcfunction(L, l_Translate ); lua_setglobal( L, "Translate" );
	lua_pushcfunction(L, l_Mirror ); lua_setglobal( L, "Mirror" );
	lua_pushcfunction(L, l_Rotate90 ); lua_setglobal( L, "Rotate90" );
	lua_pushcfunction(L, l_Rotate ); lua_setglobal( L, "Rotate" );
	lua_pushcfunction(L, l_Scale ); lua_setglobal( L, "Scale" );
	lua_pushcfunction(L, l_Transform ); lua_setglobal( L, "Transform" );

	lua_pushcfunction(L, l_MCLookup ); lua_setglobal( L, "MCLookup" );
	lua_pushcfunction(L, l_MCLookup2 ); lua_setglobal( L, "MCLookup2" );
}

#endif
