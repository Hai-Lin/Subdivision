#include "Vector3D.h"

Vector3D normalize(Vector3D& v){
	float d=v.Length();
	if(d==0){
		d=0.1f;
	}
	v.x/=d;
	v.y/=d;
	v.z/=d;
	return v;
}

float distanceVec(const Vector3D& v1, const Vector3D& v2)
{
	return sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z));
}
Vector3D operator+(const Vector3D& v1,const Vector3D& v2){
	return Vector3D(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);
}


Vector3D operator-(const Vector3D& v1,const Vector3D& v2){
	return Vector3D(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);
}


Vector3D operator-(const Vector3D& v){
	return Vector3D(-v.x,-v.y,-v.z);
}

Vector3D operator*(const Vector3D& v,float l){
	return Vector3D(v.x*l,v.y*l,v.z*l);
}
Vector3D operator*(float l, const Vector3D & v){
	return Vector3D(v.x*l,v.y*l,v.z*l);
}

Vector3D operator/(const Vector3D& v,float l){
	return Vector3D(v.x/l,v.y/l,v.z/l);
}


float operator^(const Vector3D& v1,const Vector3D& v2){
	return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}


Vector3D operator*(const Vector3D& v1, const Vector3D& v2){
	return Vector3D(v1.y*v2.z-v1.z*v2.y,v1.z*v2.x-v1.x*v2.z,v1.x*v2.y-v1.y*v2.x);
}

Vector3D interpretate(const Vector3D & v1, const Vector3D& v2, float t)
{
	return Vector3D(v1.x+t*(v2.x-v1.x), v1.y+t*(v2.y-v1.y),v1.z+t*(v2.z-v1.z));
}

float degree( Vector3D  v1,  Vector3D  v2 )
{
	float cos=(v1^v2)/(v1.Length()*v2.Length());
	float result = acos (cos) * 180.0 / PI;
	return result;

}
bool isVecZero(const Vector3D& v1)
{
	if(v1.x==0 && v1.y==0 && v1.z==0)
		return true;
	else
		return false;
}
