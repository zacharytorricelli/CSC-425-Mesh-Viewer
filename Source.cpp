/****************************************************************************************************************************
*
* Zachary Torricelli
* CSC 425 Computer Graphics - Dr. Bo Li
* 
* 3D Mesh Viewer using .M Files Provided
* Allows the user to change models and project type.
* Allows for mouse and motion usage.
* Right click to pop-up a menu and select your model.
* You will want to use the keyboard keys 'f', 'p', 's', and 'w'.
*
*****************************************************************************************************************************/\



// Include files and using directive
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cmath>


using namespace std;


// Global variables
const int MAX_CHARACTERS = 1024;
const int MAX_OBJECTS = 20;
const char* const FORM = " ";


float maxWidth;
float x_max;
float x_min;
float y_max;
float y_min;
float z_max;
float z_min;
float moveX = 0.0;
float moveY = 0.0;
float moveZ = 0.0;
float moveObjX = 0.0;
float moveObjY = 0.0;
float moveObjZ = 0.0;
float rotationX = 0.0;
float rotationY = 0.0;
float rotationZ = 0.0;
float rotationXZ = 0.0;
float rotateX = 0.0;
float rotateY = 0.0;
float rotateZ = 0.0;
int oldX;
int oldY;
float zoom = 1.0;
int leftbutton = 0;
int rightbutton = 0;
int middlebutton = 0;
static int myWindow;
static int menid;
static int submenid;
static int primitive = 0;
int obj_type = 0;
int proj_type = 0;
int wireframe = 3;
int	points = 2;
int flat = 1;
int smooth = 0;
int showAxesAndPlane = 1;
int orthogonal = 0;
int perspective = 1;
int showBoundingBox = 1;
int bunny = 0;
int cap = 1;
int gargoyle = 0;
int knot = 0;
int eight = 0;



// Struct used to store the vertex
struct Vertex

{

	GLfloat x, y, z;
	GLfloat vertexNormal[3];

};

int vertexCounter = 0;
Vertex *vertices;


// Struct used to store the face
struct Face

{

	GLint vertexIndices[3];
	GLfloat faceNormal[3];

};

int faceCounter = 0;
Face *faces;


struct halfEdgeVertex;
struct halfEdgeFace;


// Struct used for the half edges
struct halfEdge {

	
	halfEdgeVertex *vertex; // vertex at the end of the half-edge
	halfEdge *pair; // oppositely oriented half-edge
	halfEdgeFace *face; // the incident face
	halfEdge *previous; // previous half-edge around the face
	halfEdge *next; // next half-edge around the face

};

halfEdge *halfEdges;
int halfEdgeCounter;


// Struct used for the half edge vertices 
struct halfEdgeVertex 
{

	// Vertex coordinates
	float x;
	float y;
	float z;

	halfEdge* edge; 

	// Vertex normal coordinates
	float vertexNormal_x;
	float vertexNormal_y;
	float vertexNormal_z; 


};

halfEdgeVertex *vertexHalfEdge;
int halfEdgeVertexCounter;


// Struct used for the half edge faces
struct halfEdgeFace
{

	halfEdge *edge; 

	float surfaceNormal_x;
	float surfaceNormal_y;
	float surfaceNormal_z;
	float surfaceArea; 

};

halfEdgeFace *faceHalfEdge;
int halfEdgeFaceCounter;

int *edgePair;

Vertex center;



// Various lighting setting variables
GLfloat light0_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat light0_diffuse[] = { .6f, .6f, 1.0f, 1.0f };
GLfloat light0_position[] = { .5f, .5f, 1.0f, 0.0f };
GLfloat light1_ambient[] = { 0.1f, 0.1f, 0.3f, 1.0f };
GLfloat light1_diffuse[] = { .9f, .6f, 0.0f, 1.0f };
GLfloat light1_position[] = { -1.0f, -1.0f, 1.0f, 0.0f };



// Initializes all half-edges of the 3D model
void initializeHalfEdge() 
{


	// Variables used in this function
	int vertexStart;
	int append;
	int k;

	halfEdgeVertexCounter = vertexCounter;
	halfEdgeFaceCounter = faceCounter;
	halfEdgeCounter = faceCounter * 3;
	faceHalfEdge = new halfEdgeFace[halfEdgeFaceCounter];
	vertexHalfEdge = new halfEdgeVertex[halfEdgeVertexCounter];
	halfEdges = new halfEdge[halfEdgeCounter];
	edgePair = new int[halfEdgeCounter * 3];



	for (int i = 0; i < halfEdgeVertexCounter; i++)
	{


		vertexHalfEdge[i].x = vertices[i].x;
		vertexHalfEdge[i].y = vertices[i].y;
		vertexHalfEdge[i].z = vertices[i].z;
		vertexHalfEdge[i].edge = NULL;

	}



	for (int i = 0; i < halfEdgeFaceCounter; i++)
	{

		faceHalfEdge[i].edge = NULL;

	}



	for (int i = 0; i < halfEdgeCounter * 3; i++)
	{

		edgePair[i] = -1;

	}



	for (int i = 0; i < faceCounter; i++)
	{


		faceHalfEdge[i].edge = &halfEdges[i * 3];



		for (int j = 0; j <= 2; j++)
		{

			vertexStart = faces[i].vertexIndices[j] - 1;
			append = faces[i].vertexIndices[(j + 1) % 3] - 1;

			
			if (!vertexHalfEdge[vertexStart].edge)
			{

				vertexHalfEdge[vertexStart].edge = &halfEdges[3 * i + j];

			}


			halfEdges[3 * i + j].vertex = &vertexHalfEdge[vertexStart];
			halfEdges[3 * i + j].face = &faceHalfEdge[i];
			halfEdges[3 * i + j].previous = &halfEdges[(3 * i) + (j == 0 ? 2 : (j - 1))];
			halfEdges[3 * i + j].next = &halfEdges[(3 * i) + (j == 2 ? 0 : (j + 1))];
			halfEdges[3 * i + j].pair = NULL;

			k = 3 * i + j;

			edgePair[3 * k + 0] = 3 * i + j;
			edgePair[3 * k + 1] = vertexStart;
			edgePair[3 * k + 2] = append;

		}


	}
	

}


// Function to calculate the face and vertex normals of the 3D models
void calculateNormal() 
{

	// Variables used in this function
	halfEdgeVertex point1, point2, point3;
	float p1p2[3];
	float p1p3[3];
	halfEdge *current;
	halfEdgeVertex *vertex;
	float faceNormal;
	float vertexNormal;



	for (int i = 0; i < halfEdgeFaceCounter; i++)

	{

		current =faceHalfEdge[i].edge;

		point1.x = current->vertex->x;
		point1.y = current->vertex->y;
		point1.z = current->vertex->z;


		point2.x = current->next->vertex->x;
		point2.y = current->next->vertex->y;
		point2.z = current->next->vertex->z;


		point3.x = current->previous->vertex->x;
		point3.y = current->previous->vertex->y;
		point3.z = current->previous->vertex->z;


		p1p2[0] = point2.x - point1.x;
		p1p2[1] = point2.y - point1.y;
		p1p2[2] = point2.z - point1.z;


		p1p3[0] = point3.x - point1.x;
		p1p3[1] = point3.y - point1.y;
		p1p3[2] = point3.z - point1.z;


		current->face->surfaceNormal_x = (p1p2[1] * p1p3[2]) - (p1p3[1] * p1p2[2]);
		current->face->surfaceNormal_y = (p1p2[2] * p1p3[0]) - (p1p3[2] * p1p2[0]);
		current->face->surfaceNormal_z = (p1p2[0] * p1p3[1]) - (p1p3[0] * p1p2[1]);


		// Calculates the face normal value
		faceNormal = sqrt(current->face->surfaceNormal_x * current->face->surfaceNormal_x
			+ current->face->surfaceNormal_y * current->face->surfaceNormal_y + 
			current->face->surfaceNormal_z * current->face->surfaceNormal_z);


		// Calculates the surface area
		current->face->surfaceNormal_x /= faceNormal;
		current->face->surfaceNormal_y /= faceNormal;
		current->face->surfaceNormal_z /= faceNormal;
		current->face->surfaceArea = 0.5 * faceNormal;

	}
	


	for (int i = 0; i < halfEdgeVertexCounter; i++)
	{


		vertex = &vertexHalfEdge[i];
		current = vertex->edge;

		vertex->vertexNormal_x = (current->face->surfaceNormal_x * current->face->surfaceArea);
		vertex->vertexNormal_y = (current->face->surfaceNormal_y * current->face->surfaceArea);
		vertex->vertexNormal_z = (current->face->surfaceNormal_z * current->face->surfaceArea);



		while (true)
		{

			if (current->pair)

				if (current->pair->next == vertex->edge)
				{

					break;

				}

				else
				{
				
					current = current->pair->next;
				
				}
			
			
			else
			{

				if (!current->previous->pair)
				{

					break;

				}


				while (current->previous->pair) 
				{ 
					
					current = current->previous->pair; 
				
				}


				if ((current->previous == vertex->edge) || (current == vertex->edge))
				{

					break;

				}

			}



			vertex->vertexNormal_x += (current->face->surfaceNormal_x * current->face->surfaceArea);
			vertex->vertexNormal_y += (current->face->surfaceNormal_y * current->face->surfaceArea);
			vertex->vertexNormal_z += (current->face->surfaceNormal_z * current->face->surfaceArea);

		}


		// Calculates the vertex normal
		vertexNormal = sqrt(vertex->vertexNormal_x * vertex->vertexNormal_x
			+ vertex->vertexNormal_y * vertex->vertexNormal_y + 
			vertex->vertexNormal_z * vertex->vertexNormal_z);



		vertex->vertexNormal_x /= vertexNormal;
		vertex->vertexNormal_y /= vertexNormal;
		vertex->vertexNormal_z /= vertexNormal;

	}

}


// Function to calculate/draw the bounding box and move the 3D model to the origin
void calculateBoundingBox() 
{

	
	x_max = vertices[0].x;
	x_min = vertices[0].x;
	y_max = vertices[0].y;
	y_min = vertices[0].y;
	z_max = vertices[0].z;
	z_min = vertices[0].z;


	for (int i = 0; i < vertexCounter; i++)
	{


		if (vertices[i].x > x_max)
		{

			x_max = vertices[i].x;

		}

		if (vertices[i].x < x_min)
		{

			x_min = vertices[i].x;

		}


		if (vertices[i].y > y_max)
		{

			y_max = vertices[i].y;

		}

		if (vertices[i].y < y_min)
		{

			y_min = vertices[i].y;

		}

		if (vertices[i].z > z_max)
		{

			z_max = vertices[i].z;

		}

		if (vertices[i].z < z_min)
		{

			z_min = vertices[i].z;

		}


	}


	center.x = x_min + (x_max - x_min) / 2;
	center.y = y_min + (y_max - y_min) / 2;
	center.z = z_min + (z_max - z_min) / 2;


	if (abs(x_max - x_min)  >abs(y_max - y_min))

	{

		if (abs(x_max - x_min) > abs(z_max - z_min))
		{

			maxWidth = abs(x_max - x_min);

		}

		else
		{

			maxWidth = abs(z_max - z_min);

		}

	}

	else

	{

		if (abs(y_max - y_min) > abs(z_max - z_min))
		{

			maxWidth = abs(y_max - y_min);

		}

		else
		{

			maxWidth = abs(z_max - z_min);

		}

	}
	

}


// Function to move the model to the origin
void moveToOrigin() // Moving centre of 3D model to the origin
{

	


	for (int i = 0; i < vertexCounter; i++)
	{

		vertices[i].x = vertices[i].x - center.x;
		vertices[i].y = vertices[i].y - center.y;
		vertices[i].z = vertices[i].z - center.z;

	}



	x_max -= center.x;
	x_min -= center.x;
	y_max -= center.y;
	y_min -= center.y;
	z_max -= center.z;
	z_min -= center.z;

}


// Function to scale the model to a suitable size
void scaleModel() 
{

	
	// Variables used to scale the model
	float scale;


	scale = 1 / maxWidth;



	for (int i = 0; i < vertexCounter; i++)
	{

		vertices[i].x = vertices[i].x *scale;
		vertices[i].y = vertices[i].y *scale;
		vertices[i].z = vertices[i].z *scale;

	}


	x_max *= scale;
	x_min *= scale;
	y_max *= scale;
	y_min *= scale;
	z_max *= scale;
	z_min *= scale;


}


// Function to parse and read in the 3D .M model files
int readFile(char* filename) 
{


	// Variables used in this function
	int line = 0;
	faceCounter = 0;
	vertexCounter = 0;
	ifstream myFile;

	// Opens the input file
	myFile.open(filename);
	



	while (!myFile.eof())
	{

		char buf[MAX_CHARACTERS];
		myFile.getline(buf, MAX_CHARACTERS);
		line++;

	}


	myFile.close();



	if (vertices)
	{

		delete vertices;

	}


	if (faces)
	{

		delete faces;

	}



	vertices = new Vertex[line];
	faces = new Face[line];
	

	myFile.open(filename);


	while (!myFile.eof())
	{


		char buf[MAX_CHARACTERS];
		myFile.getline(buf, MAX_CHARACTERS);
		const char* token[MAX_OBJECTS] = {};



		int i = 0, j;

		token[0] = strtok(buf, FORM);


		if (token[0])
		{

			for (i = 1; i < MAX_OBJECTS; i++)
			{

				token[i] = strtok(0, FORM);

			}

			if (strcmp(token[0], "#") != 0)
			{

				if (strcmp(token[0], "Vertex") == 0)
				{

					j = atoi(token[1]) - 1;
					vertices[j].x = stof(token[2]);
					vertices[j].y = stof(token[3]);
					vertices[j].z = stof(token[4]);
					vertexCounter++;

				}

				if (strcmp(token[0], "Face") == 0)
				{

					j = atoi(token[1]) - 1;

					faces[j].vertexIndices[0] = atoi(token[2]);
					faces[j].vertexIndices[1] = atoi(token[3]);
					faces[j].vertexIndices[2] = atoi(token[4]);
					faceCounter++;

				}

			}

		}

	}

	calculateBoundingBox();
	moveToOrigin();
	scaleModel();
	initializeHalfEdge();
	calculateNormal();


	return 1;

}


// Function used to setup mouse usage
void myMouse(int button, int state, int x, int y)
{

	oldX = x;
	oldY = y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

		leftbutton = 1;
		rightbutton = 0;
		middlebutton = 0;

	}

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{

		leftbutton = 0;
		rightbutton = 1;
		middlebutton = 0;

	}

	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{

		leftbutton = 0;
		rightbutton = 0;
		middlebutton = 1;

	}

}


// Function used to setup motion
void myMotion(int x, int y)
{


	// Function variables
	int dx = oldX - x;
	int dy = oldY - y;



	// Scaling
	if (rightbutton == 1) 
	{

		if (dx*dy > 0)
		{

			zoom += (0.01*(dx + dy));

		}

		else
		{

			zoom += (0.01*((-1 * dx) + dy));

		}

		if (proj_type == 0)
		{

			if (zoom > 2.5)
			{

				zoom = 2.5;

			}

			if (zoom < 0.2)
			{
				zoom = 0.2;

			}
		}

		else
		{

			if (zoom > 3.75)
			{

				zoom = 3.75;

			}
			
			if (zoom < 0.35)
			{

				zoom = 0.35;
			
			}

		}

	}

	// Rotation
	else if (leftbutton == 1) 
	{

		if (abs(dy) >= 10 * abs(dx))
		{

			rotationXZ -= 0.1*dy;

		}

		else if (abs(dx) > 10 * abs(dy))
		{

			rotationY += 0.1*dx;
		
		}
		
		else
		{

			if (dx*dy > 0)
			{

				rotationZ += 0.05*(dx + dy);

			}

			else
			{
				rotationX -= 0.05*((-1 * dx) + dy);

			}

		}



		if (rotationX >= 360)
		{
			
			rotationX -= 360;
		
		}

		if (rotationY >= 360)
		{
			
			rotationY -= 360;
		
		}
		if (rotationZ >= 360)
		{

			rotationZ -= 360;

		}

		if (rotationX <= -360)
		{

			rotationX += 360;

		}
		
		if (rotationY <= -360)
		{

			rotationY += 360;

		}

		if (rotationZ <= -360)
		{

			rotationZ += 360;

		}

	}

	// Translation
	else if (middlebutton == 1) 
	{


		if (abs(dx) >= abs(2 * dy))
		{

			moveX -= 0.1 * dx;
			moveZ += 0.1  *dx;

		}

		else if (abs(dy) >= abs(2 * dx))
		{

			moveY += 0.1 * dy;

		}

		else if (abs(abs(dy) - abs(dx)) <= 5)
		{

			if (dx*dy > 0)
			{

				moveX -= 0.1*(dx + dy);

			}

			else
			{

				moveZ -= 0.1*((-1 * dx) + dy);

			}

		}


		if (moveX >= 5)
		{

			moveX = 5;

		}

		if (moveY >= 5)
		{

			moveY = 5;

		}

		if (moveZ >= 5)
		{

			moveZ = 5;

		}

		if (moveX <= -5)
		{

			moveX = -5;

		}

		if (moveY <= -5)
		{

			moveY = -5;

		}

		if (moveZ <= -5)
		{

			moveZ = -5;

		}

	}


	oldX = x;
	oldY = y;


	// Forces the redraw function
	glutPostRedisplay();

}


// Draws the 3D model
void drawMesh() 

{

	halfEdge *current;

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < halfEdgeFaceCounter; i++)

	{

		current = faceHalfEdge[i].edge;


		glColor3d(current->previous->vertex->z, current->previous->vertex->z, current->previous->vertex->z);
		

		glNormal3f(current->previous->vertex->vertexNormal_x, 
			current->previous->vertex->vertexNormal_y, 
			current->previous->vertex->vertexNormal_z);
		glVertex3f(current->previous->vertex->x, current->previous->vertex->y, current->previous->vertex->z);

		
		glNormal3f(current->vertex->vertexNormal_x, 
			current->vertex->vertexNormal_y,
			current->vertex->vertexNormal_z);
		glVertex3f(current->vertex->x, current->vertex->y, current->vertex->z);


		glNormal3f(current->next->vertex->vertexNormal_x,
			current->next->vertex->vertexNormal_y, 
			current->next->vertex->vertexNormal_z);
		glVertex3f(current->next->vertex->x, current->next->vertex->y, current->next->vertex->z);
		

	}

	glEnd();

}


// Function used to draw the bounding box
void drawBoundingBox() 

{


	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glColor3d(1, 0, 1);

	glBegin(GL_LINES);


	glVertex3f(x_min, y_min, z_min);
	glVertex3f(x_max, y_min, z_min);
	glVertex3f(x_min, y_min, z_max);
	glVertex3f(x_max, y_min, z_max);
	glVertex3f(x_min, y_min, z_min);
	glVertex3f(x_min, y_min, z_max);
	glVertex3f(x_max, y_min, z_min);
	glVertex3f(x_max, y_min, z_max);
	glVertex3f(x_min, y_max, z_min);
	glVertex3f(x_max, y_max, z_min);
	glVertex3f(x_min, y_max, z_max);
	glVertex3f(x_max, y_max, z_max);
	glVertex3f(x_min, y_max, z_min);
	glVertex3f(x_min, y_max, z_max);
	glVertex3f(x_max, y_max, z_min);
	glVertex3f(x_max, y_max, z_max);
	glVertex3f(x_min, y_min, z_min);
	glVertex3f(x_min, y_max, z_min);
	glVertex3f(x_max, y_min, z_min);
	glVertex3f(x_max, y_max, z_min);
	glVertex3f(x_min, y_min, z_max);
	glVertex3f(x_min, y_max, z_max);
	glVertex3f(x_max, y_min, z_max);
	glVertex3f(x_max, y_max, z_max);


	glEnd();



}


// Function used to draw the plane and axes
void drawPlaneAndAxes() 

{


	float x = -2;
	double plane = 0.2;
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();

	while (x < 2.2)

	{

		glColor3f(1.0, 1.0, 0.0);

		glBegin(GL_LINES);

		glVertex3f(x, 0.0, -2.0);
		glVertex3f(x, 0.0, 2.0);
		glVertex3f(-2.0, 0.0, x);
		glVertex3f(2.0, 0.0, x);

		x += plane;

		glEnd();

	}

	

	// Draws the z-axis
	glPushMatrix();
	glColor3f(0, 0, 1);
	gluCylinder(quadratic, 0.01f, 0.01f, 1.0f, 32, 32);
	glTranslatef(0, 0, 1);
	glutSolidCone(0.02f, 0.06f, 8, 8);
	glPopMatrix();


	// Draws the x-axis
	glPushMatrix();
	glColor3f(1, 0, 0);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadratic, 0.01f, 0.01f, 1.0f, 32, 32);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1, 0, 0);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidCone(0.02f, 0.06f, 8, 8);
	glPopMatrix();


	// Draws the y-axis
	glPushMatrix();
	glColor3f(0, 1, 0);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, 0.01f, 0.01f, 1.0f, 32, 32);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 1, 0);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.02f, 0.06f, 8, 8);
	glPopMatrix();


	glFlush();


}


// Function used to create a menu
void menu(int value)
{
	if (value == 0)
	{
		glutDestroyWindow(myWindow);
		exit(0);
	}
	else
	{
		primitive = value;
	}

	// you would want to redraw now
	glutPostRedisplay();
}


// Function used to create a menu and submenu
void createMenu(void)
{
	// Create a submenu, this has to be done first.
	submenid = glutCreateMenu(menu);

	// Add sub menu entry
	glutAddMenuEntry("Bunny", 2);
	glutAddMenuEntry("Cap", 3);
	glutAddMenuEntry("Figure Eight", 4);
	glutAddMenuEntry("Gargoyle", 5);
	glutAddMenuEntry("Knot", 6);


	// Create the menu, this menu becomes the current menu
	menid = glutCreateMenu(menu);


	glutAddMenuEntry("Clear", 1);
	glutAddSubMenu("Draw", submenid);
	// Create an entry
	glutAddMenuEntry("Quit", 0);

	// Let the menu respond on the right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}


// Function used to display the models
void display(void)
{


	glClearColor(.9f, .9f, .9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(135, 1, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.3f, 1.2f, 0.5f, 0, 0, 0, 0, 1, 0);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	
	// Puts the model in orthogonal projection. Do not know yet how to implement

	/*
		glOrtho(-2.5, 2.5, -2.5, 2.5, 0.1, 50);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(10, 7, 10, 0, 0, 0, 0, 1, 0);
	*/
	



	//Camera Control
	glScalef(zoom, zoom, zoom);
	glTranslatef(moveX, moveY, moveZ);
	glRotatef(rotationXZ, 1, 0, -1);
	glRotatef(rotationX, 1, 0, 0);
	glRotatef(rotationY, 0, 1, 0);
	glRotatef(rotationZ, 0, 0, 1);



	// Displaying the axes and plane
	if (showAxesAndPlane)
	{

		drawPlaneAndAxes();

	};



	//Object transformations
	glScalef(1.0, 1.0, 1.0);
	glTranslatef(moveObjX, moveObjY, moveObjZ);
	glRotatef(rotateX, 1, 0, 0);
	glRotatef(rotateY, 0, 1, 0);
	glRotatef(rotateZ, 0, 0, 1);



	//Determining and loading 3D object to be displayed
	if (primitive == 1)
	{

		glutPostRedisplay();

	}

	else if (primitive == 2)
	{

		if (readFile("bunny.m") == 0) return;
		bunny = 1; cap = 0; gargoyle = 0; knot = 0; eight = 0;

	}

	else if (primitive == 3)
	{
	

		if (readFile("cap.m") == 0) return;
		bunny = 0; cap = 1; gargoyle = 0; knot = 0; eight = 0;

	}


	else if (primitive == 4)
	{
		

		if (readFile("eight.m") == 0) return;
		bunny = 0; cap = 0; gargoyle = 0; knot = 0; eight = 1;

	}

	else if (primitive == 5)
	{

		if (readFile("gargoyle.m") == 0) return;
		bunny = 0; cap = 0; gargoyle = 1; knot = 0; eight = 0;

	}

	else if (primitive == 6)
	{
		
		if (readFile("knot.m") == 0) return;
		bunny = 0; cap = 0; gargoyle = 0; knot = 1; eight = 0;

	}



	// Determines the rendering scheme 
	if (obj_type == wireframe)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}
	else if (obj_type == points)
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	}
	else if (obj_type == flat)
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_FLAT);

	}
	else
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);

	}


	// Draws the mesh
	drawMesh();
	

	// Displaying the bounding box
	if (showBoundingBox)
	{

		drawBoundingBox();
	};



	// Swaps buffers
	glutSwapBuffers();


}


// Function used to switch the projection type
void myKey(unsigned char key, int x, int y)
{

	switch (key)
	{
	
	case 'f':
		cout << "key 'f' is pressed! draw the object in flat" << endl;
		obj_type = flat;
		break;
	case 'p':
		cout << "key 'p' is pressed! draw the object in points" << endl;
		obj_type = points;
		break;
	case 's':
		cout << "key 's' is pressed! draw the object in smooth" << endl;
		obj_type = smooth;
		break;
	case 'w':
		cout << "key 'w' is pressed! draw the object in wireframe" << endl;
		obj_type = wireframe;
		break;
	}

	// force the redraw function
	glutPostRedisplay();
}


// Main driver function
int main(int argc, char **argv)
{

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition(10, 10);
	glutInitWindowSize(750, 750);

	myWindow = glutCreateWindow("CSC 425 Programming Assignment 1 - Zachary Torricelli");

	createMenu();

	glutDisplayFunc(display);
	glutMotionFunc(myMotion);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKey);


	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);


	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);


	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	return EXIT_SUCCESS;

}