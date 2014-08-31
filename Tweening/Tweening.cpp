
#define GLUT_DISABLE_ATEXIT_HACK
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl\glut.h>

float ratio,time=0.0;
float hawk[16][3]= {{25.0,0.0,0.0},
            {30.0,10.0,0.0},
            {10.0,10.0,0.0},
            {0.0,30.0,0.0},
            {30.0,30.0,0.0},
            {30.0,48.0,0.0},
            {35.0,45.0,0.0},
            {46.0,42.0,0.0},
            {46.0,38.0,0.0},
            {40.0,32.0,0.0},
            {40.0,30.0,0.0},
            {70.0,30.0,0.0},
            {60.0,10.0,0.0},
            {40.0,10.0,0.0},
            {45.0,0.0,0.0},
            {25.0,0.0,0.0}};

float plane[16][3]={{20.0,0.0,0.0},
            {30.0,20.0,0.0},
            {0.0,20.0,0.0},
            {5.0,30.0,0.0},
            {35.0,30.0,0.0},
            {30.0,48.0,0.0},
            {35.0,48.0,0.0},
            {45.0,30.0,0.0},
            {50.0,30.0,0.0},
            {55.0,48.0,0.0},
            {70.0,48.0,0.0},
            {65.0,30.0,0.0},
            {60.0,20.0,0.0},
            {45.0,20.0,0.0},
            {30.0,0.0,0.0},
            {20.0,0.0,0.0}};
float temp[16][3];
void tween(float t, float arr1[][3], float arr2[][3])
{
    for(int i=0;i<16;i++)
    {
        temp[i][0]=arr1[i][0]+(arr2[i][0]-arr1[i][0])*t;
        temp[i][1]=arr1[i][1]+(arr2[i][1]-arr1[i][1])*t;
        temp[i][2]=0.0;
    }
}

void init()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.0,0.0,0.5);

    glEnable(GL_DEPTH_TEST);
    glEnable ( GL_COLOR_MATERIAL );
    glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
}

void reshape(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    ratio = 1.0f * w / h;
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the clipping volume

    gluPerspective(120,ratio,1,500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5000,
              0.0,0.0,0.0,
              0.0f,1.0f,0.0f);
}

void display(void)
 
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
      glTranslatef(0.0,0.0,-4.0);
      glScalef(.1,.1,.1);

      glColor3f(1.0,1.0,1.0);
      glTranslatef(20.0,-30.0,0.0);
      tween(time,plane,hawk);
      glBegin(GL_LINE_LOOP);
        for(int i=0; i<16;i++)
        {
            glVertex3f(temp[i][0],temp[i][1],temp[i][2]);
        }
      glEnd();

    
 glPopMatrix();
 glFlush();
 glutSwapBuffers();
}



void keyboard (unsigned char key, int x, int y)
{
    switch (key) {
       case 'w':
           if(time<1.0)
           time+=0.1;
           break;
       case 's':
          if(time>0.0)
           time-=0.1;
          break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

void main(int argc, char** argv)
{
    glutInit            ( &argc, argv ); // Erm Just Write It =)
    glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA ); // Display Mode
    glutInitWindowPosition (0,0);
    glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
    glutCreateWindow    ( "Tweening Example" ); // Window Title (argv[0] for current directory as title)
    init ();
    glutFullScreen      ( );          // Put Into Full Screen
    glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
    glutReshapeFunc     ( reshape );
    glutKeyboardFunc    ( keyboard );
    // glutSpecialFunc     ( arrow_keys );
    glutIdleFunc			 ( display );
    glutMainLoop        ( );          // Initialize The Main Loop
}

