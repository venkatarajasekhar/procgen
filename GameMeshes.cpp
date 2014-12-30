#include "GameMeshes.h"
#include "core/util.h"
#include "core/BadMesh.h"
//#include "core/assetloader.h"
//#include "core/graphics.h"
#include <map>

namespace GameMeshes {
	std::map<std::string,BadMesh*> meshes;

	void LoadOne( const char *name, const char *filename, float scale = 1.0f, bool xzyCorrect = false, bool genUVs = true ) {
		Mat44 correction = gIdentityMat;
		if( xzyCorrect )
			correction.RotX( -M_PI_2 );
		correction.Scale(scale);
		BadMesh *m = new BadMesh();
		Log( 3, "Loading %s\n", filename );
		m->Load(filename, correction );
		meshes[name] = m;
		if( genUVs )
			m->UVsFromBB();
	}

	void Init() {
		Log( 1, "loading meshes\n" );

		// default mesh if things go wrong
		BadMesh *cube = new BadMesh();
		cube->SetAsCube();
		cube->UVsFromBB();
		meshes["cube"] = cube;
	}
	void Shutdown() {
	}
	BadMesh *Get( const std::string &str ) {
		if( meshes.count( str ) ) {
			return meshes[ str ];
		}
		return meshes["cube"];
	}
	void Add( const std::string &str, BadMesh *mesh ) {
		if( meshes.count( str ) ) {
			eprintf( "adding a mesh where there already is one [%s]\n", str.c_str() );
		} else {
			meshes[ str ] = mesh;
		}
	}
};

