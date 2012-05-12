#ifndef _VECTOR2D_H_
#define _VECTOR2D_H_
#include <cmath>
#include <assert.h>
#include<iostream>
using namespace std;
class Vector2D {
public:
	float x, y;
	Vector2D(float xx = 0.0, float yy = 0.0):x(xx), y(yy){}
	inline operator float *() {return &x;}
	float Length(){return sqrt(x * x + y * y);}
	void display()
	{
		cout<<"x="<<x<<" y="<<y<<endl;
	}
};
float distance(const Vector2D & v1, const Vector2D & v2);
Vector2D normalize (Vector2D& v);

Vector2D operator+ (const Vector2D& v1, const Vector2D& v2);
Vector2D operator- (const Vector2D& v);

Vector2D operator- (const Vector2D& v1, const Vector2D& v2);
Vector2D operator* (const Vector2D& v1, float l);
Vector2D operator* (float l, const Vector2D& v1);
Vector2D operator/ (const Vector2D& v1, float l);
float  operator^ (const Vector2D& v1, const Vector2D& v2);//dot product

#endif
