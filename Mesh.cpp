/*
 * =====================================================================================
 *
 *       Filename:  Mesh.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/20/2012 01:25:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "Mesh.h"

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

void Mesh::setFaceNormal()
{
	Vector3D edge1, edge2,normal;
	for(unsigned int i=0; i<faces.size();++i)
	{
		edge1=vertices[faces[i].ver_id[1]].point-vertices[faces[i].ver_id[0]].point;
		edge2=vertices[faces[i].ver_id[2]].point-vertices[faces[i].ver_id[1]].point;
		normal=edge1*edge2;
		faces[i].faceNormal=normalize(normal);
		faces[i].isNormal=true;
		/*  
			vertices[faces[i].ver_id[0]].point.display();
			vertices[faces[i].ver_id[1]].point.display();
			vertices[faces[i].ver_id[2]].point.display();
			edge1.display();
			edge2.display();
			normal.display();
			*/

	}
}
void Face::displayFace()
{

	cout<<"Face id: "<<id<<endl;
	cout<<"Face vertices id: ";
	for(int i=0; i<ver_id.size();++i)
		cout<<i<<" : "<<ver_id[i]<<" ";
	cout<<endl;

	if(isNormal)
	{
		cout<<"Face's normal is:"<<endl;
		faceNormal.display();
	}
	else
	{
		cout<<"Face's normal is not avaiable now"<<endl;
	}
	cout<<endl;




}
void Vertex::displayVertex()
{
	cout<<"Vertex id: "<<id<<endl;
	cout<<"Vertex edge id: "<<edge_id<<endl;
	cout<<"Vertex point:";
	point.display();
	if(isNormal)
	{
		cout<<"Vertex's normal is:"<<endl;
		normal.display();

	}
	else
		cout<<"Normal is not avaiable"<<endl;
	if(isBoundary)
		cout<<"Vertex is Boundary"<<endl;


}



void Mesh::loadFile( char* fileName)
{
	ifstream input(fileName);
	string line;
	vector<string> temp;
	if(input.is_open())
	{
		cout<<"file opens successfully"<<endl;

		//get mesh type
		getline(input, line);
		if(line=="# loop")
			type=0;
		else
		{
			if(line=="# catmullclark")
				type=1;
			else
				cout<<"type error";
		}




		while(getline(input,line))
		{
			temp.clear();
			StringSplit(line," ",&temp);
			if(temp[0]=="#")
				continue;
			if(temp[0]=="v")
			{
				if(temp.size()==4)
				{
					Vertex tempVertex(atof(temp[1].c_str()),atof(temp[2].c_str()),atof(temp[3].c_str()),vertices.size());
					vertices.push_back(tempVertex);
				}
				else
				{
					cout<<"file format error: v size"<<endl;
				}

			}
			if(temp[0]=="f")
			{
				if(temp.size()>=4)
				{
					vector<int> tempVertices;
					tempVertices.clear();
					Face tempFace(faces.size());
					for(unsigned int i=1; i<temp.size();++i)
					{
						tempVertices.push_back(atoi(temp[i].c_str())-1);
						Edge tempEdge(edges.size(),atoi(temp[i].c_str())-1,faces.size());
						//update next_id and edge_id in faces and vertices
						if(i==temp.size()-1)
						{
							tempEdge.next_id=edges.size()-temp.size()+2;
							vertices[atoi(temp[i].c_str())-1].edge_id=edges.size()-temp.size()+2;
							tempFace.edge_id=edges.size();
						}
						else
						{
							vertices[atoi(temp[i].c_str())-1].edge_id=edges.size()+1;
							tempEdge.next_id=edges.size()+1;
						}
						edges.push_back(tempEdge);
					}
					tempFace.ver_id=tempVertices;
					faces.push_back(tempFace);

				}
				else
				{
					cout<<"file format error: face size"<<endl;
				}

			}

		}
		cout<<"succefully load "<<vertices.size()<<" vertices and "<<faces.size()<<" faces and "<<edges.size()<<" edges"<<endl;
	}
	else
	{
		cout<<"file open error"<<endl;
	}
}
void Edge::displayEdge()
{
	cout<<"edge id: "<<id<<endl;
	cout<<"edge vertex id: "<<vertex_id<<endl;
	cout<<"face id: "<<face_id<<endl;
	cout<<"pair edge id: "<<pair_id<<endl;
	cout<<"next edge id: "<<next_id<<endl;
}


void Mesh::displayMesh()
{
	cout<<"This is vertics:"<<endl;
	for(unsigned int i =0; i<vertices.size(); ++i)
		vertices[i].displayVertex();
	cout<<"This is faces:"<<endl;
	for(unsigned int i=0; i<faces.size();++i)
		faces[i].displayFace();
	cout<<"These are edges:"<<endl;
	for(unsigned int i=0; i<edges.size();++i)
		edges[i].displayEdge();
	cout<<"Mesh type is "<<type<<endl;
}
vector<int> Mesh::edgesOfVertex(int vertex)
{
	Edge edge;
	vector<int> temp;
	edge=edges[vertices[vertex].edge_id];
	if(vertices[vertex].isBoundary)
	{
		for(unsigned int i=0; i<edges.size();++i)
		{
			if(edges[i].vertex_id==vertex)
				temp.push_back(edges[i].next_id);
		}
		return temp;
	}
	do
	{
		edge=edges[edges[edge.pair_id].next_id];
		temp.push_back(edge.id);

	}while(edge.id!=edges[vertices[vertex].edge_id].id);
	return temp;
}

vector<int> Mesh::edgesOfFace(int face)
{
	Edge edge;
	vector<int> temp;
	edge=edges[faces[face].edge_id];
	do
	{
		edge=edges[edge.next_id];
		temp.push_back(edge.id);
	}while(edge.id!=edges[faces[face].edge_id].id);
	return temp;
}

int Mesh::previousEdge(int edge)
{
	Edge next;
	next=edges[edges[edge].next_id];
	do{
		next=edges[next.next_id];

	}while(next.next_id!=edge);
	return next.id;
}
void setPairEdge(Mesh & mesh)
{
	for(unsigned int i=0; i<mesh.edges.size();++i)
	{
		if(mesh.edges[i].pair_id==-1)
		{
			for(unsigned int j=0; j<mesh.edges.size();++j)
			{
				if(mesh.edges[i].vertex_id==mesh.edges[mesh.previousEdge(j)].vertex_id && mesh.edges[j].vertex_id==mesh.edges[mesh.previousEdge(i)].vertex_id)
				{
					/*  

						cout<<"I is "<<i<<endl;
						cout<<"J is "<<j<<endl;
						cout<<"i's vertex edge id "<<mesh.edges[i].vertex_id<<endl;
						cout<<"j's vertex edge id "<<mesh.edges[j].vertex_id<<endl;
						cout<<"i's previous"<<mesh.edges[mesh.previousEdge(i)].vertex_id<<endl;
						cout<<"j's previous"<<mesh.edges[mesh.previousEdge(j)].vertex_id<<endl;
						}
						*/

				mesh.edges[i].pair_id=j;
				mesh.edges[j].pair_id=i;
				break;
			}
		}
		//boundry vertex
		if(mesh.edges[i].pair_id==-1)
		{
			mesh.vertices[mesh.edges[i].vertex_id].isBoundary=true;
			mesh.vertices[mesh.edges[mesh.previousEdge(i)].vertex_id].isBoundary=true;
		}
	}
}
}
void setVertexNormal( Mesh & mesh)
{
	setPairEdge(mesh);
	mesh.setFaceNormal();
	vector<int> neighbor;
	for(unsigned int i=0; i<mesh.vertices.size();++i)
	{
		//get neighbor face
		neighbor.clear();
		neighbor=mesh.edgesOfVertex(i);
		Vector3D temp;
		for(unsigned int j=0;j<neighbor.size();++j)
		{
			int k=(j+1)%neighbor.size();
			if(mesh.vertices[i].isBoundary)
			{
				temp=temp+mesh.faces[mesh.edges[neighbor[j]].face_id].faceNormal;
			}
			else
			{
				Vector3D edge1=mesh.vertices[mesh.edges[neighbor[j]].vertex_id].point-mesh.vertices[mesh.edges[mesh.edges[neighbor[j]].pair_id].vertex_id].point;
				Vector3D edge2=mesh.vertices[mesh.edges[neighbor[k]].vertex_id].point-mesh.vertices[mesh.edges[mesh.edges[neighbor[k]].pair_id].vertex_id].point;
				float weight=degree(edge1,edge2)/360;
				temp=temp+weight*mesh.faces[mesh.edges[neighbor[j]].face_id].faceNormal;
			}
		}
		mesh.vertices[i].normal=normalize(temp);
		mesh.vertices[i].isNormal=true;
	}

}

/*  
int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		cout<<"invalid input!"<<endl;
		return -1;
	}

	Mesh myMesh;
	myMesh.loadFile(argv[1]);
//	setPairEdge(myMesh);


	setVertexNormal(myMesh);

	myMesh.displayMesh();

	return 0;
}

*/
