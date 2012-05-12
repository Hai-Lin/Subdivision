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
	for(unsigned int i=0; i<triangles.size();++i)
	{
		edge1=vertices[triangles[i].ver_id[1]].point-vertices[triangles[i].ver_id[0]].point;
		edge2=vertices[triangles[i].ver_id[2]].point-vertices[triangles[i].ver_id[1]].point;
		normal=edge1*edge2;
		triangles[i].faceNormal=normalize(normal);
		triangles[i].isNormal=true;
		/*  
			vertices[triangles[i].ver_id[0]].point.display();
			vertices[triangles[i].ver_id[1]].point.display();
			vertices[triangles[i].ver_id[2]].point.display();
			edge1.display();
			edge2.display();
			normal.display();
			*/

	}
}
void Triangle::displayTriangle()
{

	cout<<"Triangle id: "<<id<<endl;
	cout<<"Triangle vertices id: ";
	for(int i=0; i<=2;++i)
		cout<<i<<" : "<<ver_id[i]<<" ";
	cout<<endl;

	if(isNbr)
	{
		cout<<"Triangle nerbighor's id are: "<<endl;
		for(int i=0; i<=2;++i)
		{
			cout<<i<<" : "<<nbr_id[i]<<" ";
		}
		cout<<endl;
	}
	else
	{
		cout<<"Triangle neibhor is not avaliable now"<<endl;
	}

	if(isNormal)
	{
		cout<<"Triangle's normal is:"<<endl;
		faceNormal.display();
	}
	else
	{
		cout<<"Triangle's normal is not avaiable now"<<endl;
	}
	cout<<endl;




}
void Vertex::displayVertex()
{
	cout<<"Vertex id: "<<id<<endl;
	cout<<"Vertex tri_id: "<<tri_id<<endl;
	cout<<"Vertex point:";
	point.display();
	for(int i=0; i<=3;++i)
	{
		if(isNormal[i])
		{
			cout<<"Vertex's "<<i<<"'s normal is:"<<endl;
			normal[i].display();

		}
		else
			cout<<"Normal "<<i<<" is not avaiable"<<endl;

	}

}



void Mesh::loadFile( char* fileName)
{
	ifstream input(fileName);
	string line;
	vector<string> temp;
	if(input.is_open())
	{
		cout<<"file opens successfully"<<endl;


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
				if(temp.size()==4)
				{
					Triangle tempTriangle(atoi(temp[1].c_str())-1,atoi(temp[2].c_str())-1,atoi(temp[3].c_str())-1,triangles.size());
					for(int i=1; i<4;++i)
						vertices[atoi(temp[i].c_str())-1].tri_id=triangles.size();
					triangles.push_back(tempTriangle);
				}
				else
				{
					cout<<"file format error: face size"<<endl;
				}

			}

		}
		cout<<"succefully load "<<vertices.size()<<" vertices and "<<triangles.size()<<" triangles"<<endl;
	}
	else
	{
		cout<<"file open error"<<endl;
	}
}
float Mesh::area(int i)
{
	Vector3D edge1, edge2, cross;
	edge1=vertices[triangles[i].ver_id[1]].point-vertices[triangles[i].ver_id[0]].point;
	edge2=vertices[triangles[i].ver_id[2]].point-vertices[triangles[i].ver_id[0]].point;
	cross=edge1*edge2;
	return 0.5*cross.Length();
}
void Mesh::displayMesh()
{
	cout<<"This is vertics:"<<endl;
	for(unsigned int i =0; i<vertices.size(); ++i)
		vertices[i].displayVertex();
	cout<<"This is trangles:"<<endl;
	for(unsigned int i=0; i<triangles.size();++i)
		triangles[i].displayTriangle();

}

void Mesh::findNeighbor()
{
	for(unsigned int i=0; i<triangles.size();++i)
	{
		for(unsigned int k=0; k<triangles.size();++k)
		{
			for(unsigned int j=0; j<3;++j)
				for(unsigned int l=0; l<3;++l)
				{
					if(!triangles[i].nbrFound[j])
					{
						if(triangles[i].ver_id[j]==triangles[k].ver_id[l] && triangles[i].ver_id[(j+1)%3]==triangles[k].ver_id[(l+2)%3])
						{
							triangles[i].nbr_id[j]=k;
							triangles[i].nbrFound[j]=true;
							triangles[k].nbr_id[(l+2)%3]=i;
							triangles[k].nbrFound[(l+2)%3]=true;
							
						}
					}
				}
		}
		for(unsigned int z=0; z<3;++z)
		{
			if(!triangles[i].nbrFound[z])
				triangles[i].nbr_id[z]=-1;
		}
		triangles[i].isNbr=true;
		//		cout<<"triangles "<<i<<" found its neighbors"<<endl;
	}
}
vector<int> Mesh::faceOfVertex(int vertex)
{
	int position,tri_index;
	vector<int> result;
	Triangle t=triangles[vertices[vertex].tri_id];
	do{
		for(unsigned int i=0; i<3;++i)
		{	
			if(t.ver_id[i]==vertex)
			{
				position=i;
			}
		}
		tri_index=t.nbr_id[(position+2)%3];
		result.push_back(tri_index);
		if(degree(t.faceNormal, triangles[tri_index].faceNormal)>60)
			vertices[vertex].isSharp=true;
		t=triangles[t.nbr_id[(position+2)%3]];
	} while (t.id!=vertices[vertex].tri_id);
	return result;
}
void setVertexNormal( Mesh & mesh)
{
	mesh.setFaceNormal();
	mesh.findNeighbor();
	vector<int> neighbor;
	float totalArea;
	for(unsigned int i=0; i<mesh.vertices.size();++i)
	{
		//get neighbor face
		neighbor.clear();
		neighbor=mesh.faceOfVertex(i);
		totalArea=0;
		bool isFlat=false;
		Vector3D temp1, temp2, temp3;
		for(unsigned int j=0;j<neighbor.size();++j)
		{
			if(mesh.triangles[neighbor[j]].isFlat)
				isFlat=true;
			//get total Area
			totalArea+=mesh.area(neighbor[j]);
			//set normal 1
			temp1=temp1+mesh.triangles[neighbor[j]].faceNormal;
			//set normal 3
			for(int k=0; k<3;++k)
			{
				if(mesh.triangles[neighbor[j]].ver_id[k]==i)
				{
					Vector3D edge1=mesh.vertices[mesh.triangles[neighbor[j]].ver_id[(k+1)%3]].point-mesh.vertices[i].point;
					Vector3D edge2=mesh.vertices[mesh.triangles[neighbor[j]].ver_id[(k+2)%3]].point-mesh.vertices[i].point;
					float weight=degree(edge1,edge2)/180;
					temp3=temp3+weight*mesh.triangles[neighbor[j]].faceNormal;
				}
			}
		}
		for(unsigned int k=0;k<neighbor.size();++k)
		{
			if(mesh.vertices[i].isSharp)
			mesh.triangles[neighbor[k]].isFlat=true;
			//set normal2
			float weight=mesh.area(neighbor[k])/totalArea;
			temp2=temp2+weight*mesh.triangles[neighbor[k]].faceNormal;
		}
		mesh.vertices[i].normal[0]=normalize(temp1);
		mesh.vertices[i].normal[1]=normalize(temp2);
		mesh.vertices[i].normal[2]=normalize(temp3);
		mesh.vertices[i].isNormal[0]=true;
		mesh.vertices[i].isNormal[1]=true;
		mesh.vertices[i].isNormal[2]=true;

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
	setVertexNormal(myMesh);
	myMesh.displayMesh();

	return 0;
}


*/
