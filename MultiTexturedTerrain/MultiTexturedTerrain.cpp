#define GLUT_DISABLE_ATEXIT_HACK
#include <cstdlib>
#include <gl/glew.h>
#include <GL/glut.h>
#include "abShaders.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define gridLength 100	
#define gridWidth 100
#define interval 1.0

// stuff for lighting
GLfloat lAmbient[] = {0.3,0.3,0.3,1.0};
GLfloat lDiffuse[] = {1.0,1.0,1.0,1.0};
GLfloat lSpecular[] = {1.0,1.0,1.0,1.0};

GLfloat lPosition[] = {0.0,-50.0,50.0,1.0};

// materials
GLfloat mSpecular[] = {1.0,1.0,1.0,1.0};
// the smaller the larger the specular area is
GLfloat mShininess[] = {128.0};

//colors
GLfloat cBlack[] = {0.0,0.0,0.0,1.0};
GLfloat cOrange[] = {1.0,0.5,0.5,1.0}; 
GLfloat cWhite[] = {1.0,1.0,1.0,1.0}; 
GLfloat cGrey[] = {0.1,0.1,0.1,1.0};
GLfloat cLightGrey[] = {0.9,0.9,0.9,1.0};

//for normal calculations
static int terrainGridWidth=gridWidth ,terrainGridLength=gridLength;
static float *terrainNormals = NULL;

GLfloat z=0.0,rotatex=0.0,rotatez=0.0;
int iterations=0;
GLfloat terrainData[int((float(gridLength)*float(gridWidth))/(interval*interval))][3];
int length=0,width=0;
GLfloat maxHeight=0.0;
GLfloat minHeight=0.0;

char *grass = "assets\\grass.jpg";
char *ice ="assets\\ice.jpg";
char *sand = "assets\\sand.jpg";
char *rock = "assets\\rock1.jpg";

GLuint textures[4];
abShader multiTex("shaders\\multiTex.vert","shaders\\multiTex.frag");

GLuint loadTexture(char *TexName)
{
  // TGAImg Img ;        // Image loader
   GLuint Texture;
 
 // Load our Texture
   //if(Img.Load(TexName)!=IMG_OK)
    //return -1;
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
       GL_BGR_EXT,GL_UNSIGNED_BYTE,img->imageData);
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
 
   return Texture;
}
float *terrainCrossProduct(int x1,int y1,int x2,int y2,int x3,int y3) {

    float *auxNormal,v1[3],v2[3];
        
    v1[0] = x2-x1; 
    v1[1] = y2-y1;
    v1[2] = -terrainData[y1 * width + x1][2] 
            + terrainData[y2 * width + x2][2];
     


    v2[0] = x3-x1; 
    v2[1] = y3-y1;
    v2[2] = -terrainData[y1 * width + x1][2] 
            + terrainData[y3 * width + x3][2];
     

    auxNormal = (float *)malloc(sizeof(float)*3);

    auxNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];
    auxNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
    auxNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];
    return(auxNormal);
}

void terrainNormalize(float *v) {

    double d,x,y,z;

    x = v[0];
    y = v[1];
    z = v[2];
    
    d = sqrt((x*x) + (y*y) + (z*z));

    v[0] = x / d;
    v[1] = y / d;
    v[2] = z / d;
}

void terrainAddVector(float *a, float *b) {

    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
}

void terrainComputeNormals() {

    float *norm1,*norm2,*norm3,*norm4; 
    int i,j,k;
    
    if (terrainNormals == NULL)
        return;

    for(i = 0; i < length; i++)
        for(j = 0; j < width; j++)
        {
            norm1 = NULL;
            norm2 = NULL;
            norm3 = NULL;
            norm4 = NULL;

            // normals for the four corners
            if (i == 0 && j == 0) {
                norm1 = terrainCrossProduct(0,0, 0,1, 1,0);	
                terrainNormalize(norm1);				
            }
            else if (j == width-1 && i == length-1) {
                norm1 = terrainCrossProduct(i,j, j,i-1, j-1,i);	
                terrainNormalize(norm1);				
            }
            else if (j == 0 && i == length-1) {
                norm1 = terrainCrossProduct(i,j, j,i-1, j+1,i);	
                terrainNormalize(norm1);				
            }
            else if (j == width-1 && i == 0) {
                norm1 = terrainCrossProduct(i,j, j,i+1, j-1,i);	
                terrainNormalize(norm1);				
            }

            // normals for the borders
            else if (i == 0) {
                norm1 = terrainCrossProduct(j,0, j-1,0, j,1);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(j,0,j,1,j+1,0);
                terrainNormalize(norm2);
            }
            else if (j == 0) {
                norm1 = terrainCrossProduct(0,i, 1,i, 0,i-1);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(0,i, 0,i+1, 1,i);
                terrainNormalize(norm2);
            }
            else if (i == length-1) {
                norm1 = terrainCrossProduct(j,i, j,i-1, j+1,i);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(j,i, j+1,i, j,i-1);
                terrainNormalize(norm2);
            }
            else if (j == width-1) {
                norm1 = terrainCrossProduct(j,i, j,i-1, j-1,i);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(j,i, j-1,i, j,i+1);
                terrainNormalize(norm2);
            }

            // normals for the interior
            else {
                norm1 = terrainCrossProduct(j,i, j-1,i, j,i+1);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(j,i, j,i+1, j+1,i);
                terrainNormalize(norm2);
                norm3 = terrainCrossProduct(j,i, j+1,i, j,i-1);
                terrainNormalize(norm3);
                norm4 = terrainCrossProduct(j,i, j,i-1, j-1,i);
                terrainNormalize(norm4);
            }
            if (norm2 != NULL) {
                terrainAddVector(norm1,norm2);
                free(norm2);
            }
            if (norm3 != NULL) {
                terrainAddVector(norm1,norm3);
                free(norm3);
            }
            if (norm4 != NULL) {
                terrainAddVector(norm1,norm4);
                free(norm4);
            }
            terrainNormalize(norm1);
            norm1[2] = - norm1[2];
            for (k = 0; k< 3; k++) 
            {
                terrainNormals[3*(i*width + j) + k] = norm1[k];
                if((3*(i*width + j) + k) >= width *length*3)
                {
                    std::cout<<"\nfunc:terrainComputeNormals() This can cause a heap corruption !!!";
                    std::cout<<"\nvalue is: "<<3*(i*width + j) + k;
                }
            }
            free(norm1);
        }

}


void compute()
{
    length=float(gridLength)/interval;
    width=float(gridWidth)/interval;
}


void init(void)
{    
   /*GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_position[] = { 0.0, 10.0, 10.0, 1.0 };
   GLfloat light_diffuse[] = { 0.6, 0.7, .2, 1.0 };
   GLfloat light_ambient[] = { 0.4, 0.2, .4, 1.0 };
   GLfloat light_specular[] = { 0.5, 0.7, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 75.0 };

   GLfloat light1_position[] = {2.0, 2.0, 2.0, 0.0 };
*/
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);

 /* glLightfv(GL_LIGHT0,GL_AMBIENT,lAmbient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lDiffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,lSpecular);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);*/
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);


   long int temp=0;
   float x=-(float(gridLength)/2.0),y=-(float(gridWidth)/2.0),z=0.0;
   for(int i=0;i<length;i++)
   {
       for(int j=0;j<width;j++)
       {
          terrainData[temp][0]=x;
          terrainData[temp][1]=y;
          terrainData[temp][2]=z;
          temp++;
          y+=interval;
       }
       y=-(float(gridWidth)/2.0);
       x+=interval;
   }
    /*terrainNormals = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float) * 3);
    if (terrainNormals == NULL)
        printf("nautanki");

    terrainComputeNormals();
    */
    textures[0]=loadTexture(sand);
    textures[1]=loadTexture(grass);
    textures[2]=loadTexture(ice);
    textures[3]=loadTexture(rock);

    multiTex.abCreate();
    multiTex.abCompileAndLink();
}
float faultAlgo(float x,float y,float a, float b, float c)
{	
    float d=a*x + b*y - c;
    return d;
}
void display(void)
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /*glLightfv(GL_LIGHT0,GL_POSITION,lPosition);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS,mShininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT, cGrey);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cWhite);
    glColor3f(1.0,1.0,1.0);*/

    float v = rand();
    float a = sin(v);
    float b = cos(v);
    float d = sqrt(float(gridLength)*float(gridLength) + float(gridWidth)*float(gridWidth));		// rand() / RAND_MAX gives a random number between 0 and 1.
    float temp=float(rand()) / float(RAND_MAX);			// therefore c will be a random number between -d/2 and d/2
    float c = temp * d - d/2;
    float p=0.0;
    int temp1=0;
     float x=-(float(gridLength)/2.0),y=-(float(gridWidth)/2.0),z=0.0;
    glPushMatrix();
    glRotatef(rotatex,1.0,0.0,0.0);
    glRotatef(rotatez,0.0,0.0,1.0);
    if(iterations<1000)
    {
        for(x=-(float(gridLength)/2.0);x<(float(gridLength)/2.0);x+=interval)
        {
            for(float y=-(float(gridWidth)/2.0);y<(float(gridWidth)/2.0);y+=interval)
            {
                p=0.0;
                p=faultAlgo(x,y,a,b,c);
                if(p>=-1.57 && p<=1.57)
                {
                    terrainData[temp1][2]+=sin(p)/5; //for cos use cos function here
                    temp1++;
                    //break;
                }
                else if(p<-1.57)
                {
                    terrainData[temp1][2]-=0.4;
                    temp1++;
                    //break;
                }
                else if(p>1.57)
                {
                    terrainData[temp1][2]+=0.4;
                    temp1++;
                    //break;
                }
                else
                    break;

                if(maxHeight<terrainData[temp1][2])
                    maxHeight = terrainData[temp1][2];
                if(minHeight > terrainData[temp1][2])
                    minHeight = terrainData[temp1][2];
            }
        }
        iterations++;
        multiTex.abSetUniformVariable("maxHeight",maxHeight);
        multiTex.abSetUniformVariable("minHeight",minHeight);
        maxHeight=0.0;
        minHeight=0.0;
    }
//	compute();
//	terrainComputeNormals();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    multiTex.abSetUniformTexture("sand",0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,textures[1]);
    multiTex.abSetUniformTexture("grass",1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,textures[2]);
    multiTex.abSetUniformTexture("ice",2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,textures[3]);
    multiTex.abSetUniformTexture("rock",3);
    multiTex.abUseProgram();
    temp1=0;
    int temp2=0;
        for(int i=0; i<length-1;i++)
        {	
            glBegin(GL_TRIANGLE_STRIP);
            for(int j=0;j<width;j++)
            {
            
                //temp1++;
        /*		glNormal3f(terrainNormals[3*(i*terrainGridWidth + j)],
                          terrainNormals[3*(i*terrainGridWidth + j)+1],
                          terrainNormals[3*(i*terrainGridWidth + j)+2]);
        */		
            
                glMultiTexCoord2f(GL_TEXTURE0,GLfloat(j)/width,GLfloat(i)/length);
                glMultiTexCoord2f(GL_TEXTURE1,GLfloat(j)/width,GLfloat(i)/length);
                glMultiTexCoord2f(GL_TEXTURE2,GLfloat(j)/width,GLfloat(i)/length);
                glMultiTexCoord2f(GL_TEXTURE3,GLfloat(j)/width,GLfloat(i)/length);
                
                                
                glVertex3f(terrainData[i*(width)+j][0],terrainData[i*(width)+j][1],terrainData[i*(width)+j][2]); 
                
        /*		glNormal3f(terrainNormals[3*((i+1)*terrainGridWidth + j)],
                          terrainNormals[3*((i+1)*terrainGridWidth + j)+1],
                          terrainNormals[3*((i+1)*terrainGridWidth + j)+2]);
        */		
                glMultiTexCoord2f(GL_TEXTURE0,GLfloat(j)/width,GLfloat(i+1)/length);
                glMultiTexCoord2f(GL_TEXTURE1,GLfloat(j)/width,GLfloat(i+1)/length);
                glMultiTexCoord2f(GL_TEXTURE2,GLfloat(j)/width,GLfloat(i+1)/length);
                glMultiTexCoord2f(GL_TEXTURE3,GLfloat(j)/width,GLfloat(i+1)/length);
                
                temp2=(i+1)*(width)+j; //terrainData is a 1D array 
                glVertex3f(terrainData[temp2][0],terrainData[temp2][1],terrainData[temp2][2]);
            }
            glEnd();
        }
        
   glPopMatrix();
   glFlush();
   glutSwapBuffers();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(70.0, (GLfloat) w/(GLfloat) h, 1.0, 150.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0.0,-70.0,70.0,0.0,0.0,0.0,0.0,1.0,1.0);
}



void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
        exit(0);
        break;
    case 'w':
        rotatex=rotatex+0.5;
        if(rotatex>360.0)
            rotatex=0.0;
        break;
    case 's':
        rotatex=rotatex-0.5;
        if(rotatex<-360.0)
            rotatex=0.0;
        break;

    case 'a':
        rotatez=rotatez+0.5;
        if(rotatez>360.0)
            rotatez=0.0;
        break;
    case 'd':
        rotatez=rotatez-0.5;
        if(rotatez<-360.0)
            rotatez=0.0;
        break;
    default:
        break;
    }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(550, 550);
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   fprintf(
        stdout,
        "INFO: OpenGL Version: %s\n",
        glGetString(GL_VERSION)
    );
   compute();
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutIdleFunc(display);
   glutKeyboardFunc(keyboard);
   glutFullScreen();
   glutMainLoop();
   return 0; 
}