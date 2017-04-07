#define GLUT_DISABLE_ATEXIT_HACK
#include <stdlib.h>
#include "GL/glut.h"
#include <amp_math.h>
#include <stdio.h>
#define PI 3.141593
#define Width 900.00
#define Height 900.00
int move = 0;	//move or not
				//int studentTurn = 0;	//which direction the student face
int zoom = -1;	//how close is the camara
int height = 3;	//height of the camara
int px = 600;	//the base position of camara
int py = 300;	//the base position of camara
int pz = 600;	//the base position of camara
int light = 0;	//light mode
				//int spotlight = 0;	//enable or disable the soptlight
double camaraTurn = 0;	//angle of camara
int lastTime = 0;	//time stamp of last frame
long currentTime;	//time stamp of this frame
int chara = 0;	//the active character.
int studentTurn[2] = { 0,0 };	//direction of students
bool tour = false;	//set auto-tour

int toured = 0;	//number of place toured
int ltime = -150;	//time of last tour


struct box {
	float x1;
	float y1;
	float x2;
	float y2;
};	//place where cant run intod

struct point {
	float x;
	float y;
	float z;
};	//can also be used to set color

point position[2] = { { -200, 0, -150 },{ -100, 0, -100 } };	//position of students
point placeToTour[] = { { 450, -66, -130 },{ -63, 30, -163 },{ -68, -30, -40 } };	//place to tour
																					//point currentPosition = position[0];	//position of the active student

box col[] = { { -358.0, -316, -317, -310 },{ -190, -316, -149, -214 },{ -358, -96, -271, -92 },{ -362, -310, -145, -220 }
,{ -358, -220, -275, -137 },{ -362, -137, -267, -96 } ,{ -99, -298, -20, -283 },{ -118, -283, -2, -168 }
,{ -123, -265, 3, -186 },{ -99, -168, -20, -165 },{ 30, -314, 71, -213 },{ 197, -313, 236, -308 }
,{ 25, -309, 243, -218 },{ 152, -218, 237, -90 },{ 146, -133, 243, -94 },{ -360, -11, -317, 101 }
,{ -317, -5.5, -188, 95 },{ -188, -11, -147, 101 },{ -306, 95, -202, 146 },{ 25, -11, 66, 101 }
,{ 66, -8, 200, 92 },{ 195, -11, 236, 101 },{ 80, 92, 187, 119 } };

float power(float input) {
	return input*input;
}

bool colDetect(point p) {
	//printf("%f %f \n", (-2.747*p.x + p.z), (2.747*p.x + p.z));
	//printf("%f %f \n", p.x,p.z);
	if (p.x > 500 || p.x < -400 || p.z>210 || p.z < -400)	//in the map
		return false;
	else if (p.x < 215 && p.x > 185 && p.z < 205 && p.z > 175)	//peds 1
		return false;
	else if (p.x > 435 && p.x < 465 && p.z < -115 && p.z > -145)	//peds 2
		return false;
	else if (chara == 0 && p.x > position[1].x - 15 && p.x < position[1].x + 15 && p.z > position[1].z - 15 && p.z < position[1].z + 15)	//another student that can be controlled
		return false;
	else if (chara == 1 && p.x > position[0].x - 15 && p.x < position[0].x + 15 && p.z > position[0].z - 15 && p.z < position[0].z + 15)	//another student that can be controlled
		return false;
	else if (power(p.x + 63) + power(p.z + 163) < 484)	//the white cylinder of the middle building
		return false;

	for (int i = 0; i < 23; i++) {	//retangular biuldings
									//printf("%f %f %f %f %f %f\n",p.x,p.z, col[i].x1, col[i].x2, col[i].y1, col[i].y2);
		if (p.x > col[i].x1 && p.x < col[i].x2 && p.z > col[i].y1 && p.z < col[i].y2)
			return false;
	}

	if ((p.x > -361 && p.z > 30) || (p.x > -120 && p.x <-5 && p.z > -5 && p.z < 30))	//lower place
		position[chara].y = -66;
	//printf("%f %f \n", p.x, p.z);
	//printf("%c \n", 'a');
	else if ((power(p.x + 63) + power(p.z + 5) < 3364 && !((-2.747*p.x + p.z) < 168.1 && (2.747*p.x + p.z) < -178.061)) || (power(p.x + 63) + power(p.z + 5) < 1450))	//before on stair
		position[chara].y = -66;
	else if (p.y == -66 && p.x < 400 && !((-2.747*p.x + p.z) < 168.1 && (2.747*p.x + p.z) < -178.061))	//round wall
		return false;
	if (((-2.747*p.x + p.z) < 168.1 && (2.747*p.x + p.z) < -178.06) && (power(p.x + 63) + power(p.z + 5) > 1444) && (power(p.x + 63) + power(p.z + 5) < 3600) && p.z < -5)	//on stair
		position[chara].y = 0.034375*(power(p.x + 63) + power(p.z + 5) - 1444) - 66;
	else if (p.x > 243 && p.z < 30 && p.x < 400)	//on the slope at left
		position[chara].y = (243 - p.x) / 2.38;

	if (position[chara].y > 0)	//set height to 0~-66
		position[chara].y = 0;
	else if (position[chara].y < -66)
		position[chara].y = -66;

	return true;
}

void drawPolygon(point p1, point p2, point p3, point p4, int edges = 4, point p5 = { 0,0,0 }) {
	glBegin(GL_POLYGON);

	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p4.x, p4.y, p4.z);
	if (edges == 5)
		glVertex3f(p5.x, p5.y, p5.z);
	glEnd();
}

void drawCube(point p1, point p2, point p3, point p4, point p5, point p6, point p7, point p8) {
	drawPolygon(p1, p2, p3, p4);
	drawPolygon(p3, p7, p8, p4);
	drawPolygon(p5, p6, p7, p8);
	drawPolygon(p2, p6, p5, p1);
	drawPolygon(p2, p6, p7, p3);
	drawPolygon(p1, p5, p8, p4);
}

//draw cube by setting base and height
void simpleCube(float x1, float y1, float x2, float y2, float h = 0, float btm = 0, point top = { 0,0,0 }, point side = { 0,0,0 }) {
	point p1, p2, p3, p4, p5, p6, p7, p8, color;
	p1 = { x1,btm,y2 };
	p2 = { x1,h,y2 };
	p3 = { x2,h,y2 };
	p4 = { x2,btm,y2 };
	p5 = { x1,btm,y1 };
	p6 = { x1,h,y1 };
	p7 = { x2,h,y1 };
	p8 = { x2,btm,y1 };
	color = { 0,0,0 };
	if (top.x != color.x || top.y != color.y || top.z != color.z) {
		glColor3f(side.x, side.y, side.z);
		glNormal3f(0, 0, 1);
		drawPolygon(p1, p2, p3, p4);
		glNormal3f(1, 0, 0);
		drawPolygon(p3, p7, p8, p4);
		glNormal3f(0, 0, -1);
		drawPolygon(p5, p6, p7, p8);
		glNormal3f(-1, 0, 0);
		drawPolygon(p2, p6, p5, p1);
		glNormal3f(0, -1, 0);
		drawPolygon(p1, p5, p8, p4);

		glColor3f(top.x, top.y, top.z);
		glNormal3f(0, 1, 0);
		drawPolygon(p2, p6, p7, p3);
	}
	else {
		glNormal3f(0, 0, 1);
		drawPolygon(p1, p2, p3, p4);
		glNormal3f(1, 0, 0);
		drawPolygon(p3, p7, p8, p4);
		glNormal3f(0, 0, -1);
		drawPolygon(p5, p6, p7, p8);
		glNormal3f(-1, 0, 0);
		drawPolygon(p2, p6, p5, p1);
		glNormal3f(0, -1, 0);
		drawPolygon(p1, p5, p8, p4);
		glNormal3f(0, 1, 0);
		drawPolygon(p2, p6, p7, p3);
	}
}

//draw a circle, center is (x,y,z) and on plane XY if d = 0, XZ if d = 1
void drawCircle(int d, double x, double y, double z, double radius, bool fill, double startangle = 0, double endangle = 360)
{
	if (d == 0) {
		if (fill)
		{
			glBegin(GL_TRIANGLE_FAN);
			glVertex3d(x, y, z);
		}
		else
		{
			glBegin(GL_LINE_STRIP);
		}
		startangle = startangle / 180 * PI;
		endangle = endangle / 180 * PI;
		for (double i = startangle; i <= endangle; i += PI / 180)
		{
			double vx = x + radius * cos(i);
			double vy = y + radius * sin(i);
			glVertex3d(vx, vy, z);
		}
		glEnd();
	}
	else if (d == 1) {
		if (fill)
		{
			glBegin(GL_TRIANGLE_FAN);
			glVertex3d(x, y, z);
		}
		else
		{
			glBegin(GL_LINE_STRIP);
		}
		startangle = startangle / 180 * PI;
		endangle = endangle / 180 * PI;
		for (double i = startangle; i <= endangle; i += PI / 180)
		{
			double vx = x + radius * cos(i);
			double vy = z + radius * sin(i);
			glVertex3d(vx, y, vy);
		}
		glEnd();
	}
}

//draw a cylinder
void glutSolidCylinder(int r, int h) {
	GLUquadricObj *qobj = gluNewQuadric();
	gluCylinder(qobj, r, r, h, 30, 30);
	drawCircle(0, 0, 0, 0, r, true);
	drawCircle(0, 0, 0, h, r, true);
}

//draw a half shpere or cylinder
void drawHalfShape(double r, float x, float y, float z, char shape, int h = 0) {

	GLdouble eqn[4] = { x, y, z, 0.0 };
	glClipPlane(GL_CLIP_PLANE0, eqn);
	glEnable(GL_CLIP_PLANE0);
	if (shape == 's')
		glutSolidSphere(r, 20, 20);
	if (shape == 'c')
		glutSolidCylinder(r, h);
	glDisable(GL_CLIP_PLANE0);
}

//draw the ground
void drawGround() {

	double la = 70.0 / 180 * PI;
	double ra = 110.0 / 180 * PI;

	double lux = -63 - 58 * cos(la);
	double luy = -5 - 58 * sin(la);
	double llx = -63 - 38 * cos(la);
	double lly = -5 - 38 * sin(la);

	double rux = -63 - 58 * cos(ra);
	double ruy = -5 - 58 * sin(ra);
	double rlx = -63 - 38 * cos(ra);
	double rly = -5 - 38 * sin(ra);

	glColor3f(0.95, 0.91, 0.87);
	glNormal3f(0, 1, 0);
	//bottom plane
	glBegin(GL_POLYGON);
	glVertex3f(-360, -66, -400);
	glVertex3f(400, -66, -400);
	glVertex3f(400, -66, 210);
	glVertex3f(-360, -66, 210);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(500, -66, -400);
	glVertex3f(400, -66, -400);
	glVertex3f(400, -66, 210);
	glVertex3f(500, -66, 210);
	glEnd();
	//upper ground
	glColor3f(0.98, 0.94, 0.90);
	glBegin(GL_POLYGON);
	glVertex3f(-400, 0, -400);
	glVertex3f(-400, 0, 210);
	glVertex3f(-361, 0, 210);
	glVertex3f(-361, 0, -400);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-361, 0, -400);
	glVertex3f(-361, 0, 30);
	glVertex3f(-120, 0, 30);
	glVertex3f(-120, 0, -400);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-120, 0, -400);
	glVertex3f(-120, 0, -63);
	glVertex3f(-5, 0, -63);
	glVertex3f(-5, 0, -400);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-5, 0, -400);
	glVertex3f(-5, 0, 30);
	glVertex3f(243, 0, 30);
	glVertex3f(243, 0, -400);
	glEnd();


	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(-120, 0, -63);
	for (double i = 0; i <= PI / 2; i += PI / 180)
	{
		double vx = -63 - 58 * cos(i);
		double vy = -5 - 58 * sin(i);
		glVertex3d(vx, 0, vy);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(-5, 0, -63);
	for (double i = PI / 2; i <= PI; i += PI / 180)
	{
		double vx = -63 - 58 * cos(i);
		double vy = -5 - 58 * sin(i);
		glVertex3d(vx, 0, vy);
	}
	glEnd();


	glColor3f(0.73, 0.55, 0.50);
	//wall left
	for (double i = 0; i < 80; i += 1)
	{
		glBegin(GL_POLYGON);
		glNormal3f(1, 0, tan(i*PI / 180));
		double vx0 = -63 - 58 * cos(i*PI / 180);
		double vy0 = -5 - 58 * sin(i*PI / 180);
		double vx1 = -63 - 58 * cos((i + 1)*PI / 180);
		double vy1 = -5 - 58 * sin((i + 1)*PI / 180);
		glVertex3d(vx0, 0, vy0);
		glVertex3d(vx1, 0, vy1);
		glVertex3d(vx1, -66, vy1);
		glVertex3d(vx0, -66, vy0);
		glEnd();
	}

	glNormal3f(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(-121, 0, 30);
	glVertex3f(-121, -66, 30);
	glVertex3f(-121, -66, -5);
	glVertex3f(-121, 0, -5);
	glEnd();

	//wall right
	for (double i = 10; i < 90; i += 1)
	{
		glBegin(GL_POLYGON);
		glNormal3f(-1, 0, -tan(PI / 2 + i*PI / 180));
		double vx0 = -63 - 58 * cos(PI / 2 + i*PI / 180);
		double vy0 = -5 - 58 * sin(PI / 2 + i*PI / 180);
		double vx1 = -63 - 58 * cos(PI / 2 + (i + 1)*PI / 180);
		double vy1 = -5 - 58 * sin(PI / 2 + (i + 1)*PI / 180);
		glVertex3d(vx0, 0, vy0);
		glVertex3d(vx1, 0, vy1);
		glVertex3d(vx1, -66, vy1);
		glVertex3d(vx0, -66, vy0);
		glEnd();
	}

	glNormal3f(-1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(-5, 0, 30);
	glVertex3f(-5, -66, 30);
	glVertex3f(-5, -66, -5);
	glVertex3f(-5, 0, -5);
	glEnd();

	// left
	glNormal3f(0, 0, 1);
	glBegin(GL_POLYGON);
	glVertex3f(-150, 0, 30);
	glVertex3f(-121, 0, 30);
	glVertex3f(-121, -66, 30);
	glVertex3f(-150, -66, 30);
	glEnd();

	// right
	//glNormal3f(0, 0, 1);
	glBegin(GL_POLYGON);
	glVertex3f(-5, 0, 30);
	glVertex3f(30, 0, 30);
	glVertex3f(30, -66, 30);
	glVertex3f(-5, -66, 30);
	glEnd();

	//right slope
	glColor3f(0.98, 0.94, 0.90);
	glNormal3f(1, 2.38, 0);
	glBegin(GL_POLYGON);
	glVertex3f(243, 0, -400);
	glVertex3f(243, 0, 30);
	glVertex3f(400, -66, 30);
	glVertex3f(400, -66, -400);
	glEnd();

	// side
	glColor3f(0.73, 0.55, 0.50);
	glNormal3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
	glVertex3f(243, 0, 30);
	glVertex3f(243, -66, 30);
	glVertex3f(400, -66, 30);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(243, 0, 30);
	glVertex3f(233, 0, 30);
	glVertex3f(233, -66, 30);
	glVertex3f(243, -66, 30);
	glEnd();

	//far left wall
	glNormal3f(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(-360, 0, 100);
	glVertex3f(-360, 0, 210);
	glVertex3f(-360, -66, 210);
	glVertex3f(-360, -66, 100);
	glEnd();

	//stair wall
	glNormal3f(luy - lly, 0, llx - lux);
	glBegin(GL_TRIANGLES);
	glVertex3f(lux, 0, luy);
	glVertex3f(lux, -66, luy);
	glVertex3f(llx, -66, lly);
	glEnd();
	glNormal3f(rly - ruy, 0, rux - rlx);
	glBegin(GL_TRIANGLES);
	glVertex3f(rux, 0, ruy);
	glVertex3f(rux, -66, ruy);
	glVertex3f(rlx, -66, rly);
	glEnd();

	//stairs
	glColor3f(0.98, 0.94, 0.90);
	glBegin(GL_TRIANGLE_FAN);
	for (double i = 70.0 / 180 * PI; i <= 110.0 / 180 * PI; i += PI / 180)
	{
		double vx1 = -63 - 58 * cos(i);
		double vy1 = -5 - 58 * sin(i);
		glVertex3d(vx1, 0, vy1);
	}
	for (double j = 110.0 / 180 * PI; j >= 70.0 / 180 * PI; j -= PI / 180)
	{
		double vx = -63 - 38 * cos(j);
		double vy = -5 - 38 * sin(j);
		glVertex3d(vx, -66, vy);
	}
	glEnd();

}

//draw the main buildings
void drawBuilding()
{
	point top, side, top2;
	top = { 0.5, 0.5, 0.5 };
	side = { float(0.65),  float(0.5),  float(0.5) };
	top2 = { float(0.93),  float(0.93),  float(0.97) };

	glColor3f(0.36, 0.44, 0.29);
	simpleCube(-253, -194, -160, -153, 1, -2);
	simpleCube(-253, -145, -160, -105, 1, -2);
	simpleCube(38, -194, 131, -153, 1, -2);
	simpleCube(38, -145, 131, -105, 1, -2);



	simpleCube(-358, -316, -317, -310, 200, 0, top, side);
	simpleCube(-190, -316, -149, -214, 200, 0, top, side);
	simpleCube(-358, -96, -271, -92, 200, 0, top, side);
	simpleCube(-362, -310, -145, -220, 200, 0, top, side);
	simpleCube(-358, -220, -275, -137, 200, 0, top, side);
	simpleCube(-362, -137, -267, -96, 200, 0, top, side);

	simpleCube(-99, -298, -20, -283, 200, 0, top, side);
	simpleCube(-145, -259, -123, -237, 200, 133, top, side);
	simpleCube(-123, -265, 3, -186, 200, 0, top, side);
	simpleCube(-118, -283, -2, -168, 200, 0, top, side);
	simpleCube(3, -259, 25, -237, 200, 133, top, side);
	simpleCube(-99, -168, -20, -165, 200, 0, top, side);

	simpleCube(30, -314, 71, -213, 200, 0, top, side);
	simpleCube(197, -313, 236, -308, 200, 0, top, side);
	simpleCube(25, -309, 243, -218, 200, 0, top, side);
	simpleCube(152, -218, 237, -90, 200, 0, top, side);
	simpleCube(146, -133, 243, -94, 200, 0, top, side);

	simpleCube(-360, -11, -317, 101, 200, -66, top, side);
	simpleCube(-317, -5.5, -188, 95, 200, -66, top, side);
	simpleCube(-188, -11, -147, 101, 200, -66, top, side);
	simpleCube(-306, 95, -202, 146, 20, -66, top2, side);

	simpleCube(25, -11, 66, 101, 200, -66, top, side);
	simpleCube(66, -8, 200, 92, 200, -66, top, side);
	simpleCube(195, -11, 236, 101, 200, -66, top, side);
	simpleCube(80, 92, 187, 119, 20, -66, top2, side);

	glPushMatrix();
	glTranslatef(-63, 0, -163);
	glRotatef(-90, 1, 0, 0);
	glColor3f(0.93, 0.93, 0.97);
	drawHalfShape(22, 0, 0, 0, 'c', 66);
	glPopMatrix();

}

//draw the middle part
void drawMiddle()
{

	simpleCube(280, 439, 315, 470, 250);
	simpleCube(274, 483, 316, 646, 250);
	simpleCube(302, 615, 343, 673, 250);
	drawCircle(302, 645, 28, true, 180, 270);

	glColor3f(0.99, 0.98, 0.98);
	drawCircle(357, 404, 69, true, 90, 180);
	simpleCube(288, 404, 298, 435, 250);

	glColor3f(0.54, 0.47, 0.45);
	drawCircle(357, 404, 59, true, 90, 180);
	simpleCube(298, 404, 315, 435, 250);

	glColor3f(0.98, 0.94, 0.90);
	drawCircle(357, 404, 42, true, 90, 180);
	drawCircle(357, 404, 69, true, 90, 105);
	drawCircle(357, 611, 42, true, 180, 270);
	simpleCube(315, 404, 329, 618, 250);

	glColor3f(0.63, 0.6, 0.46);
	drawCircle(357, 404, 28, true, 90, 180);
	drawCircle(357, 611, 28, true, 180, 270);
	simpleCube(329, 404, 357, 435, 250);
	simpleCube(329, 484, 357, 611, 250);

	glColor3f(0.0, 0.0, 0.0);

	glBegin(GL_LINE);
	glVertex2d(288, -404);
	glVertex2d(288, -435);
	glEnd();
	drawCircle(357, 404, 69, false, 105, 180);

}

void drawNeck() {
	//draw neck
	glTranslatef(0, 501, 0);
	glRotatef(-90, 1, 0, 0);
	glutSolidCylinder(20, 34);
}

void drawHead() {
	//draw head
	glColor3f(1.0, 0.9, 0.6);
	glTranslatef(0, 567, 0);
	glutSolidSphere(40, 30, 30);
}

void drawTrunk() {
	//draw trunk
	glColor3f(0.0, 0.478, 0.95);
	glTranslatef(0, 411, 0);
	glScalef(121, 181, 41);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 310, 0);
	glColor3f(0.92, 0.91, 0.95);
	glScalef(121, 20, 41);
	glutSolidCube(1);
}

void drawArm() {
	//draw arms
	glColor3f(0.5, 0.478, 0.95);
	glTranslatef(60, 501, 0);
	glRotatef(90, 0, 1, 0);
	drawHalfShape(20, 0, -1, 0, 'c', 20);
	glBegin(GL_POLYGON);
	glVertex3f(-20, 0, 0);
	glVertex3f(-20, 0, 20);
	glVertex3f(20, 0, 20);
	glVertex3f(20, 0, 0);
	glEnd();
	glTranslatef(0, 0, 20);
	drawHalfShape(20, 0, -1, 0, 's');
	glRotatef(-90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(5, 0, 0);
	glutSolidCylinder(20, 225);
	glTranslatef(0, 0, 240);
	glColor3f(1.0, 0.9, 0.6);
	glutSolidSphere(25, 30, 30);
}

void drawTie() {
	//draw tie of bag
	glColor3f(0.2, 0.9, 0.9);
	point p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12;
	p1 = { 42, 502, 21 };
	p2 = { 18, 502, 21 };
	p3 = { 61, 416, 21 };
	p4 = { 61, 440, 21 };
	p5 = { 18, 502, -21 };
	p6 = { 42, 502, -21 };
	p7 = { 42, 452, -21 };
	p8 = { 18, 452, -21 };
	p9 = { 61, 376, -21 };
	p10 = { 61, 352, -21 };
	p11 = { 40, 352, -21 };
	p12 = { 40, 376, -21 };
	glNormal3f(0, 0, 1);
	drawPolygon(p1, p2, p3, p4, 4);
	glNormal3f(0, 1, 0);
	drawPolygon(p1, p2, p5, p6, 4);
	glNormal3f(0, 0, -1);
	drawPolygon(p5, p6, p7, p8, 4);
	glNormal3f(1, 0, 0);
	drawPolygon(p3, p4, p9, p10, 4);
	glNormal3f(0, 0, -1);
	drawPolygon(p9, p10, p11, p12, 4);

	p1.x *= -1;
	p2.x *= -1;
	p3.x *= -1;
	p4.x *= -1;
	p5.x *= -1;
	p6.x *= -1;
	p7.x *= -1;
	p8.x *= -1;
	p9.x *= -1;
	p10.x *= -1;
	p11.x *= -1;
	p12.x *= -1;
	glNormal3f(0, 0, 1);
	drawPolygon(p1, p2, p3, p4, 4);
	glNormal3f(0, 1, 0);
	drawPolygon(p1, p2, p5, p6, 4);
	glNormal3f(0, 0, -1);
	drawPolygon(p5, p6, p7, p8, 4);
	glNormal3f(-1, 0, 0);
	drawPolygon(p3, p4, p9, p10, 4);
	glNormal3f(0, 0, -1);
	drawPolygon(p9, p10, p11, p12, 4);
}

void drawLeg() {
	//draw legs
	point p1, p2, p3, p4, p5, p6, p7, p8;
	glColor3f(0.92, 0.91, 0.95);

	p1 = { 0, 301, 21 };
	p2 = { 0, 301, -21 };
	p3 = { 15, 50, -21 };
	p4 = { 15, 50, 21 };
	p5 = { 60, 301, 21 };
	p6 = { 60, 50, 21 };
	p7 = { 60, 301, -21 };
	p8 = { 60, 50, -21 };
	glNormal3f(-1, 0, 0);
	drawPolygon(p1, p2, p3, p4, 4);
	glNormal3f(0, 0, 1);
	drawPolygon(p1, p5, p6, p4, 4);
	glNormal3f(1, 0, 0);
	drawPolygon(p5, p7, p8, p6, 4);
	glNormal3f(0, 0, -1);
	drawPolygon(p7, p2, p3, p8, 4);

	p1.x *= -1;
	p2.x *= -1;
	p3.x *= -1;
	p4.x *= -1;
	p5.x *= -1;
	p6.x *= -1;
	p7.x *= -1;
	p8.x *= -1;
	glNormal3f(1, 0, 0);
	drawPolygon(p1, p2, p3, p4, 4);
	glNormal3f(0, 0, 1);
	drawPolygon(p1, p5, p6, p4, 4);
	glNormal3f(-1, 0, 0);
	drawPolygon(p5, p7, p8, p6, 4);
	glNormal3f(0, 0, -1);
	drawPolygon(p7, p2, p3, p8, 4);
}

void drawFoot() {
	//draw feet
	glColor3f(0.99, 0.41, 0.2);
	point p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
	p1 = { 60, 50, 21 };
	p2 = { 15, 50, 21 };
	p3 = { 15, 10, 70 };
	p4 = { 60, 10, 70 };
	p5 = { 60, 0, 70 };
	p6 = { 15, 0, 70 };
	p7 = { 60, 50, -21 };
	p8 = { 60, 0, -21 };
	p9 = { 15, 0, -21 };
	p10 = { 15, 50, -21 };
	glNormal3f(0, 4, 4.9);
	drawPolygon(p1, p2, p3, p4, 4);
	glNormal3f(0, 0, 1);
	drawPolygon(p3, p4, p5, p6, 4);
	glNormal3f(1, 0, 0);
	drawPolygon(p1, p4, p5, p8, 5, p7);
	glNormal3f(0, 0, -1);
	drawPolygon(p7, p8, p9, p10, 4);
	glNormal3f(-1, 0, 0);
	drawPolygon(p2, p3, p6, p9, 5, p10);

	p1.x = -1 * p1.x;
	p2.x = -1 * p2.x;
	p3.x = -1 * p3.x;
	p4.x = -1 * p4.x;
	p5.x = -1 * p5.x;
	p6.x = -1 * p6.x;
	p7.x = -1 * p7.x;
	p8.x = -1 * p8.x;
	p9.x = -1 * p9.x;
	p10.x *= -1;
	glNormal3f(0, 4, 4.9);
	drawPolygon(p1, p2, p3, p4, 4);
	glNormal3f(0, 0, 1);
	drawPolygon(p3, p4, p5, p6, 4);
	glNormal3f(-1, 0, 0);
	drawPolygon(p1, p4, p5, p8, 5, p7);
	glNormal3f(0, 0, -1);
	drawPolygon(p7, p8, p9, p10, 4);
	glNormal3f(1, 0, 0);
	drawPolygon(p2, p3, p6, p9, 5, p10);
}

void drawBag() {
	//draw the bag
	glColor3f(0.2, 0.9, 0.9);
	glTranslatef(0, 440, -50);
	drawHalfShape(55, 0, 1, 0, 'c', 30);
	glTranslatef(0, -10, 15);
	glScalef(110, 20, 30);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.25, 0.85, 0.8);
	glTranslatef(0, 385, -40);
	glScalef(110, 70, 40);
	glutSolidCube(1);
}

void drawBook() {
	//draw the book
	glColor3f(0.5, 0.25, 0.0);
	glTranslatef(0, 0, 35);
	glScalef(10, 90, 50);
	glutSolidCube(1);
}

//draw the student
void drawStudent(int chara, float length)
{
	glPushMatrix();
	glTranslatef(position[chara].x, position[chara].y, position[chara].z);
	//currentTime = glutGet(GLUT_ELAPSED_TIME);
	//float length = (currentTime - lastTime)*0.4;
	//printf("%f \n", length);
	//printf("%f %d\n", length, chara);
	if (move != 0 && chara == ::chara) {
		if (studentTurn[chara] == 0)
		{
			glTranslatef(0.0, 0.0, length);
			position[chara].z += move*length;
		}
		else if (studentTurn[chara] == 1)
		{
			glTranslatef(length, 0.0, 0.0);
			position[chara].x += move*length;
		}
		else if (studentTurn[chara] == 2)
		{
			glTranslatef(0.0, 0.0, -length);
			position[chara].z -= move*length;
		}
		else if (studentTurn[chara] == 3)
		{
			glTranslatef(-length, 0.0, 0.0);
			position[chara].x -= move*length;
		}
		if (!colDetect(position[chara])) {
			move = -move;
			if (studentTurn[chara] == 0)
			{
				glTranslatef(0.0, 0.0, length);
				position[chara].z += move*length;
			}
			else if (studentTurn[chara] == 1)
			{
				glTranslatef(length, 0.0, 0.0);
				position[chara].x += move*length;
			}
			else if (studentTurn[chara] == 2)
			{
				glTranslatef(0.0, 0.0, -length);
				position[chara].z -= move*length;
			}
			else if (studentTurn[chara] == 3)
			{
				glTranslatef(-length, 0.0, 0.0);
				position[chara].x -= move*length;
			}
		}
		move = 0;
	}
	//lastTime = currentTime;

	glRotatef(studentTurn[chara] * 90, 0, 1, 0);
	glScalef(0.067, 0.067, 0.067);
	glPushMatrix();
	drawBag();
	glPopMatrix();
	glPushMatrix();
	drawTie();
	glPopMatrix();
	glPushMatrix();
	drawNeck();
	glPopMatrix();
	glPushMatrix();
	drawHead();
	glPopMatrix();
	glPushMatrix();
	drawTrunk();

	glPopMatrix();
	glPushMatrix();
	drawArm();
	drawBook();

	glPopMatrix();
	glPushMatrix();
	drawLeg();
	glPopMatrix();
	glPushMatrix();
	drawFoot();
	glPopMatrix();
	glPushMatrix();
	//glScalef(-1, 0, 0);
	drawLeg();
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	drawArm();
	glPopMatrix();
	glPopMatrix();

}

//draw other students
void drawPeds() {
	glPushMatrix();
	glScalef(0.067, 0.067, 0.067);
	glPushMatrix();
	drawBag();
	glPopMatrix();
	glPushMatrix();
	drawTie();
	glPopMatrix();
	glPushMatrix();
	drawNeck();
	glPopMatrix();
	glPushMatrix();
	drawHead();
	glPopMatrix();
	glPushMatrix();
	drawTrunk();

	glPopMatrix();
	glPushMatrix();
	drawArm();
	drawBook();

	glPopMatrix();
	glPushMatrix();
	drawLeg();
	glPopMatrix();
	glPushMatrix();
	drawFoot();
	glPopMatrix();
	glPushMatrix();
	glScalef(-1, 0, 0);
	drawLeg();
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	drawArm();
	glPopMatrix();
	glPopMatrix();
}

void display(void)
{
	// clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if (!tour) {	//usual controll
		gluLookAt(px, py, pz, position[chara].x, position[chara].y, position[chara].z, 0, 1, 0);

		glPushMatrix();
		drawGround();
		drawBuilding();
		glPopMatrix();

		//draw 2 students
		glPushMatrix();
		currentTime = glutGet(GLUT_ELAPSED_TIME);
		float length = (currentTime - lastTime)*1.0;
		drawStudent(0, length);
		drawStudent(1, length);
		lastTime = currentTime;
		glPopMatrix();

		//draw 2 pedestrians
		glPushMatrix();
		glTranslatef(200, -66, 190);
		drawPeds();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(450, -66, -130);
		drawPeds();
		glPopMatrix();
		glPopMatrix();
	}
	else {	//auto tour
		currentTime = glutGet(GLUT_ELAPSED_TIME);
		gluLookAt((placeToTour[toured].x + 100.0), (placeToTour[toured].y + 100.0), (placeToTour[toured].z + 100.0), placeToTour[toured].x, placeToTour[toured].y, placeToTour[toured].z, 0, 1, 0);
		if (currentTime - ltime > 3000) {
			//printf("%d \n", currentTime);
			//printf("%f \n", placeToTour[toured].x + 100);
			//gluLookAt((placeToTour[toured].x + 100.0), (placeToTour[toured].y + 100.0), (placeToTour[toured].z + 100.0), placeToTour[toured].x, placeToTour[toured].y, placeToTour[toured].z, 0, 1, 0);
			ltime = currentTime;
			toured++;
			if (toured == 3)
				toured = 0;
		}

		glPushMatrix();
		drawGround();
		drawBuilding();
		glPopMatrix();

		//draw 2 students
		move = 0;
		glPushMatrix();
		drawStudent(0, 0);
		drawStudent(1, 0);

		glPopMatrix();

		//draw 2 pedestrians
		glPushMatrix();
		glTranslatef(200, -66, 190);
		drawPeds();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(450, -66, -130);
		drawPeds();
		glPopMatrix();
		glPopMatrix();
	}

	glutSwapBuffers();
}

//detect if run into a building


void pressArrow(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		if (zoom < 1)
			zoom += 1;
		break;
	case GLUT_KEY_DOWN:
		if (zoom > -1)
			zoom -= 1;
		break;
	case GLUT_KEY_LEFT:
		camaraTurn -= 5;
		break;
	case GLUT_KEY_RIGHT:
		camaraTurn += 5;
		break;
	}
	if (camaraTurn == -5)
		camaraTurn = 355;
	else if (camaraTurn == 360)
		camaraTurn = 0;
}

void pressKey(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		move += 1;
		break;
	case 's':
		move -= 1;
		break;
	case 'a':
		studentTurn[chara] -= 1;
		break;
	case 'd':
		studentTurn[chara] += 1;
		break;
	case 'l':
		light += 1;
		break;
	case 'h':
		height += 1;
		break;
	case 'c':
		chara += 1;
		break;
	case 't':
		tour = !tour;
		break;
	case '+':
		glEnable(GL_LIGHT3);
		break;
	case '-':
		glDisable(GL_LIGHT3);
		break;
	}
	if (studentTurn[chara] == -1)
		studentTurn[chara] = 3;
	else if (studentTurn[chara] == 4)
		studentTurn[chara] = 0;
	if (height == -1)
		height = 3;
	else if (height == 5)
		height = 0;
	if (light == 4)
		light = 0;
	if (chara == 2)
		chara = 0;
	/*printf("%d \n", light);*/

}

//void enableLight() {
//	if (lightnum == 1) {
//		glEnable(GL_LIGHT1);
//		glDisable(GL_LIGHT2);
//	}
//	else {
//		glEnable(GL_LIGHT1);
//		glEnable(GL_LIGHT2);
//	}
//}

void myidle()
{
	int scope = 600;
	switch (zoom) {
	case -1:
		scope = 600;
		break;
	case 0:
		scope = 300;
		break;
	case 1:
		if (height != 0)
			scope = 100;
		else
			scope = 150;
		break;
	}

	switch (height) {
	case 0:
		py = 40;
		break;
	case 1:
		py = 150;
		break;
	case 2:
		py = 250;
		break;
	case 3:
		py = 400;
		break;
	case 4:
		py = 600;
		break;
	}
	switch (light) {
	case 0:
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		break;
	case 1:
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		break;
	case 2:
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		break;
	case 3:
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		break;
		//case 4:
		//	glDisable(GL_LIGHTING);
		//	break;
	}

	px = scope * sin(camaraTurn / 180 * PI) + position[chara].x;
	py = py + position[chara].y;
	pz = scope * cos(camaraTurn / 180 * PI) + position[chara].z;

	GLfloat pos[] = { px, py, pz ,1 };
	GLfloat dire[] = { position[chara].x - px, position[chara].y - py, position[chara].z - pz,1 };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dire);
	glLightfv(GL_LIGHT3, GL_POSITION, pos);
	glutPostRedisplay();
}

void init()
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(35, 1, 120, 3000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	gluLookAt(0, 400, 600, position[chara].x, position[chara].y, position[chara].z, 0, 1, 0);

	GLfloat lightAmbient[] = { 0.5,0.5,0.5, 1 };

	GLfloat lightDiffuse[] = { 0.85, 0.9, 1.0, 0.5 };
	GLfloat lightDiffuse1[] = { 1.0, 1.0, 1.0, 0.5 };
	GLfloat lightDiffuse2[] = { 1.0, 0.9, 0.85, 0.5 };
	GLfloat lightDiffuse3[] = { 0.5, 1.0, 0.5, 0.5 };

	//GLfloat lightDirection[] = { -1.0, -1.0, -1.0 };
	GLfloat lightSpecular[] = { 0.5, 0.5, 0.5, 0.5 };

	GLfloat lightPosition[] = { 600,600,0,0 };
	GLfloat lightPosition1[] = { 0,800,0,0 };
	GLfloat lightPosition2[] = { -600,600,0,0 };
	GLfloat lightPosition3[] = { px,py,pz,1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);  //Setup The Ambient Light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);  //Setup The Diffuse Light
													 //glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);

	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse2);
	//glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition2);

	glLightfv(GL_LIGHT3, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lightDiffuse3);
	glLightfv(GL_LIGHT3, GL_SPECULAR, lightSpecular);
	//glLightfv(GL_LIGHT3, GL_POSITION, lightPosition2);
	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);  //spot direction
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 12.0);   //spot cutoff   

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_COLOR_MATERIAL);
	glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

}

void myReshape(int w, int h)
{

}

int main(int argc, char** argv)
{

	/* Initialize mode and open a window in upper left corner of screen */
	/* Window title is name of program (arg[0]) */

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(Width, Height);
	//glutReshapeFunc(myReshape);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glutIdleFunc(myidle);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Shelby");

	glutDisplayFunc(display);
	init();
	glutSpecialFunc(pressArrow);
	glutKeyboardFunc(pressKey);


	glutMainLoop();

}
