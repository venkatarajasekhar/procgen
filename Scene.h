#ifndef _SCENE_H_
#define _SCENE_H_

#include <string>
#include <vector>
using namespace std;
#include "core/geom.h"

struct Scene {
	struct Node {
		Node( string mesh, string texture, const Mat44 &orientation ) : o(orientation), m(mesh), t(texture), isScene(false), isCamera(false) {}
		Node( string scene, const Mat44 &orientation ) : o(orientation), s(scene), isScene(true), isCamera(false) {}
		Node( string mesh, string texture, string orientation_function ) : m(mesh), t(texture), o_func(orientation_function), isScene(false), isCamera(false) {}
		Node( const Mat44 &orientation ) : o(orientation), isScene(false), isCamera(true) {}
		Node( string orientation_function ) : o_func(orientation_function), isScene(false), isCamera(true) {}
		Mat44 o;
		string s,m,t;
		string o_func;
		bool isScene;
		bool isCamera;
		Mat44 GetOrientation();
	};
	typedef vector<Node> NodeVec;
	NodeVec m_nodes;
	void PushNode( string mesh, string texture, string orientationFunction );
	void PushNode( string mesh, string texture, const Mat44 &orientation );
	void PushNode( string scene, const Mat44 &orientation );
	void PushNode( string orientationFunction );
	void PushNode( const Mat44 &orientation );
	void Render( const Mat44 &rootTransform );
};

#endif

