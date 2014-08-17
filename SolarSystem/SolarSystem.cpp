
#define GLUT_DISABLE_ATEXIT_HACK
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <GL/glut.h>

#include "math.h"

static float year = 0, day = 0 ,mercuryRot=0.0,venusRot=0.0,earthRot=0.0,marsRot=0.0,jupitorRot=0.0,saturnRot=0.0,neptuneRot=0.0,uranusRot=0.0,plutoRot=0.0;
static float rotateScene=0.0,rotateAstroid=0.0;
static int flag=0;

GLUquadricObj * sun = NULL;
GLUquadricObj * mercury = NULL;
GLUquadricObj * venus = NULL;
GLUquadricObj * earth = NULL;
GLUquadricObj * mars = NULL;
GLUquadricObj * jupiter = NULL;
GLUquadricObj * saturn = NULL;
GLUquadricObj * uranus = NULL;
GLUquadricObj * pluto = NULL;
GLUquadricObj * neptune = NULL;

GLuint texture[10];
GLuint skyTex[6];

char *texSun="sun.JPG",
	*texMercury="mercury.JPG",
	*texVenus="venus.JPG",
	*texEarth="earth.JPG",
	*texMars="mars.JPG",
	*texJupiter="jupiter.JPG",
	*texSaturn="saturn.JPG",
	*texUranus="uranus.JPG",
	*texNeptune="neptune.JPG",
	*texPluto="pluto.JPG";

char	*texTop ="top.JPG",
		*texBottom= "bottom.JPG",
		*texLeft= "left.JPG",
		*texRight = "right.JPG",
		*texFront ="front.JPG",
		*texBack ="back.JPG";


// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx=0.0f,lz=-1.0f;
// XZ position of the camera
float x=0.0f, z=5.0f;
// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;


GLuint LoadTexture(char *TexName)
{

   GLuint Texture;
 
   IplImage *img = cvLoadImage(TexName);
   if(!img)
   {
	   std::cout<<"\n cannot load image !!!!";
   }
 
   glGenTextures(1,&Texture);            // Allocate space for texture
   glBindTexture(GL_TEXTURE_2D,Texture); // Set our Tex handle as current
 
   // Create the texture
  //  if(Img.GetBPP()==24)
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,img->width,img->height,0,
	   GL_RGB,GL_UNSIGNED_BYTE,img->imageData);
    /*else if(Img.GetBPP()==32)
     glTexImage2D(GL_TEXTURE_2D,0,4,Img.GetWidth(),Img.GetHeight(),0,
                  GL_RGBA,GL_UNSIGNED_BYTE,Img.GetImg());
    else
     return -1;
 */

   // Specify filtering and edge actions
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
   glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   return Texture;
}

void init(void) 
{
	
   GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat sunPositionR[] = { 0.0, 0.0, 2.0, 1.0 }; //w value is nonzero, the light is positional
   GLfloat sunPositionL[] = { -1.7, 0.0, 0.0, 1.0 };
   GLfloat sunPositionB[] = { 0.0, 0.0, -1.7, 1.0 };
   GLfloat sunPositionF[] = { 0.0, 0.0, 1.7, 1.0 };
   GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
      GLfloat local_view[] = { 0.0 };
  
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   glLightfv(GL_LIGHT0,GL_DIFFUSE, specular);
      glLightfv(GL_LIGHT0,GL_SPECULAR, specular);
   glLightfv(GL_LIGHT0,GL_POSITION, sunPositionR);
  // glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, .4);
//	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.04);

   /*glLightfv(GL_LIGHT1,GL_DIFFUSE, specular);
      glLightfv(GL_LIGHT1,GL_SPECULAR, specular);
   glLightfv(GL_LIGHT1,GL_POSITION, sunPositionL);
   glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, .2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, .3);
 /*  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 2.0);
glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 1.0);
glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.5);*/

   /*glLightfv(GL_LIGHT2,GL_DIFFUSE, specular);
      glLightfv(GL_LIGHT2,GL_SPECULAR, specular);
   glLightfv(GL_LIGHT2,GL_POSITION, sunPositionB);
   glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, .2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, .3);
  /* glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 2.0);
glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 1.0);
glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.5);*/

    /*glLightfv(GL_LIGHT3,GL_DIFFUSE, specular);
      glLightfv(GL_LIGHT3,GL_SPECULAR, specular);
   glLightfv(GL_LIGHT3,GL_POSITION, sunPositionF);
   glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, .2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, .3);
   /*glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 2.0);
glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 1.0);
glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.5);*/

//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);


  // glEnable(GL_LIGHT0);
  // glEnable(GL_LIGHT0);
 //  glEnable(GL_LIGHT2);
  // glEnable(GL_LIGHT3);
   glEnable(GL_DEPTH_TEST);
   //glEnable(GL_LIGHTING);
   texture[0]=LoadTexture(texSun);
   texture[1]=LoadTexture(texMercury);
   texture[2]=LoadTexture(texVenus);
   texture[3]=LoadTexture(texEarth);
   texture[4]=LoadTexture(texMars);
   texture[5]=LoadTexture(texJupiter);
   texture[6]=LoadTexture(texSaturn);
   texture[7]=LoadTexture(texUranus);
   texture[8]=LoadTexture(texNeptune);
   texture[9]=LoadTexture(texPluto);
	
   skyTex[0] = LoadTexture(texBottom);
   skyTex[1] = LoadTexture(texTop);
   skyTex[2] = LoadTexture(texRight);
   skyTex[3] = LoadTexture(texLeft);
   skyTex[4] = LoadTexture(texFront);
   skyTex[5] = LoadTexture(texBack);


   glEnable(GL_TEXTURE_2D);

	sun = gluNewQuadric( );
    gluQuadricNormals( sun, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( sun, GL_TRUE );

	mercury = gluNewQuadric( );
    gluQuadricNormals( mercury, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( mercury, GL_TRUE );

	venus = gluNewQuadric( );
    gluQuadricNormals( venus, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( venus, GL_TRUE );
	
	earth = gluNewQuadric( );
    gluQuadricNormals( earth, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( earth, GL_TRUE );

	mars = gluNewQuadric( );
    gluQuadricNormals( mars, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( mars, GL_TRUE );

	jupiter = gluNewQuadric( );
    gluQuadricNormals( jupiter, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( jupiter, GL_TRUE );

	saturn = gluNewQuadric( );
    gluQuadricNormals( saturn, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( saturn, GL_TRUE );

	uranus = gluNewQuadric( );
    gluQuadricNormals( uranus, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( uranus, GL_TRUE );

	neptune = gluNewQuadric( );
    gluQuadricNormals( neptune, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( neptune, GL_TRUE );

	pluto = gluNewQuadric( );
    gluQuadricNormals( pluto, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( pluto, GL_TRUE );

}
void computePos(float deltaMove) {

	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void computeDir(float deltaAngle) {

	angle += deltaAngle;
	lx = sin(angle);
	lz = -cos(angle);
}

void circle(float x,float y,float z, float r)
{
	float theta=0.0;
	glBegin(GL_POINTS);
		for(theta=0.0;theta<360;theta+=0.5)
		{
			x=r*cos(theta);
			z=r*sin(theta);
			glVertex3f((GLfloat)x,(GLfloat)y,(GLfloat)z);
		}
	glEnd();

}
void circle1(float x,float y,float z, float r,float theta)
{
	glBegin(GL_POINTS);
		for(theta=0.0;theta<360;theta=theta+2.0)
		{
			x=r*cos(theta);
			z=r*sin(theta);
			glVertex3f((GLfloat)x,(GLfloat)y,(GLfloat)z);
		}
	glEnd();

}
void makeAstroidBelt()
{
	float r;
	glColor3f(0.5,0.5,0.5);
	for(r=8.80;r<9.20;r+=0.10)
	{
		glRotatef(GLfloat(r-5.0),0.0,1.0,0.0);
		circle1(0.0,0.0,0.0,r,2.0);
	}

}


void displayPath()
{
	float r=0.0;
	for(r=2.0;r<=18;r+=2.0)
	circle(0.0,0.0,0.0,r);
}

void skyBox()
{
	//creating Skybox
	 glBindTexture(GL_TEXTURE_2D, skyTex[4]);
	 glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	float size = 100.0;
	 glBegin ( GL_QUADS );
   	   // Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size,  size);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size,  size);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size,  size);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size,  size);
     glEnd();

		// Back Face
	glBindTexture(GL_TEXTURE_2D, skyTex[5]);

	glBegin ( GL_QUADS );
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, -size);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-size,  size, -size);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( size,  size, -size);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( size, -size, -size);
	glEnd();

	// Top Face

	glBindTexture(GL_TEXTURE_2D, skyTex[1]);

	glBegin ( GL_QUADS );
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size, -size);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-size,  size,  size);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( size,  size,  size);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size, -size);
	glEnd();
   // Bottom Face

	glBindTexture ( GL_TEXTURE_2D, skyTex[0] );
   
	glBegin ( GL_QUADS );
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, -size, -size);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( size, -size, -size);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( size, -size,  size);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size,  size);
	glEnd();

    // Right face
	glBindTexture ( GL_TEXTURE_2D, skyTex[2] );
	
	glBegin ( GL_QUADS );
		glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size, -size);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size, -size);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( size,  size,  size);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( size, -size,  size);
	glEnd();
	
	// Left Face
	glBindTexture ( GL_TEXTURE_2D, skyTex[3] );
	glBegin ( GL_QUADS );
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size,  size);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-size,  size,  size);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size, -size);
	glEnd();

}
void display(void)
{

   GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat sun_ambient[] = { 1.0, 1.0, 0.0, 1.0 };
   GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
   GLfloat sun_diffuse[] = { 0.6, 0.6, 0.0, 1.0 };
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat no_shininess[] = { 0.0 };
   GLfloat low_shininess[] = { 5.0 };
   GLfloat sun_shininess[] = { 70.0 };
   GLfloat sun_emission[] = {0.3, 0.3, 0.0, 0.0};

	GLfloat mercury_ambient[] = { 0.6, 0.1, 0.0, 1.0 };
	GLfloat mercury_diffuse[] = { 0.6, 0.1, 0.0, 1.0 };
	GLfloat mercury_shininess[] = { 30.0 };
    GLfloat mercury_emission[] = {0.2, 0.2, 0.0, 0.0};

	GLfloat venus_ambient[] = { 0.6, 0.3, 0.0, 1.0 };
	GLfloat venus_diffuse[] = { 0.6, 0.3, 0.0, 1.0 };
	GLfloat venus_shininess[] = { 30.0 };
    GLfloat venus_emission[] = {0.2, 0.3, 0.0, 0.0};

	GLfloat earth_ambient[] = { 0.2, 0.2, 0.55, 1.0 };
	GLfloat earth_diffuse[] = { 0.2, 0.2, 0.55, 1.0 };
	GLfloat earth_shininess[] = { 40.0 };
    GLfloat earth_emission[] = {0.1, 0.1, 0.45, 0.0};

	GLfloat mars_ambient[] = { 0.75, 0.2, 0.02, 1.0 };
	GLfloat mars_diffuse[] = { 0.75, 0.2, 0.02, 1.0 };
	GLfloat mars_shininess[] = { 30.0 };
    GLfloat mars_emission[] = {0.3, 0.2, 0.0, 0.0};

	GLfloat jupitor_ambient[] = { 0.9, 0.7, 0.3, 1.0 };
	GLfloat jupitor_diffuse[] = { 0.9, 0.7, 0.3, 1.0 };
	GLfloat jupitor_shininess[] = { 20.0 };
	GLfloat jupitor_emission[] = {0.1, 0.1, 0.0, 0.0};

	GLfloat saturn_ambient[] = { 0.6, 0.6, 0.4, 1.0 };
	GLfloat saturn_diffuse[] = { 0.6, 0.6, 0.4, 1.0 };
	GLfloat saturn_shininess[] = { 20.0 };
	GLfloat saturn_emission[] = {0.2, 0.2, 0.0, 0.0};

	GLfloat uranus_ambient[] = { 0.4, 0.6, 0.8, 1.0 };
	GLfloat uranus_diffuse[] = { 0.4, 0.6, 0.8, 1.0 };
	GLfloat uranus_shininess[] = { 20.0 };
	GLfloat uranus_emission[] = {0.1, 0.2, 0.5, 0.0};

	GLfloat neptune_ambient[] = { 0.4, 0.0, 0.6, 1.0 };
	GLfloat neptune_diffuse[] = { 0.4, 0.0, 0.6, 1.0 };
	GLfloat neptune_shininess[] = { 20.0 };
	GLfloat neptune_emission[] = {0.1, 0.1, 0.2, 0.0};

	GLfloat pluto_ambient[] = { 0.4, 0.1, 0.7, 1.0 };
	GLfloat pluto_diffuse[] = { 0.4, 0.1, 0.7, 1.0 };
	GLfloat pluto_shininess[] = { 20.0 };
	GLfloat pluto_emission[] = {0.0, 0.2, 0.3, 0.0};

	
	if (deltaMove)
		computePos(deltaMove);
	if (deltaAngle)
		computeDir(deltaAngle);

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glColor3f (1.0, 1.0, 1.0);

   //glRotatef(5.0,1.0,0.0,0.0);

   glPushMatrix();
    gluLookAt(	x, 1.0f, z,
				x+lx, 1.0f,  z+lz,
				0.0f, 1.0f,  0.0f);

 	glRotatef(rotateScene,1.0,0.0,0.0);
	if(flag==1)
	displayPath();
	

	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, sun_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_diffuse);
		//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, sun_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, sun_emission);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		gluSphere(sun,1.5,20,20);
		//glutSolidSphere(1.5, 20, 20);   /* draw sun */
	glPopMatrix();

	glPushMatrix();
		glRotatef ((GLfloat) mercuryRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, mercury_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mercury_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS, mercury_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, mercury_emission);
			glTranslatef (2.0, 0.0, 0.0);
			 glBindTexture(GL_TEXTURE_2D, texture[1]);
			 gluSphere(mercury,.2,20,18);
		glPopMatrix();
		if(mercuryRot>=360.0)
			mercuryRot=0.0;
	mercuryRot=mercuryRot+1.0 ;
	glPopMatrix();

	glPushMatrix();
		glRotatef ((GLfloat) venusRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, venus_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, venus_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS, venus_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, venus_emission);
			glTranslatef (4.0, 0.0, 0.0);
			 glBindTexture(GL_TEXTURE_2D, texture[2]);
			 gluSphere(venus,.4,20,18);
		glPopMatrix();
			venusRot=venusRot+.800;
			if(venusRot>=360.0)
			venusRot=0.0;
	glPopMatrix();

	glPushMatrix();
		glRotatef(rotateAstroid,0.0,1.0,0.0);
		makeAstroidBelt();
		rotateAstroid+=0.3;
	glPopMatrix();


	glPushMatrix();
		glRotatef ((GLfloat) earthRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, earth_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS,earth_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, earth_emission);
			glTranslatef (6.0, 0.0, 0.0);
			glBindTexture(GL_TEXTURE_2D, texture[3]); //earth
			 gluSphere(earth,.5,20,18);
			earthRot=earthRot+.5;
			if(earthRot>=360.0)
			earthRot=0.0;
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		glRotatef ((GLfloat) marsRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, mars_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mars_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS,mars_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, mars_emission);
			glTranslatef (8.0, 0.0, 0.0);
			//glutSolidSphere(0.65, 20, 18); //mars
			glBindTexture(GL_TEXTURE_2D, texture[4]);
			 gluSphere(mars,.65,20,18);
			marsRot=marsRot+0.53;
				if(marsRot>=360.0)
			marsRot=0.0;
		glPopMatrix();		
	glPopMatrix();



	glPushMatrix();
		glRotatef ((GLfloat) jupitorRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
		glRotatef (90.0,1.0,0.0,0.0);
			glMaterialfv(GL_FRONT, GL_AMBIENT, jupitor_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, jupitor_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS,jupitor_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, jupitor_emission);
			glTranslatef (10.0, 0.0, 0.0);
			//glutSolidSphere(0.8, 20, 18);  //jupiter
			glBindTexture(GL_TEXTURE_2D, texture[5]);
			 gluSphere(jupiter,.9,20,18);
			jupitorRot=jupitorRot+0.084;
				if(jupitorRot>=360.0)
			jupitorRot=0.0;
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		glRotatef ((GLfloat) saturnRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, saturn_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, saturn_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS,saturn_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, saturn_emission);
			glTranslatef (12.0, 0.0, 0.0);
			circle(0.0,0.0,0.0,0.9);
			circle(0.0,0.0,0.0,0.85);
			circle(0.0,0.0,0.0,0.95);
			circle(0.0,0.0,0.0,1.0);
			circle(0.0,0.0,0.0,1.05);
			circle(0.0,0.0,0.0,1.10);
			circle(0.0,0.0,0.0,1.15);
			//glutSolidSphere(0.7, 20, 18);  //saturn
			glBindTexture(GL_TEXTURE_2D, texture[6]);
			gluSphere(saturn,.7,20,18);
			saturnRot=	saturnRot+	0.034;
			if(saturnRot>=360.0)
			saturnRot=0.0;
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		glRotatef ((GLfloat) uranusRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, uranus_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, uranus_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS,uranus_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, uranus_emission);
			glTranslatef (14.0, 0.0, 0.0);
			glBindTexture(GL_TEXTURE_2D, texture[7]);
			gluSphere(uranus,.6,20,18);
			uranusRot=uranusRot+0.012;
				if(uranusRot>=360.0)
			uranusRot=0.0;
		glPopMatrix();		
	glPopMatrix();
	
	glPushMatrix();
		glRotatef ((GLfloat) neptuneRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, neptune_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, neptune_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS,neptune_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, neptune_emission);
			glTranslatef (16.0, 0.0, 0.0);
		//	glutSolidSphere(0.3, 20, 18); //neptune
			glBindTexture(GL_TEXTURE_2D, texture[8]);
			gluSphere(neptune,.3,20,18);
			neptuneRot=neptuneRot+0.007;
				if(neptuneRot>=360.0)
			neptuneRot=0.0;
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		glRotatef ((GLfloat) plutoRot, 0.0, 1.0, 0.0);
		glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, pluto_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, pluto_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS,pluto_shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, pluto_emission);
			glTranslatef (18.0, 0.0, 0.0);
			//glutSolidSphere(0.3, 20, 18);  //pluto
			glBindTexture(GL_TEXTURE_2D, texture[9]);
			gluSphere(pluto,.3,20,18);
			plutoRot=plutoRot+0.004;
				if(plutoRot>=360.0)
			plutoRot=0.0;
		glPopMatrix();
	glPopMatrix();
	
   glFlush();
   glPopMatrix();
   skyBox();
   glutSwapBuffers();
}
void pressKey(int key, int xx, int yy) {

	switch (key) {
		case GLUT_KEY_LEFT : deltaAngle = -0.01f; break;
		case GLUT_KEY_RIGHT : deltaAngle = 0.01f; break;
		case GLUT_KEY_UP : deltaMove = 0.5f; break;
		case GLUT_KEY_DOWN : deltaMove = -0.5f; break;
	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT :
		case GLUT_KEY_RIGHT : deltaAngle = 0.0f;break;
		case GLUT_KEY_UP :
		case GLUT_KEY_DOWN : deltaMove = 0;break;
	}
}
void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(70.0, (GLfloat) w/(GLfloat) h, 1.0, 600.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (0.0, 0.0, 35.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 'w':
         rotateScene = rotateScene + 1.0;
		 if(rotateScene>=360)
			 rotateScene=0.0;
		  glutPostRedisplay();
         break;
      case 's':
         rotateScene = rotateScene - 1.0;
		 if(rotateScene<=-360)
			 rotateScene=0.0;
		 glutPostRedisplay();
         break;
      case 'p':
         flag=1;
         glutPostRedisplay();
         break;
      case 'P':
			flag=0;         
		  glutPostRedisplay();
         break;
	  case 'd':
		break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   glutFullScreen();
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutSpecialFunc(pressKey);

	// here are the new entries
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(display);
   glutMainLoop();
   return 0;
}
