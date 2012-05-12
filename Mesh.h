/*
 * =====================================================================================
 *
 *       Filename:  Mesh.h
 *
 *    Description:  Trangle neighbor structure
 *
 *        Version:  1.0
 *        Created:  04/19/2012 19:41:35
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


class Vertex;
class Triangle;
class Mesh;

class Triangle{
	public:
		int nbr_id[3];
		int ver_id[3];
		int id;
		bool isNormal;
		bool isNbr;
		bool nbrFound[3];
		bool isFlat;
		Vector3D faceNormal;
		Triangle(int x=0, int y=0, int z=0, int d=0)
		{
			ver_id[0]=x;
			ver_id[1]=y;
			ver_id[2]=z;
			id=d;
			isFlat=false;
			isNormal=false;
			isNbr=false;
			for(int i=0;i<=2;++i)
				nbrFound[i]=false;

		}
		void displayTriangle();
};

class Vertex
{
	public:
		Vector3D point;
		Vector3D normal[3]; 
		// normal1 is average of adjacent trangles
		// norma2 is average of adjacent trangles, each weighted by its trangle area
		// normal3 is average of adjacent trangles, each weighted by angle of incident triangle edges
		int id;
		bool isSharp;
		bool isNormal[3];
		int tri_id;         //id of any triangle the vertex point to
		Vertex(float x=0.0, float y=0.0, float z=0.0, int _id=0)
		{
			id=_id;
			Vector3D xx(x,y,z);
			point=xx;
			isSharp=false;
			for(int i=0; i<=3;++i)
				isNormal[i]=false;

		}
		void displayVertex();

};

class Mesh
{
	public:
		vector<Triangle> triangles;
		vector<Vertex> vertices;
		Mesh() {}
		void loadFile(char *fname);
		void displayMesh();
		void setFaceNormal();
		void findNeighbor();
		vector<int> faceOfVertex(int vertex);
		float area(int i);
};
void setVertexNormal(Mesh &);
void StringSplit(string str, string separator, vector<string>* results);
