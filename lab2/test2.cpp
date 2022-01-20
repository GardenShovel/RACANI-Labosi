//*************************************************************************************************************
//	Crtanje tijela
//	Tijelo se crta naredbom glutWireCube (velicina)
//	
//	Zadatak: Treba ucitati tijelo zapisano u *.obj, sjencati i ukloniti staznje poligone
//	S tastature l - pomicanje ocista po x osi +
//				k - pomicanje ocista po x osi +
//              r - pocetni polozaj
//              esc izlaz iz programa
//*************************************************************************************************************

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <GL/glut.h>
#include <fstream>
#include <vector>
#include <GL/freeglut.h>
#include <cstdlib>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtx/string_cast.hpp>
#include <math.h>

using namespace std;
//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLuint width = 300, height = 300;
vector<glm::vec3> points;
vector<glm::vec4> rpoints;
vector<vector<int>> planes;
int kGlobal = 1;
double i2Global = 0;
glm::vec4 ociste = {0.0, 0.0, -15.0, 1};
glm::vec4 glediste = {0.0, 0.0, 0.0, 1};

GLuint texture[2];

glm::mat4 bSplineMat;
glm::mat4 bSplineMat2 = {
	(double) -1, (double) 3, (double) -3, (double) 1,
		(double) 3, (double) -6, (double) 0, (double) 4,
		(double) -3, (double) 3, (double) 3, (double) 1,
		(double) 1, 0, 0, 0
};
bool first = true;

struct Particle {
    glm::vec3 pos, vel;
    glm::vec4 color;
    float fade;
    float life;
    bool active;
};

float dt = 0.1f;
const unsigned int max_particles = 500;
Particle particles[max_particles];




//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
void myMouse		(int button, int state, int x, int y);
void myKeyboard		(unsigned char theKey, int mouseX, int mouseY);
void myObject		();
void redisplay_all	(void);
void MyFire();
int firstUnused();

//*************************
//**************************

void redisplay_all(void)
{
    glutSetWindow(window);
    myReshape (width, height);
    glutPostRedisplay();
}

//*********************************************************************************
//	Glavni program.
//*********************************************************************************

int main(int argc, char** argv)
{	

	glutInit(&argc, argv);
	// postavljanje dvostrukog spremnika za prikaz (zbog titranja)
	glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize (width, height); 
	glutInitWindowPosition (100, 100);

    gluLookAt(0.0f, 2.5f, 50.0f,0.0f,0.0f,0.0f,0.0f,1.0,0.0);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture); 

    for (int i = 0; i < max_particles; i++)
    {
        particles[i].life = 1.0;
        particles[i].vel.x = float((rand()%50)-25)*10;
        particles[i].vel.y = float((rand()%50)-25)*10;
        particles[i].vel.z = float((rand()%50)-25)*10;
        particles[i].fade = float(rand()%100)/1000.0f+0.003f;
        particles[i].color.x = 1.0f;
        particles[i].color.y = 1.0f;
        particles[i].color.z = 1.0f;
        particles[i].active = true;
        particles[i].pos.x = float((rand()%10))*10;
        particles[i].pos.y = float((rand()%10))*10;
        particles[i].pos.z = float((rand()%10))*10;
    }
    

	window = glutCreateWindow ("Tijelo");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	printf("Tipka: l - pomicanje ocista po x os +\n");
	printf("Tipka: k - pomicanje ocista po x os -\n");
	printf("Tipka: r - pocetno stanje\n");
	printf("esc: izlaz iz programa\n");
	float direction[]	  = {0.0f, 0.0f, 1.0f, 0.0f};
	float diffintensity[] = {0.7f, 0.7f, 0.7f, 1.0f};
	float ambient[]       = {0.2f, 0.2f, 0.2f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);	
	
	
	glEnable(GL_LIGHT0);

	redisplay_all();
	glutMainLoop();
	return 0;
}

//*********************************************************************************
//	Osvjezavanje prikaza. 
//*********************************************************************************

void myDisplay(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MyFire();
	glutSwapBuffers();      // iscrtavanje iz dvostrukog spemnika (umjesto glFlush)
}

void MyFire() {
   
   for (int i = 0; i < max_particles; i++)
   {

       Particle part = particles[i]; 
     
       if (part.active)
       {
           glColor4f(part.color.x, part.color.y, part.color.z, part.life);

            glBegin(GL_TRIANGLE_STRIP);
                glVertex3f(part.pos.x+0.5f, part.pos.y+0.5f, part.pos.z);
                glVertex3f(part.pos.x+0.5f, part.pos.y-0.5f, part.pos.z);
                glVertex3f(part.pos.x-0.5f, part.pos.y-0.5f, part.pos.z);
                glVertex3f(part.pos.x-0.5f, part.pos.y+0.5f, part.pos.z);
            glEnd();

            part.pos.x += part.vel.x/750;
            part.pos.y += part.vel.y/1000;
            part.pos.z += part.vel.z/750;
            part.pos.y += 0.8;

            part.life -= part.fade/2;

            if (part.life < 0.0f)
            {
                part.life = 1.0f;
                part.vel.x = float((rand()%50)-25)*10;
                part.vel.y = float((rand()%50)-25)*10;
                part.vel.z = float((rand()%50)-25)*10;
                part.fade = float(rand()%100)/1000.0f+0.003f;
                part.color.x = 1.0f;
                part.color.y = 1.0f;
                part.color.z = 1.0f;
                part.pos.x = float((rand()%10))*10;
                part.pos.y = float((rand()%10))*10;
                part.pos.z = float((rand()%10))*10;
                part.active = true;

            } else if (part.life < 0.5) {

                part.color.y -= 0.2;

            } else if (part.life < 0.75){
  
                part.color.y -= 0.2;
            } else if (part.life < 0.9){
            
                part.color.y -= 0.2;

            }
            particles[i] = part;
       }
       
   }
   glutPostRedisplay();
}

//*********************************************************************************
//	Promjena velicine prozora.
//	Funkcija gluPerspective i gluLookAt se obavlja 
//		transformacija pogleda i projekcija
//*********************************************************************************

void myReshape (int w, int h)
{
	width=w; height=h;
	glViewport (0, 0, width, height); 
	glMatrixMode (GL_PROJECTION);        // aktivirana matrica projekcije
	glLoadIdentity ();
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);		         // boja pozadine - bijela
	glClear ( GL_COLOR_BUFFER_BIT );				     // brisanje zaslona
	gluPerspective(45.0, (float)width/height, 0.5, 100.0); // kut pogleda, x/y, prednja i straznja ravnina odsjecanja
	gluLookAt (ociste.x, ociste.y, ociste.z, glediste.x, glediste.y, glediste.z, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
	glColor3ub (0, 0, 255);
	glMatrixMode (GL_MODELVIEW);         // aktivirana matrica modela
}

//*********************************************************************************
//	Crta moj objekt. Ovdje treba naciniti prikaz ucitanog objekta.
//*********************************************************************************








//*********************************************************************************
//	Mis.
//*********************************************************************************

void myMouse(int button, int state, int x, int y)
{
	//	Desna tipka - brise canvas. 
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		ociste.x=0;
		redisplay_all();
	}
}

//*********************************************************************************
//	Tastatura tipke - esc - izlazi iz programa.
//*********************************************************************************

void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
	switch (theKey) 
	{
		case 'l': ociste[0] = ociste[0]+0.1;
        break;

		case 'j': ociste[0] =ociste[0]-0.1;
        break;

        case 'i': ociste[1] = ociste[1]+0.1;
        break;

        case 'k': ociste[1] = ociste[1]-0.1;
        break;

        case 'u': ociste[2] = ociste[2]+0.1;
        break;

        case 'o': ociste[2] = ociste[2]-0.1;
        break;

		case 'a': glediste[0] = glediste[0]+0.1;
        break;

		case 'd': glediste[0] =glediste[0]-0.1;
        break;

        case 'w': glediste[1] = glediste[1]+0.1;
        break;

        case 's': glediste[1] = glediste[1]-0.1;
        break;

        case 'q': glediste[2] = glediste[2]+0.1;
        break;

        case 'e': glediste[2] = glediste[2]-0.1;
        break;

		case 'r': ociste.x=0.0;
        break;

		case 27:  exit(0);
		break;
	}
    redisplay_all();
}
