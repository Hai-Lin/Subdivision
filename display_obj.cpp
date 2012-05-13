// release 02/03/12 - added significant comments
//
//
// Notes: in this assignment, you will be reading in a file describing a series
// of curves & or surfaces, and then "sampling" them so that they may be displayed.
//
// "Sampling" means that you will compute a set of points on each curve (or
// surface), which you will then provide to OpenGL to draw lines between
// (approximating the curve) or triangles between (approximating the surface).
//
// Your code should do these things, in order:
//     1. read in the scene file, and convert to geometric entities (much
//        of this is aready done - see the parser provided in the Resources
//        section of the wiki. This should be done in the main() before
//        setting up the OpenGL callbacks.
//
//     2. sample the geometry - using the deBoor algorithm or the b-spline
//        basis. if it's a curve, you will be sampling the curve at regular
//        spacings in u, and storing the resulting samples in a 1-D array. if
//        it's a surface, you will be sampling at regular intervals in both
//        u AND v, generating a 2-D array of sample points (though you may
//        still be storing that in a 1-d data structure). this is the bulk of
//        your assignment.
//
//     3. outputting it to OpenGL. you should make (minor) modifications to the 
//        function DrawCurvesSurfaces (); See the notes there for what is required.
//
// One note: you only need to generate the samples on your curve/surface ONCE -
// before you start sending them to OpenGL. unless you have changed somthing on
// the surface, or you have changed the sampling rate (more or fewer samples),
// you do NOT need to repeat step 2 above - OpenGL will keep redrawing them
// by calling DrawCuresSurfaces as it needs to.
//
//
// Building:
// on CS machines, compile with:
//
// g++ -I /usr/include/GL -I /usr/include -L /usr/local/lib -lglut -lGL -lGLU 
//                           -lm trackball.cpp display_bspline.cpp -o display_spline
/*
// uncomment this on the mac
#define MAC

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <gl.h>
#include <glu.h>
#include <glut.h>
#endif
*/
#ifdef __APPLE__              //to make in both Mac OS and linux. Thi
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include<GL/glu.h>
#include<GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>
#include "trackball.h"
#include <sys/time.h>
#include"Mesh.h"

vector<HMesh> myHMesh;   //vector of half edge 
vector<Mesh> myMesh;     //vector of indexed triangle data structure
int currentMesh=0;   //current Mesh index number
int mode=1;  //display mode, default set to 1
int type=0;  //type of subdivision 0 is index mesh--Loop subdivision, 1 is half edge--Catmull-Clark Subds


//
// The next 2 functions are just for (rough) timing of how many
// frames-per-second you are displaying.
//
struct timeval frameStartTime, frameEndTime;
#define FRAME_AVG_COUNT 5
int frameCount = 0;  // count 5 frames at a time and average...
float elapsedTime = 100.0; 
void frameStart(void) {
	if (frameCount == 0) 
		gettimeofday(&frameStartTime, NULL);
}



void frameEnd(void *font, GLclampf r, GLclampf g, GLclampf b, 
		GLfloat x, GLfloat y) {
	/* font: font to use, e.g., GLUT_BITMAP_HELVETICA_10
	   r, g, b: text colour
	   x, y: text position in window: range [0,0] (bottom left of window)
	   to [1,1] (top right of window). */
	frameCount++;

	char str[30]; 
	char *ch; 
	GLint matrixMode;
	GLboolean lightingOn;

	if (frameCount == FRAME_AVG_COUNT) {

		gettimeofday(&frameEndTime, NULL);

		elapsedTime= frameEndTime.tv_sec - frameStartTime.tv_sec +
			((frameEndTime.tv_usec - frameStartTime.tv_usec)/1.0E6);
		elapsedTime /= FRAME_AVG_COUNT;
		frameCount = 0;
	}

	sprintf(str, "Frames per second: %2.0f", 1.0/elapsedTime);

	lightingOn= glIsEnabled(GL_LIGHTING);        /* lighting on? */
	if (lightingOn) glDisable(GL_LIGHTING);

	glGetIntegerv(GL_MATRIX_MODE, &matrixMode);  /* matrix mode? */

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */
	glColor3f(r, g, b);
	glRasterPos3f(x, y, 0.0);
	for(ch= str; *ch; ch++) {
		glutBitmapCharacter(font, (int)*ch);
	}
	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(matrixMode);
	if (lightingOn) glEnable(GL_LIGHTING);

}


#define kRotationStep 10.0f
#define kTranslationStep 0.1f
#define BUFFER_LENGTH 64 

bool activeshift = false;


// a default bezier surface:
GLfloat ctrlpoints[4][4][3] =
{
	{
		{-1.5, -1.5, 4.0},
		{-0.5, -1.5, 2.0},
		{0.5, -1.5, -1.0},
		{1.5, -1.5, 2.0}},
	{
		{-1.5, -0.5, 1.0},
		{-0.5, -0.5, 3.0},
		{0.5, -0.5, 0.0},
		{1.5, -0.5, -1.0}},
	{
		{-1.5, 0.5, 4.0},
		{-0.5, 0.5, 0.0},
		{0.5, 0.5, 3.0},
		{1.5, 0.5, 4.0}},
	{
		{-1.5, 1.5, -2.0},
		{-0.5, 1.5, -2.0},
		{0.5, 1.5, 0.0},
		{1.5, 1.5, -1.0}}
};
GLint pointNumU = 4;
GLint pointNumV = 4;

GLfloat baseplane[4][3] =
{
	{-50, -50, -100},
	{-50,  50, -100},
	{ 50, -50, -100},
	{ 50,  50, -100}
};


// Lights & Materials

GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat position[] = {0.0, 0.0, 2.0, 1.0};
GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};


// Parameters

GLfloat camRotX, camRotY, camPosX, camPosY, camPosZ;
GLboolean isSmooth;
GLboolean showLine;

GLint viewport[4];
GLdouble modelview[16];
GLdouble projection[16];

GLuint pickedObj = -1;
char titleString[150];


void initLights(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void SetCamera(void)
{
	glTranslatef(0, 0, camPosZ);
	glRotatef(camRotX, 1, 0, 0);
	glRotatef(camRotY, 0, 1, 0);
}

void SetupRC()
{
	tbInit(GLUT_RIGHT_BUTTON);
	tbAnimate(GL_TRUE);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	initLights();

	// Place Camera
	camRotX = 350.0f;
	camRotY = 680.0f;
	camPosX = 0.0f;
	camPosY = 0.0f;
	camPosZ = -10.5f;

	isSmooth = true;
}

// If we have the user selecting and moving control points, here is where we
// name them so we can find them in the selection buffer:
//
void DrawPoints(void)
{
	// Initialize the names stack
	glInitNames();
	glPushName(0);

	for (int u = 0; u < pointNumU; u++)
	{
		for (int v = 0; v < pointNumV; v++)
		{
			// Set name
			int count = u * pointNumU + v;
			glLoadName(count);

			// Set Color
			GLfloat diffuse[] = {(float)u/pointNumU, (float)v/pointNumV, 0.0, 1.0};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

			// Draw the point			
			glPushMatrix();
			{
				glTranslatef( ctrlpoints[u][v][0], ctrlpoints[u][v][1], ctrlpoints[u][v][2] );

				glutSolidSphere(0.05, 32, 32);
			}
			glPopMatrix();
		}
	}
}


// Draw the curve(s) and/or surface(s): THIS IS WHERE YOUR DRAW CODE GOES, but
// do not do your surface sampling here - it's not necessary to resample the
// surface on every redraw. Just use the same points you sampled before.
//
// This fucntion is called from RenderScene(), which is the display function.
//
// In this function, you need to go through the arrays of sampled points
// (from the curves or surfaces) and draw the appropriate lines or triangles.
//
// For each curve - you will previously have sampled the curve into a set
// of points. You will draw a series of line segments between this samples
// using somthing like:
//
//    glBegin( GL_LINE_STRIP);  // start giving vertices to connect w lines
// 
//    for( i = 0; i < numberOfSamples; ++i)
//        glVertex3f( sample[i].x, sample[i].y, sample[i].z);
//
//    glEnd();  // finished giving vertices to be connceted
//
// The glBegin and glEnd calls tell OpenGL you are drawing a bunch of line
// segments - if your curve samples are close together (say, 4 times as many
// as there are control points) it will look smooth.
//
//
// For each surface - you will have previously sampled the surface (in u and
// v) into a rectangular array of surface points. You will now output those
// points as triangles using something like this:
//
//    glBegin( GL_TRIANGLES );  // start providing triangle vertices
//
//    for (int i = 0; i < numUSamples - 1; ++i) {
//      for (int j = 0; j < numVSamples - 1; ++j {
//    
//      // draw two triangles in the mesh:
//      glVertex3f (surfacePts(i,j).x, surfacePts(i,j).y, surfacePts(i,j).z);
//      glVertex3f (surfacePts(i+1,j).x, surfacePts(i+1,j).y, surfacePts(i+1,j).z);
//      glVertex3f (surfacePts(i+1,j+1).x, surfacePts(i+1,j+1).y, surfacePts(i+1,j+1).z);
//
//      glVertex3f (surfacePts(i,j).x, surfacePts(i,j).y, surfacePts(i,j).z);
//      glVertex3f (surfacePts(i+1,j+1).x, surfacePts(i+1,j+1).y, surfacePts(i+1,j+1).z);
//      glVertex3f (surfacePts(i,j+1).x, surfacePts(i,j+1).y, surfacePts(i,j+1).z);
//
//    glEnd();  // finished providing triangle vertices
//
// Remember you will need to do these for each piece of geometry you have read
// in, so there may be multiple curves and surfaces.
//
void DrawCurvesSurfaces(void)
{


	// Set the color to display the surface with:
	GLfloat grey_diffuse[] = {0.6, 0.6, 0.6, 1.0};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, grey_diffuse);

	//
	// this sets up a demo bezier surface:
	// everthing after this in this function should be replaced 
	// with your code:
	//

	glMap2f(
			GL_MAP2_VERTEX_3,		// target
			0, 1, 3, 4,				// u1, u2, uStride, uOrder
			0, 1, 12, 4,			// v1, v2, vStride, vOrder
			&ctrlpoints[0][0][0]	// points
		   );

	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glMapGrid2f(
			60, 0.0, 1.0,	// un, u1, u2
			60, 0.0, 1.0	// vn, v1, v2
			);


	// If you want to draw things as a mesh grid, vs. an opaque surface, you
	// can do so - though not in quite the same way as here.
	if (showLine) {
		glEvalMesh2(GL_LINE, 0, 60, 0, 60);
	}
	else {
		glEvalMesh2(GL_FILL, 0, 60, 0, 60);
	}
}

void displayMesh()
{
	switch(mode){
		case 1:   //flat normal
			for(unsigned int i=0; i<myMesh.triangles.size();++i)
			{	
				glBegin(GL_TRIANGLES);
				glNormal3f(myMesh.triangles[i].faceNormal.x,myMesh.triangles[i].faceNormal.y,myMesh.triangles[i].faceNormal.z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.z);
				glEnd();
			}
			break;
		case 2:   //average normal
			for(unsigned int i=0; i<myMesh.triangles.size();++i)
			{
				glBegin(GL_TRIANGLES);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[0].x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[0].y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[0].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.z);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[0].x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[0].y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[0].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.z);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[0].x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[0].y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[0].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.z);
				glEnd();


			}
			break;
		case 3:   //average normal
			for(unsigned int i=0; i<myMesh.triangles.size();++i)
			{
				glBegin(GL_TRIANGLES);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[1].x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[1].y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[1].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.z);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[1].x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[1].y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[1].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.z);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[1].x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[1].y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[1].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.z);
				glEnd();


			}
			break;
		case 4:   //average normal
			for(unsigned int i=0; i<myMesh.triangles.size();++i)
			{
				glBegin(GL_TRIANGLES);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[2].x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[2].y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[2].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.z);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[2].x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[2].y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[2].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.z);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[2].x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[2].y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[2].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.z);
				glEnd();
			}
			break;
		case 5:  //shape edge
			for(unsigned int i=0; i<myMesh.triangles.size();++i)
			{
				if(myMesh.triangles[i].isFlat)
				{
				glBegin(GL_TRIANGLES);
				glNormal3f(myMesh.triangles[i].faceNormal.x,myMesh.triangles[i].faceNormal.y,myMesh.triangles[i].faceNormal.z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.z);
				glEnd();

				}
				else
				{
				glBegin(GL_TRIANGLES);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[2].x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[2].y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].normal[2].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[0]].point.z);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[2].x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[2].y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].normal[2].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[1]].point.z);
				glNormal3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[2].x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[2].y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].normal[2].z);
				glVertex3f(myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.x,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.y,myMesh.vertices[myMesh.triangles[i].ver_id[2]].point.z);
				glEnd();
				}
			}
			break;

	}
}

void RenderScene(void)
{

	frameStart();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glPushMatrix();	
	{

		SetCamera();
		tbMatrix();

		glRotatef(85.0, 1.0, 1.0, 1.0);

		// 
		// This function is where your code goes:
		// 
		// DrawCurvesSurfaces();
		displayMesh();
		// uncomment this if you want to allow the user to select and move
		// control points, shaping the surface. at the moment it's buggy -
		// it looks like the plane in which the points can be moved is not
		// calculated correctly
		//
		// DrawPoints();

		// Retrieve current matrice before they popped.
		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );		// Retrieve The Modelview Matrix
		glGetDoublev( GL_PROJECTION_MATRIX, projection );	// Retrieve The Projection Matrix
		glGetIntegerv( GL_VIEWPORT, viewport );				// Retrieves The Viewport Values (X, Y, Width, Height)

	}
	glPopMatrix();

	frameEnd(GLUT_BITMAP_HELVETICA_10, 1.0, 1.0, 1.0, 0.05, 0.95);

	glFlush();
}


// If the user is selecting control points, you need "DrawPoints()" above
// uncommented.
//
// Here is where we we figure out what was clicked on:
//
void ProcessSelection(int xPos, int yPos)
{
	GLfloat fAspect;

	// Space for selection buffer
	static GLuint selectBuff[BUFFER_LENGTH];

	// Hit counter and viewport storage
	GLint hits, viewport[4];

	// Setup selection buffer
	glSelectBuffer(BUFFER_LENGTH, selectBuff);

	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();
	{
		// Change render mode
		glRenderMode(GL_SELECT);

		// Establish new clipping volume to be unit cube around
		// mouse cursor point (xPos, yPos) and extending two pixels
		// in the vertical and horizontal direction
		glLoadIdentity();
		gluPickMatrix(xPos, viewport[3] - yPos + viewport[1], 0.1,0.1, viewport);

		// Apply perspective matrix 
		fAspect = (float)viewport[2] / (float)viewport[3];
		gluPerspective(45.0f, fAspect, 1.0, 425.0);


		// Render only those needed for selection
		glPushMatrix();	
		{
			SetCamera();
			tbMatrixForSelection();
			glRotatef(85.0, 1.0, 1.0, 1.0);
			DrawPoints();
		}
		glPopMatrix();


		// Collect the hits
		hits = glRenderMode(GL_RENDER);

		// If a single hit occurred, display the info.
		if(hits == 1)
		{
			// Save current picked object.
			pickedObj = selectBuff[3];


			glPushMatrix();
			{
				GLfloat winX, winY, winZ;
				GLdouble posX, posY, posZ;

				winX = (float)xPos;
				winY = (float)viewport[3] - (float)yPos;
				glReadPixels( xPos, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
				gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);			
				float k = -(float)1 / ( (float)1 - winZ );
				baseplane[0][2] = k;
				baseplane[1][2] = k;
				baseplane[2][2] = k;
				baseplane[3][2] = k;

				sprintf (titleString, "You clicked on %d win:( %d , %d , %f ) / pos:( %f, %f, %f)", pickedObj, (int)winX, (int)winY, winZ, posX, posY, posZ);
				glutSetWindowTitle(titleString);
			}
			glPopMatrix();

		}
		else
			glutSetWindowTitle("Nothing was clicked on!");

		// Restore the projection matrix
		glMatrixMode(GL_PROJECTION);
	}
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

// Keyboard handling:
//
void SpecialKeys(int key, int x, int y)
{
	if(key == GLUT_KEY_UP) {
		camRotX += kRotationStep;
	}
	if(key == GLUT_KEY_DOWN) {
		camRotX -= kRotationStep;
	}
	if(key == GLUT_KEY_LEFT) {
		camRotY += kRotationStep;
	}
	if(key == GLUT_KEY_RIGHT) {
		camRotY -= kRotationStep;
	}
	if(key=='1')
		mode=1;
	if(key=='2')
		mode=2;
	if(key== '3')
		mode=3;
	if(key=='4')
		mode=4;
	if(key=='5')
		mode=5;
	if(key=='<')
		currentMesh--;
	if(key=='>')
		currentMesh++;
	if(key == 'z') {
		camPosZ += kTranslationStep;
	}
	if(key == 'x') {
		camPosZ -= kTranslationStep;
	}
	if(key== 27) //Escape key
		exit(0);
	if(key == 'l')
	{
		showLine = !showLine;
	}
	if(key == 's')
	{
		isSmooth = !isSmooth;

		if (isSmooth)
		{
			glShadeModel(GL_SMOOTH);
		}
		else 
		{
			glShadeModel(GL_FLAT);
		}
	}

	// Refresh the Window
	glutPostRedisplay();
}

// Mouse handling:
//
void MouseCallback(int button, int state, int x, int y)
{
	tbMouse(button, state, x, y);

	int mymod;
	mymod = glutGetModifiers();

	if ((activeshift == true) && (mymod != GLUT_ACTIVE_SHIFT)) {
		glutSetWindowTitle("NO shift key!");
		activeshift = false;

	}

	if ((activeshift == false) && (mymod == GLUT_ACTIVE_SHIFT)) {
		glutSetWindowTitle("YES shift key!");
		activeshift = true;

	}


	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		ProcessSelection(x, y);

	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		pickedObj = -1;
		baseplane[0][2] = -100;
		baseplane[1][2] = -100;
		baseplane[2][2] = -100;
		baseplane[3][2] = -100;
		glutPostRedisplay();
	}
}

// If the veiwport is resized:
void ChangeSize(int w, int h)
{
	tbReshape(w, h);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the clipping volume
	gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//
// Mouse handling:
//
void motion(int x, int y)
{
	tbMotion(x, y);

	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels( x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	int u = pickedObj/pointNumU;
	int v = pickedObj - u*pointNumU;

	if (pickedObj != -1)
	{
		ctrlpoints[u][v][0] = posX;
		ctrlpoints[u][v][1] = posY;
		ctrlpoints[u][v][2] = posZ;
	}

	glutPostRedisplay();

	sprintf (titleString, "win:( %d , %d , %f ) / pos:( %f, %f, %f)", (int)winX, (int)winY, winZ, posX, posY, posZ);
	glutSetWindowTitle(titleString);

}



//
int main(int argc, char **argv)
{

	// you should probably call a function to read in and parse the scene here

	// you should probably call a function to process the curves/surfaces into 
	// arrays of sample points here

	// Now everything goes to OpenGL. You should have written some code in 
	// DrawCurvesSurfaces which OpenGL will call when it needs to redraw the
	// screen, and you're done!
	//

	myMesh[0].loadFile(argv[1]);
	setVertexNormal(myMesh[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Display and simplify Mesh");
	SetupRC();
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(motion);
	glutMainLoop();

	return 0;
}
