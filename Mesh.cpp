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
		edge2=vertices[faces[i].ver_id[2]].point-vertices[faces[i].ver_id[0]].point;
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
		cout<<" "<<ver_id[i]<<" ";
	cout<<endl;
	cout<<"Edge id: "<<edge_id<<endl;

	if(isNormal)
	{
		cout<<"Face's normal is:";
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
	cout<<"edge id: "<<edge_id<<endl;
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

vector<Vertex> getLoopVertices(Mesh mesh)
{
	vector<int> neighbor;
	vector<Vertex> newVertices;
	Vertex newVertex;
	float beta;
	for(unsigned int i=0; i<mesh.vertices.size();++i)
	{
		neighbor.clear();
		Vector3D newPoint;
		neighbor=mesh.edgesOfVertex(i);
		if(mesh.vertices[i].isBoundary)
		{
			newVertex.isBoundary=true;
			if(neighbor.size()==1)
			{
				beta=0.125;
				newPoint=0.75*mesh.vertices[i].point;
				Vector3D neighborPoint=mesh.vertices[mesh.edges[neighbor[0]].vertex_id].point;
				newPoint.display();
				newPoint=newPoint+beta*neighborPoint;
				newPoint.display();
				Edge edge=mesh.edges[mesh.edges[neighbor[0]].next_id];
				newPoint=newPoint+beta*mesh.vertices[edge.vertex_id].point;


			}
			else
			{
				int n=neighbor.size()+1;
				if(n==3)
					beta=0.1875;
				else
					beta=3.0/(8.0*(float)n);
				newPoint=(1-n*beta)*mesh.vertices[i].point;
				for(unsigned int j=0; j<n-1;++j)
				{
					Edge edge=mesh.edges[neighbor[j]];
					Edge edge1=mesh.edges[edge.next_id];
					Edge edge2=mesh.edges[edge1.next_id];
					if(edge2.pair_id==-1)
					{
						newPoint=newPoint+beta*mesh.vertices[edge1.vertex_id].point;
					}
					newPoint=newPoint+beta*mesh.vertices[edge.vertex_id].point;

				}

			}
		}
		else
		{
			newVertex.isBoundary=false;
			int n=neighbor.size();
			if(n==3)
				beta=0.1875;
			else
				beta=3.0/(8.0*(float)n);
			newPoint=(1-n*beta)*mesh.vertices[i].point;
			for(unsigned int j=0; j<n;++j)
				newPoint=newPoint+beta*mesh.vertices[mesh.edges[neighbor[j]].vertex_id].point;
		}
		newVertex.point=newPoint;
		newVertex.id=i;
		newVertex.isNormal=false;
		newVertices.push_back(newVertex);

	}
	return newVertices;
}
Vertex getLoopEdgeVertex(Mesh mesh, int edge_id)
{
	Vertex newVertex;
	Vector3D point;
	Edge edge=mesh.edges[edge_id];
	newVertex.isNormal=false;
	if(edge.pair_id==-1)
	{
		newVertex.isBoundary=true;
		point=point+0.5*mesh.vertices[edge.vertex_id].point;
		edge=mesh.edges[edge.next_id];
		edge=mesh.edges[edge.next_id];
		point=point+0.5*mesh.vertices[edge.vertex_id].point;

	}
	else
	{
		newVertex.isBoundary=false;
		Edge pair=mesh.edges[edge.pair_id];
		Edge edge_next=mesh.edges[edge.next_id];
		Edge pair_next=mesh.edges[pair.next_id];
		point=point+0.375*mesh.vertices[edge.vertex_id].point;
		point=point+0.375*mesh.vertices[pair.vertex_id].point;
		point=point+0.125*mesh.vertices[edge_next.vertex_id].point;
		point=point+0.125*mesh.vertices[pair_next.vertex_id].point;
	}
	newVertex.point=point;
	return newVertex;
}
Mesh getLoopSub(Mesh mesh)   //get the next level of Loop subdivision
{
	Mesh newMesh;
	vector<Vertex> newVertices=getLoopVertices(mesh);
	vector<Face> newFaces;
	vector<Edge> newEdges;
	int pushTime;
	for(unsigned int i=0; i<mesh.faces.size();++i)
	{
		pushTime=0;
		cout<<"i's face"<<i<<endl;
		//get three edge vertices
		Edge currentEdge1=mesh.edges[mesh.faces[i].edge_id];
		Vertex newVertex1;
		Vertex newVertex2;
		Vertex newVertex3;
		if(currentEdge1.nextEdgeVertex==-1)
		{
			newVertex1=getLoopEdgeVertex(mesh, currentEdge1.id);
			newVertex1.id=newVertices.size();
			pushTime++;
			if(currentEdge1.pair_id!=-1)
				mesh.edges[currentEdge1.pair_id].nextEdgeVertex=newVertex1.id;
		}
		else
		{
			newVertex1=newVertices[currentEdge1.nextEdgeVertex];
		}
		Edge currentEdge2=mesh.edges[currentEdge1.next_id];
	  	if(currentEdge2.nextEdgeVertex==-1)
		{
			newVertex2=getLoopEdgeVertex(mesh, currentEdge2.id);
			newVertex2.id=newVertices.size()+pushTime;
			pushTime++;
			if(currentEdge2.pair_id!=-1)
				mesh.edges[currentEdge2.pair_id].nextEdgeVertex=newVertex2.id;
		}
		else
		{
			newVertex2=newVertices[currentEdge2.nextEdgeVertex]; 
		}
		Edge currentEdge3=mesh.edges[currentEdge2.next_id];
	  	if(currentEdge3.nextEdgeVertex==-1)
		{
			newVertex3=getLoopEdgeVertex(mesh, currentEdge3.id);
			newVertex3.id=newVertices.size()+pushTime;
			if(currentEdge3.pair_id!=-1)
				mesh.edges[currentEdge3.pair_id].nextEdgeVertex=newVertex3.id;

		}
		else
		{
			newVertex3=newVertices[currentEdge3.nextEdgeVertex]; 
		}
		Vertex newConerVertex1=newVertices[currentEdge1.vertex_id];
		Vertex newConerVertex2=newVertices[currentEdge2.vertex_id];
		Vertex newConerVertex3=newVertices[currentEdge3.vertex_id];

		//construct middle triangles
		Face newFace;
		newFace.id=newFaces.size();
		newFace.ver_id.push_back(newVertex1.id);
		newFace.ver_id.push_back(newVertex2.id);
		newFace.ver_id.push_back(newVertex3.id);
		//consturct new edge
		Edge newEdge1, newEdge2, newEdge3;
		newEdge1.vertex_id=newVertex1.id;
		newEdge2.vertex_id=newVertex2.id;
		newEdge3.vertex_id=newVertex3.id;
		newEdge1.face_id=newFace.id;
		newEdge2.face_id=newFace.id;
		newEdge3.face_id=newFace.id;
		newEdge1.id=newEdges.size();
		newEdge2.id=newEdges.size()+1;
		newEdge3.id=newEdges.size()+2;
		newEdge1.next_id=newEdge2.id;
		newEdge2.next_id=newEdge3.id;
		newEdge3.next_id=newEdge1.id;
		newFace.edge_id=newEdge1.id;
		if(currentEdge1.nextEdgeVertex==-1)
		{
			newVertex1.edge_id=newEdge2.id;
			newVertices.push_back(newVertex1);
		}
		if(currentEdge2.nextEdgeVertex==-1)
		{
			newVertex2.edge_id=newEdge3.id;
			newVertices.push_back(newVertex2);

		}
		if(currentEdge3.nextEdgeVertex==-1)
		{
			newVertex3.edge_id=newEdge1.id;
			newVertices.push_back(newVertex3);

     	}
		//construct side triangles
		//1 face
		Face conerFace1;
		conerFace1.id=newFaces.size()+1;
		conerFace1.ver_id.push_back(newVertex1.id);
		conerFace1.ver_id.push_back(newConerVertex1.id);
		conerFace1.ver_id.push_back(newVertex2.id);
		//1 edges
		Edge newConer1Edge1, newConer1Edge2, newConer1Edge3;
		newConer1Edge1.vertex_id=newVertex1.id;
		newConer1Edge2.vertex_id=newConerVertex1.id;
		newConer1Edge3.vertex_id=newVertex2.id;
		newConer1Edge1.face_id=conerFace1.id;
		newConer1Edge2.face_id=conerFace1.id;
		newConer1Edge3.face_id=conerFace1.id;
		newConer1Edge1.id=newEdges.size()+3;
		newConer1Edge2.id=newEdges.size()+4;
		newConer1Edge3.id=newEdges.size()+5;
		newConer1Edge1.next_id=newConer1Edge2.id;
		newConer1Edge2.next_id=newConer1Edge3.id;
		newConer1Edge3.next_id=newConer1Edge1.id;
		conerFace1.edge_id=newConer1Edge1.id;
		newVertices[currentEdge1.vertex_id].edge_id=newConer1Edge2.id;

		//2 face
		Face conerFace2;
		conerFace2.id=newFaces.size()+2;
		conerFace2.ver_id.push_back(newVertex2.id);
		conerFace2.ver_id.push_back(newConerVertex2.id);
		conerFace2.ver_id.push_back(newVertex3.id);
		//2 edge
		Edge newConer2Edge1, newConer2Edge2, newConer2Edge3;
		newConer2Edge1.vertex_id=newVertex2.id;
		newConer2Edge2.vertex_id=newConerVertex2.id;
		newConer2Edge3.vertex_id=newVertex3.id;
		newConer2Edge1.face_id=conerFace2.id;
		newConer2Edge2.face_id=conerFace2.id;
		newConer2Edge3.face_id=conerFace2.id;
		newConer2Edge1.id=newEdges.size()+6;
		newConer2Edge2.id=newEdges.size()+7;
		newConer2Edge3.id=newEdges.size()+8;
		newConer2Edge1.next_id=newConer2Edge2.id;
		newConer2Edge2.next_id=newConer2Edge3.id;
		newConer2Edge3.next_id=newConer2Edge1.id;
		conerFace2.edge_id=newConer2Edge1.id;
		newVertices[currentEdge2.vertex_id].edge_id=newConer2Edge2.id;
		//3 face
		Face conerFace3;
		conerFace3.id=newFaces.size()+3;
		conerFace3.ver_id.push_back(newVertex3.id);
		conerFace3.ver_id.push_back(newConerVertex3.id);
		conerFace3.ver_id.push_back(newVertex1.id);
		//3 edge
		Edge newConer3Edge1, newConer3Edge2,newConer3Edge3;
		newConer3Edge1.vertex_id=newVertex3.id;
		newConer3Edge2.vertex_id=newConerVertex3.id;
		newConer3Edge3.vertex_id=newVertex1.id;
		newConer3Edge1.face_id=conerFace3.id;
		newConer3Edge2.face_id=conerFace3.id;
		newConer3Edge3.face_id=conerFace3.id;
		newConer3Edge1.id=newEdges.size()+9;
		newConer3Edge2.id=newEdges.size()+10;
		newConer3Edge3.id=newEdges.size()+11;
		newConer3Edge1.next_id=newConer3Edge2.id;
		newConer3Edge2.next_id=newConer3Edge3.id;
		newConer3Edge3.next_id=newConer3Edge1.id;
		conerFace3.edge_id=newConer3Edge1.id;
		newVertices[currentEdge3.vertex_id].edge_id=newConer3Edge2.id;
		//associate pair
		newEdge1.pair_id=newConer3Edge1.id;
		newConer3Edge1.pair_id=newEdge1.id;
		newEdge2.pair_id=newConer1Edge1.id;
		newConer1Edge1.pair_id=newEdge2.id;
		newEdge3.pair_id=newConer2Edge1.id;
		newConer2Edge1.pair_id=newEdge3.id;
		//push all of them
		//faces
		newFaces.push_back(newFace);
		newFaces.push_back(conerFace1);
		newFaces.push_back(conerFace2);
		newFaces.push_back(conerFace3);
		//edges
		newEdges.push_back(newEdge1);
		newEdges.push_back(newEdge2);
		newEdges.push_back(newEdge3);
		newEdges.push_back(newConer1Edge1);
		newEdges.push_back(newConer1Edge2);
		newEdges.push_back(newConer1Edge3);
		newEdges.push_back(newConer2Edge1);
		newEdges.push_back(newConer2Edge2);
		newEdges.push_back(newConer2Edge3);
		newEdges.push_back(newConer3Edge1);
		newEdges.push_back(newConer3Edge2);
		newEdges.push_back(newConer3Edge3);
	}
	newMesh.vertices=newVertices;
	newMesh.faces=newFaces;
	newMesh.edges=newEdges;
	newMesh.type=0;
	return newMesh;
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

Mesh newMesh=getLoopSub(myMesh);
newMesh.displayMesh();
return 0;
}
*/
