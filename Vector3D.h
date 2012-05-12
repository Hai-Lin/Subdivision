#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_
#include <cmath>
#include <assert.h>
#include<iostream>
#include <math.h>
#define PI 3.14159265

using namespace std;
class Vector3D {
public:
	float x, y, z;
	Vector3D(float xx = 0.0, float yy = 0.0, float zz = 0.0):x(xx), y(yy), z(zz){}
	inline operator float *() {return &x;}
	float Length(){return sqrt(x * x + y * y + z * z);}
	void display(){
		cout<<"x="<<x<<" y="<<y<<" z="<<z<<"\n";
	}
};
class WVector3D{
	public:
	Vector3D p;
	float w;
	WVector3D()
	{
		Vector3D temp;
		p=temp;
		w=1;
	}
		
	WVector3D (Vector3D pp, float ww=0.0): p(pp), w(ww){}

};

Vector3D normalize (Vector3D& v);
float distanceVec(const Vector3D& v1, const Vector3D & v2);
Vector3D operator+ (const Vector3D& v1, const Vector3D& v2);
Vector3D operator- (const Vector3D& v);
float degree(Vector3D , Vector3D );
Vector3D operator- (const Vector3D& v1, const Vector3D& v2);
Vector3D operator* (const Vector3D& v1, float l);
Vector3D operator* ( float l, const Vector3D & v1);
Vector3D operator/ (const Vector3D& v1, float l);
float  operator^ (const Vector3D& v1, const Vector3D& v2);//dot product
Vector3D operator* (const Vector3D& v1, const Vector3D& v2);//cross product
Vector3D interpretate(const Vector3D & v1, const Vector3D& v2, float t);//intorplate between v1 and v2 by t
bool isVecZero(const Vector3D& v1);

class PointNormal{
	public:
	Vector3D point;
	Vector3D tangentU;
	Vector3D tangentV;
	Vector3D normal;
	PointNormal(){
		Vector3D point;
		Vector3D normal;
		Vector3D tangetU;
		Vector3D tangetV;
	}
	void getNormal()
	{
		if(isVecZero(tangentU) || isVecZero(tangentV))
		{	
		//	cout<<"Cannot get normal now"<<endl;
		return;
		}
		if(isVecZero(normal)!=true)
		{
		//	cout<<"already calculated normal"<<endl;
			return;
		}
		Vector3D n=tangentV*tangentU;
	    normal=normalize(n);
		return ;
		
	}
	void displayPN(){
		cout<<"This is point:"<<endl;
		point.display();
		cout<<"This is tangentU:"<<endl;
		tangentU.display();
		cout<<"This is tangentV:"<<endl;
		tangentV.display();
		cout<<"This is normal:"<<endl;
		normal.display();
	}

};
#endif
