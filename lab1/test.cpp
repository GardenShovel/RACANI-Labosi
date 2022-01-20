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
glm::vec4 ociste = {0.0, 0.0, -5.0, 1};
glm::vec4 glediste = {0.0, 0.0, 0.0, 1};

glm::mat4 bSplineMat;
glm::mat4 bSplineMat2 = {
	(double) -1, (double) 3, (double) -3, (double) 1,
		(double) 3, (double) -6, (double) 0, (double) 4,
		(double) -3, (double) 3, (double) 3, (double) 1,
		(double) 1, 0, 0, 0
};
bool first = true;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
void myMouse		(int button, int state, int x, int y);
void myKeyboard		(unsigned char theKey, int mouseX, int mouseY);
void myObject		();
void redisplay_all	(void);

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

    ifstream file("model.txt");
    string line;
    while(getline (file, line))
    {
        if (line[0] == 'v')
        {
            string delimiter = " ";
            vector<GLdouble> values;
            size_t pos = 0;
            while ((pos = line.find(delimiter)) != string::npos) {
                string token = line.substr(0, pos);
                if (token != "v")  {
                    values.push_back(stod(token));
                }
                line.erase(0, pos + delimiter.length());
            }
            values.push_back(stod(line));
            
            points.push_back(glm::vec3(values[0], values[1], values[2]));
        }
    }
    file.close();
    
    ifstream file2("model.txt");

    while (getline(file2, line)) {
        if (line[0] == 'f')
        {
            string delimiter = " ";
            vector<int> values;
            size_t pos = 0;
            while ((pos = line.find(delimiter)) != string::npos) {
                string token = line.substr(0, pos);
                if (token != "f")  {
                    values.push_back(stoi(token)-1);
                }
                line.erase(0, pos + delimiter.length());
            }
            values.push_back(stoi(line)-1);
            planes.push_back(values);
        }
    }
    
    

    file2.close();

	ifstream file3("rpoints.txt");

	while (getline(file3, line))
	{
		string delimiter = " ";
        vector<int> values;
        size_t pos = 0;
		while ((pos = line.find(delimiter))!= string::npos)
		{
			
			values.push_back(stod(line.substr(0, pos)));
			line.erase(0, pos + delimiter.length());
		}	
		values.push_back(stod(line));
        rpoints.push_back(glm::vec4(values[0], values[1], values[2], 1));

	}


	file3.close();
	
	bSplineMat = {
		(double) -1/6, (double) 3/6, (double) -3/6, (double) 1/6,
		(double) 3/6, (double) -6/6, (double) 0/6, (double) 4/6,
		(double) -3/6, (double) 3/6, (double) 3/6, (double) 1/6,
		(double) 1/6, 0, 0, 0
		};
	
	

	glutInit(&argc, argv);
	// postavljanje dvostrukog spremnika za prikaz (zbog titranja)
	glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize (width, height); 
	glutInitWindowPosition (100, 100);
   
	window = glutCreateWindow ("Tijelo");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	printf("Tipka: l - pomicanje ocista po x os +\n");
	printf("Tipka: k - pomicanje ocista po x os -\n");
	printf("Tipka: r - pocetno stanje\n");
	printf("esc: izlaz iz programa\n");
	
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
	myObject ();
	glutSwapBuffers();      // iscrtavanje iz dvostrukog spemnika (umjesto glFlush)
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
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f);		         // boja pozadine - bijela
	glClear ( GL_COLOR_BUFFER_BIT );				     // brisanje zaslona
	gluPerspective(45.0, (float)width/height, 0.5, 100.0); // kut pogleda, x/y, prednja i straznja ravnina odsjecanja
	gluLookAt (ociste.x, ociste.y, ociste.z, glediste.x, glediste.y, glediste.z, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
	glColor3ub (0, 0, 255);
	glMatrixMode (GL_MODELVIEW);         // aktivirana matrica modela
}

//*********************************************************************************
//	Crta moj objekt. Ovdje treba naciniti prikaz ucitanog objekta.
//*********************************************************************************



glm::mat4 rMat(glm::vec4 r_1, glm::vec4 r0, glm::vec4 r1, glm::vec4 r2)
{
	glm::mat4 rMat = {
					r_1.x, r0.x, r1.x, r2.x,
					r_1.y, r0.y, r1.y, r2.y,
					r_1.z, r0.z, r1.z, r2.z,
					r_1.w, r0.w, r1.w, r2.w};
	return rMat;
}

glm::vec4 bspline(double t, glm::vec4 r_1, glm::vec4 r0, glm::vec4 r1, glm::vec4 r2)
{
	glm::vec4 tVec = {pow(t, 3), pow(t, 2), pow(t, 1), 1};
	
	
	return tVec * bSplineMat * rMat(r_1, r0, r1, r2);

}

void myObject ()
{
//	glutWireCube (1.0);
//  glutSolidCube (1.0);
//	glutWireTeapot (1.0);
//  glutSolidTeapot (1.0);
	
	
	
	if (kGlobal > rpoints.size()-2)
	{
		cout << "first if" << endl;
		kGlobal = 1;
		i2Global = 0;
	}
	if (i2Global >= 1)
	{
		cout << "second if" << endl;
		kGlobal++;
		i2Global = 0;
	}
	
	glm::vec4 p = bspline(i2Global, rpoints[kGlobal-1], rpoints[kGlobal], rpoints[kGlobal+1], rpoints[kGlobal+2]);
	glm::vec4 s = {0, 0, 1, 1};

	glm::vec4 otherT = {3 * i2Global * i2Global, 2*i2Global, 1, 0};
	glm::vec4 otherP = otherT * bSplineMat2 * rMat(rpoints[kGlobal-1], rpoints[kGlobal], rpoints[kGlobal+1], rpoints[kGlobal+2]);

	double angle = acos(glm::dot(s, otherP) / (glm::length(s) * glm::length(otherP))) *  (180.0/3.141592653589793238463);
	glPushMatrix();
	glTranslated(p.x, p.y, p.z);
	glRotated(angle, s.y * otherP.z - otherP.y * s.z, -(s.x * otherP.z - otherP.x * s.z), s.x * otherP.y - otherP.x * s.y);
	glBegin(GL_TRIANGLES);
	
	for (int i = 0; i < planes.size(); i++)
	{
		glVertex3f(points[planes[i][0]].x, points[planes[i][0]].y, points[planes[i][0]].z);
		glVertex3f(points[planes[i][1]].x, points[planes[i][1]].y, points[planes[i][1]].z);
		glVertex3f(points[planes[i][2]].x, points[planes[i][2]].y, points[planes[i][2]].z);
		
	}
	
	glEnd();
	
	glPopMatrix();

	glBegin(GL_LINES);
	glVertex3d(p.x, p.y, p.z);
	glVertex3d(otherP.x, otherP.y, otherP.z);
	glEnd();
	
	

	glBegin (GL_LINE_STRIP); // ili glBegin (GL_LINE_LOOP); za zicnu formu
		glColor3ub (0, 0, 255);
		for (int k = 1; k < rpoints.size()-2; k++)
		{
			double i = 0;
			while (i < 1){
				glm::vec4 p = bspline(i, rpoints[k-1], rpoints[k], rpoints[k+1], rpoints[k+2]);
				
				
				glVertex3f(p.x, p.y, p.z);
				i += 0.01;
			}
		}
		
	
	glEnd();

	cout << angle << endl;
	
	
	i2Global += 0.01;
	glutPostRedisplay();
	
}

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
