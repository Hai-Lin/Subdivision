#include <iostream>
#include <fstream>
#include <string.h>
#include "Vector3D.h"
#include "Mesh.h"
#include <assert.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>

using namespace std;

void StringSplit(string str, string separator, vector<string>* results){
    int found;
    found = str.find_first_of(separator);
    while(found != string::npos){
        if(found > 0){
            results->push_back(str.substr(0,found));
        }
        str = str.substr(found+1);
        found = str.find_first_of(separator);
    }
    if(str.length() > 0){
        results->push_back(str);
    }
}

Material::Material() {
	for (int i = 0; i < 3; i++) {
		ka_ambient[i] = 0.2;
		kd_diffuse[i] = 0.8;
		ks_specular[i] = 1.0;
		tf_transmission[i] = 1.0;
		ke_emission[i] = 0.0;
	}
	d_Tr_alpha = 1.0;
	Ns_shininess  = 0.0;
	Ni_refelection = 1.0;
	sharpness = 0.0;
	density = 1.0;
	illum = 2;
	name.clear();
	map_ka.clear();
	map_kd.clear();
	map_ks.clear();
	map_ke.clear();
	map_ns.clear();
	map_d.clear();
	map_bump.clear();
}

void Material::clear() {
	for (int i = 0; i < 3; i++) {
		ka_ambient[i] = 0.2;
		kd_diffuse[i] = 0.8;
		ks_specular[i] = 1.0;
		tf_transmission[i] = 1.0;
		ke_emission[i] = 0.0;
	}
	d_Tr_alpha = 1.0;
	Ns_shininess  = 0.0;
	Ni_refelection = 1.0;
	sharpness = 0.0;
	density = 1.0;
	illum = 2;
	name.clear();
	map_ka.clear();
	map_kd.clear();
	map_ks.clear();
	map_ke.clear();
	map_ns.clear();
	map_d.clear();
	map_bump.clear();
}



Triangle::Triangle() {
	edge_id = -1;
	normal_id[0] = -1;
	normal_id[1] = -1;
	normal_id[2] = -1;
	group.clear(); 
	mat = -1;
	
}

void Triangle::calFaceNormal(Vertex v1, Vertex v2, Vertex v3) {
	Vector3D dir1 = v2.position - v1.position;
	Vector3D dir2 = v3.position - v1.position;
	Vector3D n = dir1 * dir2;
	face_normal = normalize(n);
}

void Triangle::set_n(int n0, int n1, int n2, int _id) {
	normal_id[0] = n0;
	normal_id[1] = n1;
	normal_id[2] = n2;
	id = _id;
}

void Triangle::set_t(int t0, int t1, int t2, int _id) {
	texture_id[0] = t0;
	texture_id[1] = t1;
	texture_id[2] = t2;
	id = _id;
}

void Triangle::set_nt(int n0, int n1, int n2, int t0, int t1, int t2, int _id) {
	normal_id[0] = n0;
	normal_id[1] = n1;
	normal_id[2] = n2;
	texture_id[0] = t0;
	texture_id[1] = t1;
	texture_id[2] = t2;
	id = _id;
}


Vertex::Vertex(Vector3D p, int _id) {
	position = p;
	id = _id;
	has_normal = false;
	edge_id = -1;
}

Texture::Texture(Vector2D t, int _id) {
	texture = t;
	id = _id;
}

Normal::Normal(Vector3D n, int _id) {
	normal = n;
	id = _id;
}

Mesh::Mesh() {
	Material material;
	mats.push_back(material);
	m_nTriangle = 0;
	m_nVertex = 0;
	m_nNormal = 0;
	m_nTexture = 0;
	m_nEdge = 0;
	m_nGroup = 0;
	x_max = 0.0; x_min = 0.0;
	y_max = 0.0; y_min = 0.0;
	z_max = 0.0; z_min = 0.0;
}

void Mesh::EdgeOfVertex(int vid) {
	for (int i = 0; i < m_nEdge; i++) {
		if (edges[i].vertex_id == vid && !edges[i].multicase) {
			cout << edges[edges[i].pair_id].vertex_id + 1 << " ";
		}
	}
	cout << endl;
}

void Mesh::EdgeOfFace(int tindex) {
	int eid = triangles[tindex].edge_id;
	int start_eid = eid;
	do {
		cout << edges[edges[eid].pair_id].vertex_id + 1 << " ";
		eid = edges[eid].next_id;
	} while (eid != start_eid);
	cout << endl;
}

void Mesh::FaceOfFace(int tindex) {
	int eid = triangles[tindex].edge_id;
	int start_eid = eid;
	int pair = -1;
	int tid = -1;
	do {
		pair = edges[eid].pair_id;
		tid = edges[pair].triangle_id;
		if (tid != -1)
			cout << tid + 1 << " ";
		eid = edges[eid].next_id;
	} while (eid != start_eid);
	cout << endl;
}

void Mesh::setVNormal(int vid) {
	bool has_normal = vertex[vid].has_normal;
	if (has_normal) return;
	Vector3D n;
	for (int i = 0; i < m_nEdge; i = i + 2) {
		if (edges[i].vertex_id == vid) {
			int tid = edges[i].triangle_id;
			if (tid != -1)
				n = n + triangles[tid].face_normal;
		}
	}
	vertex[vid].normal = normalize(n);
	vertex[vid].has_normal = true;
}


void Mesh::setNormal() {
	int eid, start_eid;
	int vid;
	for (int i = 0; i < m_nTriangle; i++) {
		if (triangles[i].normal_id[0] == -1) {
			start_eid = triangles[i].edge_id;
			eid = start_eid;
			do {
				vid = edges[eid].vertex_id;
				setVNormal(vid);
				eid = edges[eid].next_id;
			} while (eid != start_eid);
		}
	}
}

int Mesh::loadFile(char *fname, int _mat) {
	ifstream fin(fname);
	assert(!fin.fail());
	string line;
	m_nVertex = 0;
	m_nNormal = 0;
	m_nTriangle = 0;
	m_nTexture = 0;
	m_nEdge = 0;
	m_nGroup = 0;
	float vdata[3];
	float vtexture[2];
	float vnormal[3];
	vector<int> vindex;
	vector<int> tindex;
	vector<int> nindex;
	bool texture_flag = false;
	bool normal_flag = false;
	int return_normal = 1;
	vector<string> res;
	vector<string> face_para_test;
	vector<string> face_para;
	string sep1 = " ";
	string sep2 = "/";
	string sep3 = "//";
	int temp_para;
	int res_size;
	string group = "default";
	int mat = 0;
	int first = 1;
	while (getline(fin, line)) {
		res.clear();
		texture_flag = false;
		normal_flag = false;
		StringSplit(line, sep1, &res);
		if (res.size() == 0)
			continue;
		res_size = res.size();
		while (!res[res_size - 1].compare("\r")) {
			res.pop_back();
			res_size--;
		}
		if (res.size() == 0)
			continue;
		if (!res[0].compare("v")) {
			for (int i = 1; i < res.size(); i++) {
				vdata[i - 1] = atof(res[i].c_str());
			}
			if (first) {
				x_max = vdata[0]; x_min = vdata[0];
				y_max = vdata[1]; y_min = vdata[1];
				z_max = vdata[2]; z_min = vdata[2];
				first = 0;
			}
			else {
				if (vdata[0] > x_max) x_max = vdata[0];
				if (vdata[0] < x_min) x_min = vdata[0];
				if (vdata[1] > y_max) y_max = vdata[1];
				if (vdata[1] < y_min) y_min = vdata[1];
				if (vdata[2] > z_max) z_max = vdata[2];
				if (vdata[2] < z_min) z_min = vdata[2];
			}
			Vertex tempv = Vertex(Vector3D(vdata[0], vdata[1], vdata[2]), m_nVertex);
			m_nVertex++;
			vertex.push_back(tempv);
		}
		else if (!res[0].compare("vt")) {
			for (int i = 1; i < res.size(); i++) {
				vtexture[i - 1] = atof(res[i].c_str());
			}
			Texture tempt = Texture(Vector2D(vtexture[0], vtexture[1]), m_nTexture);
			m_nTexture++;
			textures.push_back(tempt);
		}
		else if (!res[0].compare("vn")) {
			for (int i = 1; i < res.size(); i++) {
				vnormal[i - 1] = atof(res[i].c_str());
			}
			Normal tempn = Normal(Vector3D(vnormal[0], vnormal[1], vnormal[2]), m_nNormal);
			m_nNormal++;
			normals.push_back(tempn);
		}
		else if (!res[0].compare("f")) {
			vindex.clear();
			tindex.clear();
			nindex.clear();
			for (int i = 1; i < res.size(); i++) {
				face_para.clear();
				StringSplit(res[i], sep2, &face_para);
				if (face_para.size() == 1) {
					if (face_para[0].length() > 0) {
						int ind = atoi(face_para[0].c_str()) - 1;
						vindex.push_back(ind);
					}
					return_normal = 0;
				}
				else if (face_para.size() == 2) {
					face_para_test.clear();
					StringSplit(res[i], sep3, &face_para_test);
					int size_test = face_para_test.size();
					if (size_test == 2) {
						if (face_para[0].length() > 0) {
							int ind = atoi(face_para[0].c_str()) - 1;
							vindex.push_back(ind);
						}
						if (face_para[1].length() > 0) {
							int ind = atoi(face_para[1].c_str()) - 1;
							nindex.push_back(ind);
							normal_flag = true;
						}
					}
					else if (size_test == 1) {
						return_normal = 0;
						if (face_para[0].length() > 0) {
							int ind = atoi(face_para[0].c_str()) - 1;
							vindex.push_back(ind);
						}
						if (face_para[1].length() > 0) {
							int ind = atoi(face_para[1].c_str()) - 1;
							tindex.push_back(ind);
							texture_flag = true;
						}
					}
				}
				else if (face_para.size() == 3) {
					if (face_para[0].length() > 0) {
						int ind = atoi(face_para[0].c_str()) - 1;
						vindex.push_back(ind);
					}
					if (face_para[1].length() > 0) {
						int ind = atoi(face_para[1].c_str()) - 1;
						tindex.push_back(ind);
						texture_flag = true;
					}
					if (face_para[2].length() > 0) {
						int ind = atoi(face_para[2].c_str()) - 1;
						nindex.push_back(ind);
						normal_flag = true;
					}
				}
			}
			int face_size = vindex.size();
			if (face_size > 0) {
				for (int i = 1; i < face_size - 1; i++) {
					
					Triangle temptt;
					int tid = m_nTriangle;
					if (texture_flag && normal_flag) {
						temptt.set_nt(nindex[i], nindex[i + 1], nindex[0], tindex[i], tindex[i + 1], tindex[0], m_nTriangle++);
					}
					else if (normal_flag) {
						temptt.set_n(nindex[i], nindex[i + 1], nindex[0], m_nTriangle++);
					}
					else if (texture_flag) {
						temptt.set_t(tindex[i], tindex[i + 1], tindex[0], m_nTriangle++);
					}
					else {
						m_nTriangle++;
					}
					
					if (_mat) {
						temptt.mat = mat;
					}
					else {
						temptt.mat = 0;
					}
					
					temptt.calFaceNormal(vertex[vindex[0]], vertex[vindex[i]], vertex[vindex[i + 1]]);
					
					addToGroup(tid, group);
					
					
					bool v01_flag = false;
					bool v20_flag = false;
					bool v12_flag = false;
					bool v01_mul = false;
					bool v20_mul = false;
					bool v12_mul = false;
					int iv0 = vindex[0];
					int iv1 = vindex[i];
					int iv2 = vindex[i + 1];
					int i01 = -1;
					int i12 = -1;
					int i20 = -1;
					
					for (int j = 0; j < m_nEdge; j++) {
						if (edges[j].vertex_id == iv1  && edges[edges[j].pair_id].vertex_id == iv0 && edges[j].next_id == -1) {
							v01_flag = true;
							i01 = j;
						}
						if (edges[j].vertex_id == iv1  && edges[edges[j].pair_id].vertex_id == iv0 && edges[j].next_id != -1) {
							v01_mul = true;
							//i01 = j;
						}
						if (edges[j].vertex_id == iv2  && edges[edges[j].pair_id].vertex_id == iv1 && edges[j].next_id == -1) {
							v12_flag = true;
							i12 = j;
						}
						if (edges[j].vertex_id == iv2  && edges[edges[j].pair_id].vertex_id == iv1 && edges[j].next_id != -1) {
							v12_mul = true;
							//i12 = j;
						}
						if (edges[j].vertex_id == iv0  && edges[edges[j].pair_id].vertex_id == iv2 && edges[j].next_id == -1) {
							v20_flag = true;
							i20 = j;
						}
						if (edges[j].vertex_id == iv0  && edges[edges[j].pair_id].vertex_id == iv2 && edges[j].next_id != -1) {
							v20_mul = true;
							//i20 = j;
						}
					}
									
					if (v01_flag) {
						edges[i01].triangle_id = tid;
						temptt.edge_id = i01;
						if (v12_flag) edges[i01].next_id = i12;
						else edges[i01].next_id = m_nEdge;
					}
					else {
						Edge e1, e2;
						e1.vertex_id = iv1;
						e2.vertex_id = iv0;
						e1.id = m_nEdge;
						e2.pair_id = m_nEdge;
						m_nEdge++;
						e2.id = m_nEdge;
						e1.pair_id = m_nEdge;
						m_nEdge++;
						e1.triangle_id = tid;
						e2.triangle_id = -1;
						temptt.edge_id = e1.id;
						if (v12_flag) e1.next_id = i12;
						else e1.next_id = m_nEdge;
						e2.next_id = -1;
						e1.multicase = v01_mul;
						e2.multicase = v01_mul;
						edges.push_back(e1);
						edges.push_back(e2);
						i01 = e1.id;
						if (vertex[iv0].edge_id == -1) vertex[iv0].edge_id = e1.id;
					}

					if (v12_flag) {
						edges[i12].triangle_id = tid;
						if (v20_flag) edges[i12].next_id = i20;
						else edges[i12].next_id = m_nEdge;
					}
					else {
						Edge e1, e2;
						e1.vertex_id = iv2;
						e2.vertex_id = iv1;
						e1.id = m_nEdge;
						e2.pair_id = m_nEdge;
						m_nEdge++;
						e2.id = m_nEdge;
						e1.pair_id = m_nEdge;
						m_nEdge++;
						e1.triangle_id = tid;
						e2.triangle_id = -1;
						if (v20_flag) e1.next_id = i20;
						else e1.next_id = m_nEdge;
						e2.next_id = -1;
						e1.multicase = v12_mul;
						e2.multicase = v12_mul;
						edges.push_back(e1);
						edges.push_back(e2);
						if (vertex[iv1].edge_id == -1) vertex[iv1].edge_id = e1.id;
					}

					if (v20_flag) {
						edges[i20].triangle_id = tid;
						edges[i20].next_id = i01;
					}
					else {
						Edge e1, e2;
						e1.vertex_id = iv0;
						e2.vertex_id = iv2;
						e1.id = m_nEdge;
						e2.pair_id = m_nEdge;
						m_nEdge++;
						e2.id = m_nEdge;
						e1.pair_id = m_nEdge;
						m_nEdge++;
						e1.triangle_id = tid;
						e2.triangle_id = -1;
						e1.next_id = i01;
						e2.next_id = -1;
						e1.multicase = v20_mul;
						e2.multicase = v20_mul;
						edges.push_back(e1);
						edges.push_back(e2);
						if (vertex[iv2].edge_id == -1) vertex[iv2].edge_id = e1.id;
					}			

					triangles.push_back(temptt);
				}	
			}
		}
		else if (!res[0].compare("g") && _mat) {
			if (res.size() > 1) {
				group.clear();
				group.assign(res[1]);
			}
		}
		else if (!res[0].compare("usemtl") && _mat) {
			int size_mat = mats.size();
			int length = res[1].length();
			string mat_name;
			mat_name.assign(res[1]);
			for (int i = 1; i < size_mat; i++) {
				if (!mat_name.compare(mats[i].name)) {
					mat = i;
					break;
				}
			}
		}
		else {
			continue;
		}
	}
	fin.close();
	m_nGroup = groups.size();
	return return_normal;
}



/*Not fully supported now, only support Ka, Kd, Ks, Ke, Ns, d, Tr*/
void Mesh::loadMat(char* mtlfname) {
	ifstream fin(mtlfname);
	assert(!fin.fail());
	string line;
	vector<string> res;
	int res_size;
	string sep1 = " ";
	Material newmat;
	int current_mat_count = 1;
	while (getline(fin, line)) {
		res.clear();
		StringSplit(line, sep1, &res);
		if (res.size() == 0)
			continue;
		res_size = res.size();
		while (!res[res_size - 1].compare("\r")) {
			res.pop_back();
			res_size--;
		}
		if (res.size() == 0)
			continue;
		if (!res[0].compare("newmtl")) {
			if (current_mat_count != 1) {
				mats.push_back(newmat);
			}
			current_mat_count++;
			newmat.clear();
			newmat.name.assign(res[1]);
		}
		else if (!res[0].compare("Ka")) {
			for (int i = 1; i < res.size(); i++) {
				newmat.ka_ambient[i - 1] = atof(res[i].c_str());
			}
		}
		else if (!res[0].compare("Kd")) {
			for (int i = 1; i < res.size(); i++) {
				newmat.kd_diffuse[i - 1] = atof(res[i].c_str());
			}
			
		}
		else if (!res[0].compare("Ks")) {
			for (int i = 1; i < res.size(); i++) {
				newmat.ks_specular[i - 1] = atof(res[i].c_str());
			}
		}
		else if (!res[0].compare("Ke")) {
			for (int i = 1; i < res.size(); i++) {
				newmat.ke_emission[i - 1] = atof(res[i].c_str());
			}
		}
		else if (!res[0].compare("Tf")) {
			for (int i = 1; i < res.size(); i++) {
				newmat.tf_transmission[i - 1] = atof(res[i].c_str());
			}
			if (res.size() < 4) {
				newmat.tf_transmission[1] = newmat.tf_transmission[0];
				newmat.tf_transmission[2] = newmat.tf_transmission[0]; 
			}
			
		}
		else if (!res[0].compare("d")) {
			newmat.d_Tr_alpha = atof(res[1].c_str());
			
		}
		else if (!res[0].compare("Tr")) {
			newmat.d_Tr_alpha = atof(res[1].c_str());
		}
		else if (!res[0].compare("Ns")) {
			newmat.Ns_shininess = atof(res[1].c_str()) / 1000.0 * 128.0;
		}
		else if (!res[0].compare("Ni")) {
			newmat.Ni_refelection = atof(res[1].c_str());
		}
		else if (!res[0].compare("illum")) {
			newmat.illum = atoi(res[1].c_str());
		}
		
	}
	if (current_mat_count != 1)
		mats.push_back(newmat);
	fin.close();
}

/*Not supported yet*/
void Mesh::setMatIllum() {
	for (int i = 0; i < mats.size(); i++) {
		switch (mats[i].illum) {
			case 0:
			break;
			case 1:
			break;
			case 2:
			break;
			case 3:
			break;
			case 4:
			break;
			case 5:
			break;
			case 6:
			break;
			case 7:
			break;
			case 8:
			break;
			case 9:
			break;
			case 10:
			break;
		}
	}
}

void Mesh::addToGroup(int tid, string gname) {
	for (int i = 0; i < groups.size(); i++) {
		if (!gname.compare(groups[i].g_name)) {
			groups[i].tids.push_back(tid);
			return;
		}
	}
	FaceGroup fg;
	fg.g_name.assign(gname);
	fg.tids.push_back(tid);
	groups.push_back(fg);
}
