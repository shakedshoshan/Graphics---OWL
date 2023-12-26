#define _CRT_SECURE_NO_WARNINGS

#include "GLUT.h"
#include <math.h>
#include <time.h>
#include <Windows.h>
#include <stdio.h>

const int GSIZE = 200;
const double PI = 3.14;

const int H = 600;
const int W = 600;

double ground[GSIZE][GSIZE] = { 0 };

double eyex = 0, eyey = 20, eyez = 20;
double dx = 0, dz = 0, dy = 0;
double alpha = PI / 2;
double offset = 0;

// ego-motion
double speed = 0;
double sight_angle = PI;
double angular_speed = 0;
double dirx = sin(sight_angle), dirz = cos(sight_angle), diry = 0;
bool slider_is_moving = false;
double oldY;
double rollEyes = 0;
double sight = 0;

float l1amb[4] = { 0.4,0.4,0.4,1 }; // ambient
float l1diff[4] = { 0.8,0.8,0.8,1 }; // diffuse
float l1spec[4] = { 0.7,0.7,0.7,1 }; // specular
float l1pos[4] = { 2,-1,15,0 }; // 0 for directional light
// 1 for positional 




void init()
{
	glClearColor(0.8, 0.8, 1, 0); // set background color
	glEnable(GL_DEPTH_TEST); // test of near/far surface

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);


	// light definitions
	glLightfv(GL_LIGHT0, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l1diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l1pos);

}

void DrawCircle(double r, double g, double b)
{
	
	float amb[4] = { 0.3 * r,0.3 * g,0.3 * b,0 };
	float diff[4] = { 0.7 * r,0.7 * g,0.7 * b,0 };
	float spec[4] = { 0.5 * r,0.5 * g,0.5 * b,0 };


	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);
	
	double alpha, teta = 2 * PI / 360;
	glNormal3d(0, 1, 0);
	glColor3d(r, g, b);
	glBegin(GL_POLYGON);
	for (alpha = 0; alpha < 2 * PI; alpha += teta) {
		//glColor3d(r + sin(alpha)/20, g, b);
		double x = sin(alpha);
		double y = cos(alpha);
		glVertex3d(x, y, 0.0);

	}
	glEnd();
}

void DrawCylinder(int num_pts, double topr, double bottomr, double r, double g, double b)
{
	double alpha, teta = 2 * PI / num_pts;

	float amb[4] = { 0.3 * r,0.3 * g,0.3 * b,0 };
	float diff[4] = { 0.7 * r,0.7 * g,0.7 * b,0 };
	float spec[4] = { 0.5 * r,0.5 * g,0.5 * b,0 };


	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	
	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glNormal3d(sin(alpha), 0, cos(alpha));    glVertex3d(sin(alpha), 1, cos(alpha));
		glBegin(GL_POLYGON);
		
		glColor3d(r,g,b);
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // 1
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // 2
		glColor3d(r,g,b);
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // 3
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // 4
		glEnd();
	}
}

void drawHead(double r, double g, double b) {
	
	glPushMatrix();
	glTranslated(0, 15, 0);
	glScaled(5, 5, 5);
	glRotated(90, 1, 0, 0);
	DrawCircle(r, g, b);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 15, 0);
	glScaled(5, 5, 5);
	glRotated(180, 1, 0, 0);
	DrawCylinder(90, 0, 1, r, g, b);
	glPopMatrix();
}

void DrawSphere(int num_pts, int num_slices, double r, double g, double b)
{
	double beta, delta = PI / num_slices;

	for (beta = -PI / 2; beta <= PI / 2; beta += delta)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + delta) - sin(beta), 1);
		DrawCylinder(num_pts, cos(beta + delta), cos(beta), r, g, b);
		glPopMatrix();
	}
}

void DrawStick(double r, double g, double b) {
	glColor3d(r, g, b);
	glBegin(GL_POLYGON);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glVertex3d(1, 1, 0);
	glVertex3d(1, 0, 0);

	glEnd();
}

void DrawEyes() {
	//eyes
	// 
	//rigth
	glPushMatrix();
	glTranslated(0,0,0);
	glScaled(1, 1, 1);
	DrawSphere(90, 30, 1, 1, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(rollEyes*0.1, 0,1);
	glScaled(0.2, 0.2, 0.2);
	DrawSphere(90, 30, 0,0,0);
	glPopMatrix();

	// left
	glPushMatrix();
	glTranslated(3, 0, 0);
	glScaled(1, 1, 1);
	DrawSphere(90, 30, 1, 1, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(3 + rollEyes*0.1, 0, 1);
	glScaled(0.2, 0.2, 0.2);
	DrawSphere(90, 30,0,0,0 );
	glPopMatrix();
}

void DrawOwl(double r, double g, double b) {
	//wings
	glPushMatrix();
	glTranslated(0, 10, 0);
	glScaled(5, 4.5, 4);
	DrawSphere(60, 30, 1, 1, 1);
	glPopMatrix();
	

	//wings
	glPushMatrix();
	glTranslated(0, 10, 0);
	glScaled(5, 4.5, 4);
	DrawSphere(60, 30, 1, 1, 1);
	glPopMatrix();

	//stick
	glPushMatrix();
	glTranslated(-7.5, 6.6, 1.5);
	glScaled(15, 0.5, 5);
	DrawStick(0.2, 0.2, 0.1);
	glPopMatrix();
	
	//fingers
	//left
	for (int j = 0;j < 3;j++) {
		glPushMatrix();
		glTranslated(-2.8 + j , 6.6, 1.5);
		glScaled(0.3, 1.4, 2);
		DrawSphere(60, 30, 0, 0.4, 1);
		glPopMatrix();
	}

	//right
	for (int j = 0;j < 3;j++) {
		glPushMatrix();
		glTranslated(1 + j , 6.6, 1.5);
		glScaled(0.3, 1.4, 2);
		DrawSphere(60, 30, 0, 0.4, 1);
		glPopMatrix();
	}
	

	// body
	glPushMatrix();
	glTranslated(0, 10, 0);
	glScaled(4.5,5,5);
	DrawSphere(120, 30, r, g, b);
	glPopMatrix();

	// head
	drawHead(r, g, b);

	//nose
	glPushMatrix();
	glTranslated(0, 11, 5);
	glScaled(0.8, 1.7, 1);
	DrawSphere(60, 30, 0, 0, 1);
	glPopMatrix();

	//eye
	glPushMatrix();
	glTranslated(-1.38, 13, 4);
	glScaled(1,1,1);
	DrawEyes();
	glPopMatrix();
	
}


void DrawSlider()
{
	glColor3d(1, 1, 0);
	// background
	glBegin(GL_POLYGON);
	glVertex2d(0, 0);
	glVertex2d(0, 100);
	glVertex2d(100, 100);
	glVertex2d(100, 0);
	glEnd();

	glLineWidth(3);
	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(0, 50);
	glVertex2d(600, 50);
	glEnd();
	glLineWidth(1);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	glPushMatrix();
	glTranslated(rollEyes * 30 / PI, 0, 0); // move in dgrees

	// slider button
	glColor3d(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	glVertex2d(50, 70);
	glVertex2d(52, 30);
	glVertex2d(48, 30);
	glEnd();

	

	glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, W, H); // left+bottom and width+hight
	glMatrixMode(GL_PROJECTION); // glFrustum sets for 3D
	glLoadIdentity();
	// camera model
	glFrustum(-1, 1, -1, 1, 0.7, 300);
	// eyex, eyey, eyez, centerx, centery, centerz, topx, topy, topz
	gluLookAt(eyex, eyey, eyez,
		eyex + dirx, eyey - 0.5, eyez + dirz, 0, 1, 0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslated(0,4,0);
	DrawOwl(0.8,0.7,0.6);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	// controls sub-window
	glViewport(0, 0, 600, 100);
	//glOrtho(-1, 1, -1, 1, -1, 1);
	// Now we switch to Orthogonal projection, so we must change the projections matrix
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here
	glOrtho(0, 100, 0, 100, -1, 1);
	glDisable(GL_DEPTH_TEST); // return to principles of graphics 2D

	DrawSlider();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glutSwapBuffers();
}


void idle()
{
	sight_angle += angular_speed;
	dirx = sin(sight_angle);
	dirz = cos(sight_angle);

	eyex += (speed * dirx);
	eyez += (speed * dirz);
	eyey += dy;
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		angular_speed += 0.0002;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.0002;
		break;
	case GLUT_KEY_UP:
		speed += 0.005;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.005;
		break;
	case GLUT_KEY_PAGE_UP:
		dy += 0.01;
		break;
	case GLUT_KEY_PAGE_DOWN:
		dy -= 0.01;
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	
	double angle_in_degrees = rollEyes * 180 / PI  ;
	printf("%d , %d ,   %1f\n", x, y, angle_in_degrees);
	bool clickInSlider = x > 290 + angle_in_degrees && x < 310 + angle_in_degrees && H - y < 70 && H - y > 30;// field of click
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && clickInSlider) // drag slider
	{
		slider_is_moving = true;
		oldY = W - x;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) slider_is_moving = false;
}

void mouseDrag(int x, int y)
{
	double dx = (W - x) - oldY;

	if (slider_is_moving && W - x < 600 && W - x>0)
	{
		rollEyes += - dx * PI / 180; // transform dy to radian
		oldY = (W - x);

	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OWL");

	glutDisplayFunc(display); // display is the refresh function
	glutIdleFunc(idle);
	glutSpecialFunc(special); // for "home", "insert" arrows keys ...
	glutMotionFunc(mouseDrag);
	glutMouseFunc(mouse);

	init();

	glutMainLoop();
}
