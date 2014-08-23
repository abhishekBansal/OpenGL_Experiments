#define GLUT_DISABLE_ATEXIT_HACK
#include <cstdlib>
#include "windows.h"
#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <gl/glui.h>
#include <gl/glext.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define gridLength 100	
#define gridWidth 100

//  define the window position on screen
int window_x;
int window_y;

//  variables representing the window size
int window_width = 650;
int window_height = 500;
int mainWindow=0;

// stuff for lighting
//material properties for terrain
GLfloat tSpecular[] = {.1,.1,.1,1.0};
GLfloat tShininess[] = {5.0};
GLfloat tAmbient[] = {0.1,0.1,0.1,1.0};
GLfloat tDiffuse[] = {.95,.95,.95,1.0}; 
GLfloat tGrey[] = {0.2,0.2,.2,1.0};

GLfloat wAmbient[] = {0.3,0.3,0.9,1.0};
GLfloat wDiffuse[] = {0.9,0.9,.9,1.0};
GLfloat wSpecular[] = {.1,.1,.1,1.0};
GLfloat wShininess[] = {50.0};

GLfloat lPosition[] = {0.0,0.0,50.0,1.0};


//for normal calculations
static int width=gridWidth ,length=gridLength;
float *terrainNormals = NULL;

GLfloat z=0.0,rotatex=0.0,rotatez=0.0;
int iterations=0;
bool startIterations=false;
float changeHeight=0.2;

GLfloat **terrainData=NULL;
int wireframe=1;
bool anyChange=false;

float smoothingMatrix[3][3]={{1.0/9.0,1.0/9.0,1.0/9.0},
                            {1.0/9.0,1.0/9.0,1.0/9.0},
                            {1.0/9.0,1.0/9.0,1.0/9.0}};

/**************************************************GLUI STUFF ***********/

GLUI * glui_window;
float rotation_matrix[16]	//  Rotation Matrix Live Variable Array
                            = { 1.0, 0.0, 0.0, 0.0, 
                                0.0, 1.0, 0.0, 0.0,
                                0.0, 0.0, 1.0, 0.0, 
                                0.0, 0.0, 0.0, 1.0 };	
float translate_xy[2]		//  Translation XY Live Variable
                            = {0, 0};			
float translate_z = 0;		//  Translation Z Live Variable
float scale = 1;			//  Spinner Scale Live Variable
float nfScaleX = 1;
float nfScaleY = 1;
float nfScaleZ = 1;

int listbox_item_id = 12;	//  Id of the selected item in the list box

/**************************    WATER RELATED VARIABLES    *************************/
int water =0; //live variable for enabling or disabling water (checkbox)
float waterHeight=.2;
char waterTexturePath[1000]="assets\\water.bmp";
bool textureChanged=false;
GLuint waterTex[1];
/**********************************************************************************/
GLUI_Spinner *spinner;
GLUI_Spinner *nfSpinnerX;
GLUI_Spinner *nfSpinnerY;
GLUI_Spinner *nfSpinnerZ;
GLUI_Spinner *iterationNo;
/****************************** HEIGHT MAP *********************************/
GLUI_Panel *heightMapPanel;
char heightMapPath[1000] = "assets\\heightmap2.jpg";
bool heightMapMode=false; //this flag will help in determining current mode of program execution
/**************************************************************************/

//  Declare the IDs of controls generating callbacks
enum
{
    ALGORITHM_LISTBOX = 0,
    OBJECTYPE_RADIOGROUP,
    TRANSLATION_XY,
    TRANSLATION_Z,
    ROTATION,
    SCALE_SPINNER,
    NF_SCALE_SPINNER_X,
    NF_SCALE_SPINNER_Y,
    NF_SCALE_SPINNER_Z,
    HEIGHT_SPINNER,
    WATER_TEXTURE_PATH,
    HEIGHT_MAP_TEXTURE_PATH,
    BUTTON,
    NO_ITERATIONS,
    START_BUTTON,
    PAUSE_BUTTON,
    STOP_BUTTON,
    SMOOTH_BUTTON,
    HEIGHTMAP_BUTTON,
    TOGGLE_WIREFRAME,
    QUIT_BUTTON
};
/*************************************************************************/

/******************USER OPTIONS*******************************************/
int activeAlgo=1,currentIteration=0;
enum
{
    FAULT_ALGO_SINE=1,
    FAULT_ALGO_COSINE,
    FAULT_ALGO_STEP,
    CIRCLES_ALGO,
    HEIGHT_MAPS
};
/*************************************************************************/




int allocateTerrain(int w, int l)
{
    if(terrainData)
    {
        std::cout<<"\nThere already exist terrain data. Deleting it!!!"; 
        for(int i=0; i< length*width; i++)
            delete [] terrainData[i];
        delete [] terrainData;
    }
    if(!(terrainData= new GLfloat*[w*l]))
        return 0;
    for(int i=0; i<l*w ;i++)
    {
        if(!(terrainData[i]=new GLfloat[3]))
            return 0;
    }

    if(terrainNormals)
    {
        std::cout<<"\nfunc:allocateTerrain() Deleting previously allocated memory for normals...!!";
        delete [] terrainNormals;
    }
    terrainNormals = new float[w * l * sizeof(float) * 3];
    if (terrainNormals == NULL)
    {
        std::cout<<"\nfunc:allocateTerrain() Terrain Normals cannot be allocated Memory !!!";
        return 0;
    }
    width=w;   //change global values according to current trends
    length=l;
    return 1;
}
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
void centerOnScreen ()
{
    window_x = (glutGet (GLUT_SCREEN_WIDTH) - window_width)/2;
    window_y = (glutGet (GLUT_SCREEN_HEIGHT) - window_height)/2;
}
void idleGLUI()
{
    glutSetWindow (mainWindow);
    glutPostRedisplay ();
    Sleep (5);
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
    length=float(gridLength);
    width=float(gridWidth);
}


void init(void)
{    
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glLightfv(GL_LIGHT0,GL_AMBIENT,tAmbient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,tDiffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,tSpecular);
    glLightfv(GL_LIGHT0,GL_POSITION,lPosition);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    if( !allocateTerrain(100,100))
    {
        std::cout<<"\nProblem during allocation of terrain memory....!!!!";
    }
    else std::cout<<"\nIn function init() Terrain Data allocated Successfully.";
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
          y+=1;
       }
       y=-(float(gridWidth)/2.0);
       x+=1;
   }
   std::cout<<"\nIn function init() computing normals.";
   terrainComputeNormals();
   std::cout<<"\nIn function init() Normals computed.";
 }
void faultAlgorithm(int type)
{
    float v = rand();
    float a = sin(v);
    float b = cos(v);
    float d = sqrt(float(gridLength)*float(gridLength) + float(gridWidth)*float(gridWidth)); // rand() / RAND_MAX gives a random number between 0 and 1.
    float temp=float(rand()) / float(RAND_MAX);			// therefore c will be a random number between -d/2 and d/2
    float c = temp * d - d/2;
    float p=0.0;
    int temp1=0;
    float x=-(float(gridLength)/2.0),y=-(float(gridWidth)/2.0),z=0.0;
//	std::cout<<"\nFault Algorithm type = "<<type;
    for(x=-(float(gridLength)/2.0);x<(float(gridLength)/2.0);x+=1.0)
    {
        for(float y=-(float(gridWidth)/2.0);y<(float(gridWidth)/2.0);y+=1.0)
        {
            p=0.0;
            p=a*x + b*y - c;
            if(type==FAULT_ALGO_STEP)
            {
                if (p<0) 
                {
                    terrainData[temp1][2]-=changeHeight;
                    temp1++;
                }
                else if(p>0)
                {
                    terrainData[temp1][2]+=changeHeight;
                    temp1++;
                }
            }
            else
            {
                if(p>=-1.57 && p<=1.57)
                {
                    if (type==FAULT_ALGO_SINE)
                        terrainData[temp1][2]+=sin(p)/5; 
                    else if(type==FAULT_ALGO_COSINE)
                        terrainData[temp1][2]+=cos(p)/5;
                
                    temp1++;
                }
                else if(p<-1.57)
                {
                    terrainData[temp1][2]-=changeHeight;
                    temp1++;
                }
                else if(p>1.57)
                {
                    terrainData[temp1][2]+=changeHeight;
                    temp1++;
                }
                else
                    break;
            }
        }
    }
}


void circlesAlgorithm()
{
    float x = rand()%gridLength;
    float y = rand()%gridWidth;
    float temp=y*gridLength+x;

    x=terrainData[(int)temp][0];
    y=terrainData[(int)temp][1];

    float d=0.0;
    int temp1=0;
    float radius=5.0;

    for(int i=0;i<gridLength;i+=1.0)
    {
        for(int j=0;j<gridWidth;j+=1.0)
        {
            d=0.0;
            d=sqrt(((terrainData[temp1][0]-x)*(terrainData[temp1][0]-x)+(terrainData[temp1][1]-y)*(terrainData[temp1][1]-y)));
            d=d/radius;
            if(d<=1.0)
            {
                terrainData[temp1][2]+=cos(1.57*d);
            }
            temp1++;
        }
    }

}

void loadTerrainFromHeightMap()
{
    IplImage *heightMapImage=cvLoadImage(heightMapPath);
    if(!heightMapPath)
        std::cout<<"\nHeightMap Image Could Not Be loaded !!!!";
    else
        std::cout<<"\nHeightMap loaded successfully!!!";

    IplImage *greyMap=cvCreateImage(cvSize(heightMapImage->width,heightMapImage->height),IPL_DEPTH_8U,1);
    cvCvtColor(heightMapImage,greyMap,CV_RGBA2GRAY);

    if(allocateTerrain(greyMap->width,greyMap->height))
        std::cout<<"\nTerrain Data Allocation for heightmap done.";
    length=greyMap->height;
    width=greyMap->width;
    float x=-(float(greyMap->height)/2.0), y=-(float(greyMap->width)/2.0);
     long int temp=0;
    for(int i=0;i<length;i++)
    {
       for(int j=0;j<width;j++)
       {
          terrainData[temp][0]=x;
          terrainData[temp][1]=y;
          terrainData[temp][2]=((float)(greyMap->imageData[i*greyMap->widthStep + j]))/128.0;
          temp++;
          y+=1;
       }
       y=-(float(width)/2.0);
       x+=1;
   }
    cvReleaseImage(&greyMap);
    cvReleaseImage(&heightMapImage);
 terrainComputeNormals();
    
}
void smoothing(int itr)
{
    while(itr>0)
    {
        for(int i=1; i<length-1;i++)
        {	
            for(int j=1;j<width-1;j++)
            {
                terrainData[i*width+j][2]=smoothingMatrix[0][0]*terrainData[(i-1)*width + (j-1)][2]
                                        + smoothingMatrix[0][1]*terrainData[(i-1)*width + (j)][2]
                                        + smoothingMatrix[0][2]*terrainData[(i-1)*width + (j+1)][2]
                                        + smoothingMatrix[1][0]*terrainData[(i)*width + (j-1)][2]
                                        + smoothingMatrix[1][1]*terrainData[(i)*width + (j)][2]
                                        + smoothingMatrix[1][2]*terrainData[(i)*width + (j+1)][2]
                                        + smoothingMatrix[2][0]*terrainData[(i+1)*width + (j-1)][2]
                                        + smoothingMatrix[2][1]*terrainData[(i+1)*width + (j)][2]
                                        + smoothingMatrix[2][2]*terrainData[(i+1)*width + (j+1)][2];
            }
        }
        itr--;
    }


}
void render(int type)
{
    if(type==FAULT_ALGO_COSINE || type==FAULT_ALGO_SINE || type==FAULT_ALGO_STEP)
        faultAlgorithm(activeAlgo);
    else if(type==CIRCLES_ALGO)
        circlesAlgorithm();

}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(currentIteration<iterations && startIterations)
    {
        render(activeAlgo);
        currentIteration++;
        compute();
        terrainComputeNormals();
    }
    if(anyChange)
    {
    //	compute();
        terrainComputeNormals();
        anyChange=false;
    }
    int temp2=0;
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_SPECULAR, tSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS,tShininess);
        glMaterialfv(GL_FRONT, GL_AMBIENT, tGrey);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, tDiffuse);
        glTranslatef(translate_xy[0],translate_xy[1],translate_z);
        glScalef(nfScaleX,nfScaleY,nfScaleZ);
        glLightfv(GL_LIGHT0,GL_POSITION,lPosition);
        glRotatef(rotatex,1.0,0.0,0.0);
        glRotatef(rotatez,0.0,0.0,1.0);
        glMultMatrixf(rotation_matrix);
        if(wireframe)
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            
        for(int i=0; i<length-1;i++)
        {	
            glBegin(GL_TRIANGLE_STRIP);
                for(int j=0;j<width;j++)
                {
                    glNormal3f(terrainNormals[3*(i*width + j)],
                                terrainNormals[3*(i*width + j)+1],
                                terrainNormals[3*(i*width + j)+2]);
                    glVertex3f(terrainData[i*(width)+j][0],terrainData[i*(width)+j][1],terrainData[i*(width)+j][2]); 
                    glNormal3f(terrainNormals[3*((i+1)*width + j)],
                                terrainNormals[3*((i+1)*width + j)+1],
                                terrainNormals[3*((i+1)*width + j)+2]);

                    if( (3*((i+1)*width + j)+2) >= width * length *3)
                        std::cout<<"\nfunc:display() possible chances of heap curruption, since normal data exceeding its array bounds !!!!";
                    temp2=(i+1)*(width)+j; //terrainData is a 1D array 
                    glVertex3f(terrainData[temp2][0],terrainData[temp2][1],terrainData[temp2][2]);
                }
            glEnd();
        }
        if(water)
        {
            if(textureChanged)
            {
                waterTex[0]=loadTexture(waterTexturePath);
                textureChanged=false;
            }
            glPushMatrix();
            glMaterialfv(GL_FRONT, GL_SPECULAR, wSpecular);
            glMaterialfv(GL_FRONT, GL_SHININESS,wShininess);
            glMaterialfv(GL_FRONT, GL_AMBIENT, wAmbient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, wDiffuse);
            glColor3f(0.0,0.0,1.0);
            glBindTexture(GL_TEXTURE_2D, waterTex[0]);
            glEnable (GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE);
        
            glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
            glBegin(GL_QUADS);
                glTexCoord2f(0.0,0.0);
                glVertex3f(-(GLfloat)length/2.0,-(GLfloat)width/2.0,waterHeight);
                
                glTexCoord2f(16.0,0.0);
                glVertex3f((GLfloat)length/2.0,-(GLfloat)width/2.0,waterHeight);

                glTexCoord2f(16.0,16.0);
                glVertex3f((GLfloat)length/2.0,(GLfloat)width/2.0,waterHeight);
                
                glTexCoord2f(0.0,16.0);
                glVertex3f(-(GLfloat)length/2.0,(GLfloat)width/2.0,waterHeight);
            glEnd();
            glPopMatrix();
                
            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
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
   gluPerspective(90.0, (GLfloat) w/(GLfloat) h, 1.0, 500.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0.0,-70.0,70.0,0.0,0.0,0.0,0.0,1.0,1.0);
}



void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
        for(int i=0; i< length*width; i++)
            delete terrainData[i];
        delete terrainData;
        delete [] terrainNormals;
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
void glui_callback (int control_id)
{
    switch (control_id)
    {
        case ALGORITHM_LISTBOX:
            switch (listbox_item_id)
            {
                case FAULT_ALGO_SINE:
                    activeAlgo=FAULT_ALGO_SINE;
                    heightMapPanel->disable();
                    break;
                case FAULT_ALGO_COSINE:
                    activeAlgo=FAULT_ALGO_COSINE;
                    heightMapPanel->disable();
                    break;
                case FAULT_ALGO_STEP:	
                    activeAlgo=FAULT_ALGO_STEP;
                    heightMapPanel->disable();
                    break;
                case CIRCLES_ALGO:
                    activeAlgo=CIRCLES_ALGO;
                    heightMapPanel->disable();
                    break;
                case HEIGHT_MAPS:
                    startIterations=false;
                    activeAlgo=HEIGHT_MAPS;
                    heightMapPanel->enable();
                    break;
            }
        break;
        case OBJECTYPE_RADIOGROUP:
        break;
        case BUTTON:
            std::cout<<"\nNew filePath is : "<<waterTexturePath;
            textureChanged=true;
            glutPostRedisplay();
            break;
        case TRANSLATION_XY:
            break;
        case TRANSLATION_Z:
            break;
        case ROTATION:
        break;
        case SCALE_SPINNER:
            nfScaleX=scale;
            nfScaleY=scale;
            nfScaleZ=scale;
            nfSpinnerX->set_float_val(scale);
            nfSpinnerY->set_float_val(scale);
            nfSpinnerZ->set_float_val(scale);
            anyChange=true;
            glutPostRedisplay();
        break;
        case NF_SCALE_SPINNER_X:
        case NF_SCALE_SPINNER_Y:
        case NF_SCALE_SPINNER_Z:
        break;
        case PAUSE_BUTTON:
            startIterations=false;
            break;
        case START_BUTTON:
            std::cout<<"\nStart Button Pressed";
            if(heightMapMode==true)  // now we will be allocating New terrain data only when we are migrating here from heightmap mode
            {
                if(allocateTerrain(100,100))
                    std::cout<<"\nMigrating from HeightMapMode : New Terrain Data allocated Successfully.";

                long int temp=0;
                float x=-(float(gridLength)/2.0),y=-(float(gridWidth)/2.0),z=0.0;
                for(int i=0;i<length;i++)    //re initializing the terrain
                {
                    for(int j=0;j<width;j++)
                    {
                        terrainData[temp][0]=x;
                        terrainData[temp][1]=y;
                        terrainData[temp][2]=z;
                        temp++;
                        y+=1;
                    }
                y=-(float(gridWidth)/2.0);
                x+=1;
                }
                currentIteration=0;iterations=1;
                iterationNo->set_int_val(1);
                std::cout<<"\n Terrain reinitialized Successfully.\ndisabling heightmapmode flag.";
                heightMapMode=false;
            }
    //		length=100; 
    //		width=100;
            startIterations=true;
            //glutPostRedisplay();
            break;
        case TOGGLE_WIREFRAME:
            break;
        case SMOOTH_BUTTON:
            smoothing(1);
            anyChange=true;
            glutPostRedisplay();
            break;
        case HEIGHTMAP_BUTTON:
            startIterations=false;
            loadTerrainFromHeightMap();
            heightMapMode=true;    // true only when terrain was loaded from a height map
            break;
        //  Quit Button clicked
        case QUIT_BUTTON:
            for(int i=0; i< length*width; i++)
                delete[] terrainData[i];
            delete[] terrainData;
            delete [] terrainNormals;
            glDeleteTextures(1,waterTex);
            printf ("Quit Button clicked... Exit!\n");
            exit (1);
            break;
    }
}


void setupGLUI ()
{
    //  Set idle function
    GLUI_Master.set_glutIdleFunc (idleGLUI);

    //  Create GLUI window
    glui_window = GLUI_Master.create_glui( "Controles",0,100,100  );

    /*****************ALGORITHM PANEL**************************************************/
    
    GLUI_Panel *op_panel = glui_window->add_panel ("Algorithm Selection");
    GLUI_Listbox *Algorithm_listbox = glui_window->add_listbox_to_panel (op_panel, 
                                    "Algorithm", &listbox_item_id, ALGORITHM_LISTBOX, glui_callback);
    
    Algorithm_listbox->add_item (1, "FaultAlgorithm Sine Function");
    Algorithm_listbox->add_item (2, "FaultAlgorithm Cosine Function");
    Algorithm_listbox->add_item (3, "FaultAlgorithm Step Function");
    Algorithm_listbox->add_item (4, "Circles Algorithm");
    Algorithm_listbox->add_item (5, "Height Maps");
    Algorithm_listbox->set_int_val (1);

    iterationNo=glui_window->add_spinner_to_panel(op_panel,"No. of Iterations",GLUI_SPINNER_INT,&iterations,NO_ITERATIONS,glui_callback);
    iterationNo->set_int_val(0);
    GLUI_Checkbox *wireframeCheckbox=glui_window->add_checkbox_to_panel(op_panel,"Wireframe",&wireframe,TOGGLE_WIREFRAME,glui_callback);
    GLUI_Spinner *heightChange = glui_window->add_spinner_to_panel(op_panel,"Height Change",GLUI_SPINNER_FLOAT,&changeHeight,HEIGHT_SPINNER,glui_callback);
    heightChange->set_float_val(0.2);
//	glui_window->add_column_to_panel (op_panel,false);

    GLUI_Button *startButton=glui_window->add_button_to_panel(op_panel,"Start",START_BUTTON,glui_callback);
    GLUI_Button *pauseButton=glui_window->add_button_to_panel(op_panel,"Pause",PAUSE_BUTTON,glui_callback);
    GLUI_Button *smoothButton=glui_window->add_button_to_panel(op_panel,"Smooth",SMOOTH_BUTTON,glui_callback);
    
    /************************************************************************************/

    /*****************************HEIGHT MAPS********************************************/
    heightMapPanel= glui_window->add_panel("Height Maps");
    GLUI_EditText* heightMapText= glui_window->add_edittext_to_panel(heightMapPanel,"Choose Texture",GLUI_EDITTEXT_TEXT,&heightMapPath,HEIGHT_MAP_TEXTURE_PATH,glui_callback);
    heightMapText->set_text(heightMapPath);
    heightMapText->set_w(175);
    GLUI_Button *heightMapButton=glui_window->add_button_to_panel(heightMapPanel,"Enter",HEIGHTMAP_BUTTON,glui_callback);
    heightMapPanel->disable();
    /************************************************************************************/
    
    /************************** WATER PANEL ***************************/
    GLUI_Panel *waterPanel= glui_window->add_panel("Water Settings");
    
    GLUI_EditText* waterPath= glui_window->add_edittext_to_panel(waterPanel,"Choose Texture",GLUI_EDITTEXT_TEXT,waterTexturePath,WATER_TEXTURE_PATH,glui_callback);
    waterPath->set_text(waterTexturePath);
    waterPath->set_w(175);
    glui_window->add_checkbox_to_panel(waterPanel,"Enable Water",&water);
    glui_window->add_spinner_to_panel(waterPanel,"Water height",GLUI_SPINNER_FLOAT,&waterHeight);
    glui_window->add_column_to_panel (waterPanel,false);	
    GLUI_Button *button1 = glui_window->add_button_to_panel( waterPanel,"Enter", BUTTON, glui_callback);
    /******************************************************************/
 
    /************************ TRANSFORMATIONAL PANEL ****************************/ 
    GLUI_Panel *transformation_panel = glui_window->add_panel ("Transformation");
    GLUI_Panel *transformation_panel1 = glui_window->add_panel_to_panel (transformation_panel, "Translation & Rotation");
    GLUI_Translation *translation_xy = glui_window->add_translation_to_panel (transformation_panel1, "Translation XY", GLUI_TRANSLATION_XY, translate_xy, TRANSLATION_XY, glui_callback );
    translation_xy->set_speed( 0.05 );
    //  Add column, but don't draw it
    glui_window->add_column_to_panel (transformation_panel1, false);
    GLUI_Translation *translation_z = glui_window->add_translation_to_panel (transformation_panel1, "Translation Z", GLUI_TRANSLATION_Z, &translate_z, TRANSLATION_Z, glui_callback );
    translation_z->set_speed( 0.05 );

    //glui_window->add_column_to_panel (transformation_panel1, false);
    GLUI_Panel *rotationPanel=glui_window->add_panel_to_panel(transformation_panel,"Rotation");
    glui_window->add_rotation_to_panel (rotationPanel, "", rotation_matrix, ROTATION, glui_callback);

    //glui_window->add_separator_to_panel (transformation_panel1);
    GLUI_Panel *scalePanel= glui_window->add_panel_to_panel(transformation_panel,"Scale");
    
    glui_window->add_statictext_to_panel(scalePanel,"Uniform Scale");
    spinner = glui_window->add_spinner_to_panel (scalePanel, "", GLUI_SPINNER_FLOAT, &scale, SCALE_SPINNER, glui_callback);
    spinner->set_float_limits ( -4.0, 4.0 );
    spinner->set_speed(0.2);
//	spinner->set_w(5);
    
    glui_window->add_statictext_to_panel(scalePanel,"Non-Uniform Scale");
    nfSpinnerX = glui_window->add_spinner_to_panel (scalePanel, "X", GLUI_SPINNER_FLOAT, &nfScaleX, NF_SCALE_SPINNER_X, glui_callback);
    nfSpinnerX->set_float_limits ( -4.0, 4.0 );
    nfSpinnerX->set_speed(0.2);

    nfSpinnerY = glui_window->add_spinner_to_panel (scalePanel, "Y", GLUI_SPINNER_FLOAT, &nfScaleY, NF_SCALE_SPINNER_Y, glui_callback);
    nfSpinnerY->set_float_limits ( -4.0, 4.0 );
    nfSpinnerY->set_speed(0.2);

    nfSpinnerZ = glui_window->add_spinner_to_panel (scalePanel, "Z", GLUI_SPINNER_FLOAT, &nfScaleZ, NF_SCALE_SPINNER_Z, glui_callback);
    nfSpinnerZ->set_float_limits ( -4.0, 4.0 );
    nfSpinnerZ->set_speed(0.2);
    glui_window->add_column_to_panel (scalePanel, false);

    /*************************************************************************/

    //  Add the Quit Button
    glui_window->add_button ("Quit", QUIT_BUTTON, glui_callback);

    glui_window->set_main_gfx_window( mainWindow );
}


int main(int argc, char** argv)
{
   centerOnScreen ();
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(window_width, window_height);
   glutInitWindowPosition(window_x, window_y);
   mainWindow = glutCreateWindow("Simple Terrain Generator by Abhishek Bansal");
   setupGLUI ();
   compute();
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
  // glutIdleFunc(display);
   glutKeyboardFunc(keyboard);
  
   glutMainLoop();
   return 0; 
}