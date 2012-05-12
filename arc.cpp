/*
 * =====================================================================================
 *
 *       Filename:  arc.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/21/2012 16:59:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include"Vector3D.h"
#define PI 3.14159265

int main ()
{
	Vector3D v1(0,1,0);
	Vector3D v2(0,-1,0);
	float cos=(v1^v2)/(v1.Length()*v2.Length());
	printf ("cosin is %lf \n", cos);
	double param, result;
	param = 0.5;
	result=degree(v2,v1);
	printf ("The arc cosine of %lf is %lf degrees.\n", param, result );
	return 0;
}
