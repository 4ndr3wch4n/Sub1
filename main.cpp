/*******************************************************************
	       Multi-Part Model Construction and Manipulation
********************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "CubeMesh.h"
#include "QuadMesh.h"

#define M_PI 3.14159265358979323846

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
VECTOR3D ScreenToWorld(int x, int y);
void spinAccelerate(void);

static GLfloat theta = 0.0;
static GLfloat spin = 0.0;
static float spinSpeed = 0.0;
static float moveSub = 0.0;
static float moveSubz = 0.0;
static float subSpeed = 0.0;
static float moveSubVert = 4.0;
static float xSub = 0.0;
static float z = 0.0;
static int currentButton;
static unsigned char currentKey;

GLfloat light_position0[] = {-6.0,  12.0, 0.0,1.0};
GLfloat light_position1[] = { 6.0,  12.0, 0.0,1.0};
GLfloat light_diffuse[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_ambient[]   = {0.2, 0.2, 0.2, 1.0};


// Set up lighting/shading and material properties for submarine - upcoming lecture - just copy for now
GLfloat submarine_mat_ambient[] = { 0.4, 0.2, 0.0, 1.0 };
GLfloat submarine_mat_specular[] = { 0.1, 0.1, 0.0, 1.0 };
GLfloat submarine_mat_diffuse[] = { 0.9, 0.5, 0.0, 1.0 };
GLfloat submarine_mat_shininess[] = { 0.0 };

QuadMesh *groundMesh = NULL;


struct BoundingBox
{
  VECTOR3D min;
  VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 16;

int main(int argc, char **argv)
{
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Scene Modeller");

  initOpenGL(500,500);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotionHandler);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(functionKeys);
  
  glutMainLoop();
  return 0;
}



// Setup openGL */
void initOpenGL(int w, int h)
{
  // Set up viewport, projection, then change to modelview matrix mode - 
  // display function will then set up camera and modeling transforms
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.2,80.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set up and enable lighting
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHT1);
  
  // Other OpenGL setup
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.6, 0.6, 0.6, 0.0);  
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  // This one is important - renormalize normal vectors 
  glEnable(GL_NORMALIZE);
  
  //Nice perspective.
  glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST);
  
  // Set up ground quad mesh
  VECTOR3D origin  = VECTOR3D(-8.0f,0.0f,8.0f);
  VECTOR3D dir1v   = VECTOR3D(1.0f, 0.0f, 0.0f);
  VECTOR3D dir2v   = VECTOR3D(0.0f, 0.0f,-1.0f);
  groundMesh = new QuadMesh(meshSize, 16.0);
  groundMesh->InitMesh(meshSize, origin, 16.0, 16.0, dir1v, dir2v);
  
  VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
  VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
  VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
  float shininess = 0.2;
  groundMesh->SetMaterial(ambient,diffuse,specular,shininess);

  // Set up the bounding box of the scene
  // Currently unused. You could set up bounding boxes for your objects eventually.
  BBox.min.Set(-8.0f, 0.0, -8.0);
  BBox.max.Set( 8.0f, 6.0,  8.0);
}



void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glLoadIdentity();
  
  // Set up the camera
  gluLookAt(0.0,6.0,22.0,0.0,0.0,0.0,0.0,1.0,0.0);
  
  // Draw Submarine

  // Set submarine material properties
  glMaterialfv(GL_FRONT, GL_AMBIENT, submarine_mat_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, submarine_mat_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, submarine_mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SHININESS, submarine_mat_shininess);

  // Apply transformations to move submarine
  // ...
  glPushMatrix();
  glTranslatef(xSub, moveSubVert, z);
  if (xSub <= BBox.min.x) { spin = 0; spinSpeed = 0; subSpeed = 0; xSub = BBox.min.x + 2.0;  }
  if (xSub > BBox.max.x) { spin = 0; spinSpeed = 0; subSpeed = 0; xSub = BBox.max.x - 2.0; }
  if (z <= BBox.min.z) { spin = 0; spinSpeed = 0; subSpeed = 0; z = BBox.min.z + 2.0; }
  if (z > BBox.max.z) { spin = 0; spinSpeed = 0; subSpeed = 0; z = BBox.max.z - 2.0; }
  if (moveSubVert <= BBox.min.y) { spin = 0; spinSpeed = 0; subSpeed = 0; moveSubVert = BBox.min.y + 0.5; }
  if (moveSubVert > BBox.max.y) { spin = 0; spinSpeed = 0; subSpeed = 0; moveSubVert = BBox.max.y - 0.5; }
  glRotatef(theta, 0.0, 1.0, 0.0);

  // Apply transformations to construct submarine
  glPushMatrix();
  glTranslatef(0.0, 4.0, 0.0);
  //glRotatef(45.0, 0.0, 1.0, 0.0);
  glScalef(3.0, 1.0, 1.0);
  glutSolidSphere(1.0, 20, 20);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, 5.0, 0.0);
  //glRotatef(90.0, 0.0, 1.0, 0.0);
  glScalef(2.0, 1.0, 0.25);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.3, 4.0, 0);
  glRotatef(spin, 1.0, 0.0, 0.0);
  glScalef(0.5, 0.50, 0.5);
  glTranslatef(-6.3, -4.0, 0);

  glPushMatrix();
  glTranslatef(5.3, 3.5, 0.0);
  //glRotatef(1*theta, 1.0, 0.0, 0.0);
  //glScalef(1.0, 0.5, 0.25);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(5.3, 4.0, -0.3);
  glRotatef(120.0, 1.0, 0.0, 0.0);
  //glScalef(1.0, 0.5, 0.25);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(5.3, 4.0, 0.3);
  glRotatef(240.0, 1.0, 0.0, 0.0);
  //glScalef(1.0, 0.5, 0.25);
  glutSolidCube(1.0);
  glPopMatrix();

  glPopMatrix();

  glPopMatrix();
  // Draw ground
  groundMesh->DrawMesh(meshSize);

  glutSwapBuffers();
}


// Called at initialization and whenever user resizes the window */
void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.2,40.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

VECTOR3D pos = VECTOR3D(0,0,0);

// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
  currentButton = button;

  switch(button)
  {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN)
	{
	  ;
	  
	}
	break;
  case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN)
	{
	  ; 
	}
	break;
  default:
	break;
  }
  glutPostRedisplay();
}

// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
  if (currentButton == GLUT_LEFT_BUTTON)
  {
	  ;
  }
  glutPostRedisplay();
}


VECTOR3D ScreenToWorld(int x, int y)
{
	// you will need this if you use the mouse
	return VECTOR3D(0);
}// ScreenToWorld()

/* Handles input from the keyboard, non-arrow keys */
void keyboard(unsigned char key, int x, int y)
{
  switch (key) 
  {
  case 'q':
	  subSpeed += 0.02;
	  if (spinSpeed < 3.0) spinSpeed += 0.5;
	  else spinSpeed = 2.5;
	  glutIdleFunc(spinAccelerate);
	  break;
  case 'w':
	  if (subSpeed > 0) {
		  spinSpeed -= 0.5;
		  subSpeed -= 0.02;
		  glutIdleFunc(spinAccelerate);
		  break;
	  }
  }

  //glutPostRedisplay();
}

void functionKeys(int key, int x, int y)
{
  VECTOR3D min, max;

  if (key == GLUT_KEY_F1)
  {
	  MessageBox(NULL, "Use q to accelerate \nUse w to brake \nUse Up arrow key to ascend \nUse Down arrow key to descend \nUse Left arrow key to turn left \nUse Right arrow key to turn right", "Help", NULL);
  }
  /*
  // Do transformation code with arrow keys
  // GLUT_KEY_DOWN, GLUT_KEY_UP,GLUT_KEY_RIGHT, GLUT_KEY_LEFT
  else if (...)
  {
  }
  */
  if (key == GLUT_KEY_UP)
  {
	  moveSubVert += 0.05;
	  spin += 5.0;
  }
  else if (key == GLUT_KEY_DOWN)
  {
	  moveSubVert -= 0.05;
	  spin += 5.0;
  }
  else if (key == GLUT_KEY_LEFT)
  {
	  theta += 2.0;
	  spin += 5.0;
	  if (theta > 360.0)
		  theta -= 360.0;
  }
  else if (key == GLUT_KEY_RIGHT)
  {
	  theta -= 2.0;
	  spin += 5.0;
	  if (theta < 0.0)
		  theta += 360.0;
  }
  glutPostRedisplay();
}
void spinAccelerate(void)
{
	spin += spinSpeed;
	if (spin > 360.0)
		spin -= 360.0;

	xSub -= subSpeed * cos(theta / 180.0*M_PI);
	z += subSpeed * sin(theta / 180.0*M_PI);

	glutPostRedisplay();
}
