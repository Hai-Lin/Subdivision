#include "Vector2D.h"

Vector2D normalize(Vector2D& v){
	float d=v.Length();
	if(d==0){
		d=0.1f;
	}
	v.x/=d;
	v.y/=d;
	return v;
}

Vector2D operator+(const Vector2D& v1,const Vector2D& v2){
	return Vector2D(v1.x+v2.x,v1.y+v2.y);
}


Vector2D operator-(const Vector2D& v1,const Vector2D& v2){
	return Vector2D(v1.x-v2.x,v1.y-v2.y);
}


Vector2D operator-(const Vector2D& v){
	return Vector2D(-v.x,-v.y);
}

Vector2D operator*(const Vector2D& v,float l){
	return Vector2D(v.x*l,v.y*l);
}
Vector2D operator*(float l, const Vector2D& v){
	return Vector2D(v.x*l,v.y*l);
}

Vector2D operator/(const Vector2D& v,float l){
	return Vector2D(v.x/l,v.y/l);
}


float operator^(const Vector2D& v1,const Vector2D& v2){
	return v1.x*v2.x+v1.y*v2.y;
}

