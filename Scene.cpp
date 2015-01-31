#include "Scene.h" 
#include "core/util.h"
#include "LuaGenerator.h"
#include "core/graphics.h"
#include "core/BadMesh.h"

Mat44 Scene::Node::GetOrientation() {
	if( o_func.size() ) {
		if( gCurrentRenderGenerator ) {
			o = gCurrentRenderGenerator->GetMat44FromLua( o_func.c_str() );
		}
	}
	return o;
}

void Scene::PushNode( string mesh, string texture, string orientationFunction ) {
	m_nodes.push_back( Node( mesh, texture, orientationFunction ) );
}
void Scene::PushNode( string mesh, string texture, const Mat44 &orientation ) {
	m_nodes.push_back( Node( mesh, texture, orientation ) );
}
void Scene::PushNode( string scene, const Mat44 &orientation ) {
	m_nodes.push_back( Node( scene, orientation ) );
}
void Scene::PushNode( string orientationFunction ) {
	m_nodes.push_back( Node( orientationFunction ) );
}
void Scene::PushNode( const Mat44 &orientation ) {
	m_nodes.push_back( Node( orientation ) );
}

void Scene::Render( const Mat44 &rootTransform ) {
	//Log( 3, "Scene::Render this=%x\n", this);
	for( NodeVec::iterator i = m_nodes.begin(); i != m_nodes.end(); ++i ) {
		Mat44 o = i->GetOrientation();
		Mat44 transform = o * rootTransform;
		if( i->isScene ) {
			//Log( 3, "Found scene, adding \"%s\"\n", i->s.c_str() );
			if( LuaGenerator *s = AddSceneGenerator( i->s.c_str() ) ) {
				LuaGenerator *old = gCurrentRenderGenerator;
				gCurrentRenderGenerator = s;
				if ( Scene *scene = s->scene ) {
					scene->Render( transform );
				} else {
					Log( 3, "Failed to find scene \"%s\"\n", i->s.c_str() );
				}
				gCurrentRenderGenerator = old;
			} else {
				Log( 3, "Failed to load scene \"%s\"\n", i->s.c_str() );
			}
		} else {
			if( i->isCamera ) {
				Mat44 invCam = transform.Inverse();
				SetCamera( invCam );
			} else {
				if( LuaGenerator *m = AddMeshGenerator(i->m.c_str() ) ) {
					AddTextureGenerator(i->t.c_str());

					SetTexture( i->t.c_str(), 0 );
					SetModel( transform );
					if( BadMesh *bm = m->mesh ) {
						bm->DrawTriangles();
					}
				}
			}
		}
	}
}

void Scene::Update() {
	Log( 3, "Scene::Update this=%x\n", this);
	if( gCurrentRenderGenerator ) {
		gCurrentRenderGenerator->CallUpdate();
	}
	for( NodeVec::iterator i = m_nodes.begin(); i != m_nodes.end(); ++i ) {
		if( i->isScene ) {
			//Log( 3, "Found scene, adding \"%s\"\n", i->s.c_str() );
			if( LuaGenerator *s = AddSceneGenerator( i->s.c_str() ) ) {
				LuaGenerator *old = gCurrentRenderGenerator;
				gCurrentRenderGenerator = s;
				if ( Scene *scene = s->scene ) {
					scene->Update();
				} else {
					Log( 3, "Failed to find scene \"%s\"\n", i->s.c_str() );
				}
				gCurrentRenderGenerator = old;
			} else {
				Log( 3, "Failed to load scene \"%s\"\n", i->s.c_str() );
			}
		}
	}
}

