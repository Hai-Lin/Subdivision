/*
 * =====================================================================================
 *
 *       Filename:  Mesh.h
 *
 *    Description:  Half Edge structure Mesh
 *
 *        Version:  1.0
 *        Created:  05/10/2012 19:41:35
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Hai Lin, 
 *   Organization:  Columbia
 *
 * =====================================================================================
 */

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include"Vector3D.h"
#include<algorithm>
using namespace std;

class Edge;
class Vertex;
class Face;
class Mesh;

class Edge
{
	public:
		int id, pair_id, next_id, vertex_id, face_id;
		Edge(int _id=-1,  int vertex=-1, int face=-1, int pair=-1 )
		{
			id=_id;
			vertex_id=vertex;
			face_id=face;
			pair_id=pair;
		}
		void displayEdge();
		~Edge() {}
};

class Face{
	public:
		vector<int> ver_id;
		int id, edge_id;
		Edge faceEdge; //andy incident h-edge
		Vector3D faceNormal;
		bool isNormal;
		Face( int d=0)
		{
			id=d;
		}
		void displayFace();
};

class Vertex
{
	public:
		Vector3D point;
		Vector3D normal; 
		// normal3 is average of adjacent trangles, each weighted by angle of incident triangle edges
		int id, edge_id; // id of half edge
		bool isNormal, isBoundary;
		Vertex(float x=0.0, float y=0.0, float z=0.0, int _id=0, int edge=-1)
		{
			id=_id;
			edge_id=edge;
			Vector3D xx(x,y,z);
			point=xx;
			isNormal=false;
			isBoundary=false;
		}
		void displayVertex();

};

class Mesh
{
	public:
		vector<Face> faces;
		vector<Vertex> vertices;
		vector<Edge> edges;
		int type;  // 0 is Loop subdivision, 1 is 
		Mesh() {}
		void loadFile(char *fname);
		void displayMesh();
		void setFaceNormal();
		int previousEdge(int edge);
		vector<int> edgesOfVertex(int vertex);
		vector<int> edgesOfFace(int face);
};
void setPairEdge(Mesh &);
void setVertexNormal(Mesh &);
void StringSplit(string str, string separator, vector<string>* results);
