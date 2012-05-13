#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "Vector3D.h"
#include "Vector2D.h"
#include <algorithm>
#include <string>

using namespace std;

class Edge;
class Vertex;
class Triangle;
class Normal;

class Vertex {
public:
	Vector3D position;
	Vector3D normal;
	bool has_normal;
	int id;
	int edge_id;
	Vertex(Vector3D p, int _id);
	
	~Vertex(){}
};

class Normal {
public:
	Vector3D normal;
	int id;
	Normal(Vector3D n, int _id);
	~Normal(){}
};

class Triangle {
public:
	int edge_id;
	int normal_id[3];
	int texture_id[3];
	int id;
	string group;
	int mat;
	Vector3D face_normal;
	Triangle();
	void set_n(int n0, int n1, int n2, int _id);
	void set_t(int t0, int t1, int t2, int _id);
	void set_nt(int n0, int n1, int n2, int t0, int t1, int t2, int _id);
	void calFaceNormal(Vertex v1, Vertex v2, Vertex v3);
	~Triangle() {}
};

class Edge {
public:
	int pair_id;
	int next_id;
	int id;
	int vertex_id;
	int triangle_id;
	int multicase;
	Edge() {}
	~Edge() {}
};

class Mesh {
public:
	int m_nTriangle;
	int m_nVertex;
	int m_nNormal;
	int m_nTexture;
	int m_nEdge;
	int m_nGroup;
	float x_max;
	float x_min;
	float y_max;
	float y_min;
	float z_max;
	float z_min;
	vector<Triangle> triangles;
	vector<Vertex> vertex;
	vector<Normal> normals;
	vector<Texture> textures;
	vector<Edge> edges;
	vector<Material> mats;
	vector<FaceGroup> groups;
	void draw();
	Mesh();
	void loadMat(char *mtlfname);
	void setMatIllum();
	//int loadFile(char *fname, int query = 0, int _mat = 0);
	int loadFile(char *fname, int _mat = 0);
	void setNormal();
	void EdgeOfFace(int tindex);
	void FaceOfFace(int tindex);
	void EdgeOfVertex(int vid);
	void setVNormal(int vid);
	//void mergeGroup();
	void addToGroup(int tid, string gname);
	~Mesh(){}
};

#endif
