#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "abShaders.h"
#include <gl\glut.h>
#include <GL\glui.h>
#include <time.h>


char *floorImg = "assets\\marble.jpg";
char *leftWallImg = "assets\\redGranite.jpg";
char *backWallImg = "assets\\redGranite.jpg";


//Shaders Initialization
//abShader rayTrace("perPixel.vert","perPixel.frag");
abShader rayTrace("shaders\\rayTrace.vert","shaders\\rayTraceBlinnPhong.frag");
abShader rayTraceToon("shaders\\rayTrace.vert","shaders\\rayTraceToon.frag");
abShader rayTracePhong("shaders\\rayTrace.vert","shaders\\rayTracePhong.frag");
abShader rayTraceWard("shaders\\rayTrace.vert","shaders\\rayTraceWard.frag");
//....................//
GLuint floorTex,leftWallTex,backWallTex;

//uniform light position 
float  lPos[3] ={10.0, 30.0, 5.0};
float *halfVector=NULL;
float  camera[3] ={0.0,10.0,50.0};
int refL1=1, refL2= 1,shadow=1;
float textureScale=.02;
//Ward shading parameters
float alphaX = 1.0;
float alphaY = 1.0;
float tangent[3] = {1.0,0.0,0.0};
//GLUI Parameters

GLUI * glui_window;
//  Set up the GLUI window and its components
void setupGLUI ();
//  Idle callack function
void idle ();

//for frame rate calculations
int frame=0;
long time1,timebase=0;
char s[30];
void *font =  GLUT_BITMAP_TIMES_ROMAN_24;

//........................//

//  Declare callbacks related to GLUI
void glui_callback (int arg);

int mainWindow=0;
int shaderType=0;
enum{
    BLINN_PHONG=0,
    PHONG,
    TOON,
    WARD
};
enum{
    SHADERTYPE_RADIOGROUP=0,
};

//.........................//

void idle()
{
    glutSetWindow (mainWindow);
    glutPostRedisplay ();
    Sleep (5);
}
void setupGLUI ()
{
    //  Set idle function
    GLUI_Master.set_glutIdleFunc (idle);
    glui_window = GLUI_Master.create_glui ("Options", 0, 200, 200);

    GLUI_Panel *masterPanel = glui_window->add_panel ("Ray Tracing");

    GLUI_Panel *shaderTypePanel = glui_window->add_panel_to_panel(masterPanel,"ShaderType");
    GLUI_RadioGroup *shaderTypeRadioGroup = glui_window->add_radiogroup_to_panel(shaderTypePanel,&shaderType,SHADERTYPE_RADIOGROUP,glui_callback);
    glui_window->add_radiobutton_to_group(shaderTypeRadioGroup,"Blinn Phong");
    glui_window->add_radiobutton_to_group(shaderTypeRadioGroup,"Phong");
    glui_window->add_radiobutton_to_group(shaderTypeRadioGroup,"Toon Shading");
    glui_window->add_radiobutton_to_group(shaderTypeRadioGroup,"Ward Anisotropic");
    
    GLUI_Rollout *commonRollout = glui_window->add_rollout ("Common Parameters");
    GLUI_Spinner *lightPositionXSpinner = glui_window->add_spinner_to_panel(commonRollout,"LightX",GLUI_SPINNER_FLOAT,&lPos[0]);
    GLUI_Spinner *lightPositionYSpinner = glui_window->add_spinner_to_panel(commonRollout,"LightY",GLUI_SPINNER_FLOAT,&lPos[1]);
    GLUI_Spinner *lightPositionZSpinner = glui_window->add_spinner_to_panel(commonRollout,"LightZ",GLUI_SPINNER_FLOAT,&lPos[2]);

    GLUI_Spinner *cameraPositionXSpinner = glui_window->add_spinner_to_panel(commonRollout,"CameraX",GLUI_SPINNER_FLOAT,&camera[0]);
    GLUI_Spinner *cameraPositionYSpinner = glui_window->add_spinner_to_panel(commonRollout,"CameraY",GLUI_SPINNER_FLOAT,&camera[1]);
    GLUI_Spinner *cameraPositionZSpinner = glui_window->add_spinner_to_panel(commonRollout,"CameraZ",GLUI_SPINNER_FLOAT,&camera[2]);

    GLUI_Checkbox *refLevel1CheckBox = glui_window->add_checkbox_to_panel(commonRollout,"Ref L1",&refL1);
    GLUI_Checkbox *refLevel2CheckBox = glui_window->add_checkbox_to_panel(commonRollout,"Ref L2",&refL2);
    GLUI_Checkbox *shadowCheckBox = glui_window->add_checkbox_to_panel(commonRollout,"Shadow",&shadow);

    GLUI_Spinner *textScaleSpinner = glui_window->add_spinner_to_panel(commonRollout,"Tex Scaling",GLUI_SPINNER_FLOAT,&textureScale);

    GLUI_Rollout *WardRollout = glui_window->add_rollout ("Ward Anisotropic Parameters");
    GLUI_Spinner *tangentX = glui_window->add_spinner_to_panel(WardRollout,"tangentX",GLUI_SPINNER_FLOAT,&tangent[0]);
    GLUI_Spinner *tangentY = glui_window->add_spinner_to_panel(WardRollout,"tangentY",GLUI_SPINNER_FLOAT,&tangent[1]);
    GLUI_Spinner *tangentZ = glui_window->add_spinner_to_panel(WardRollout,"tangentZ",GLUI_SPINNER_FLOAT,&tangent[2]);
    GLUI_Spinner *alphaXSpinner = glui_window->add_spinner_to_panel(WardRollout,"alphaX",GLUI_SPINNER_FLOAT,&alphaX);
    GLUI_Spinner *alphaYSpinner = glui_window->add_spinner_to_panel(WardRollout,"alphaY",GLUI_SPINNER_FLOAT,&alphaY);
    

}
void glui_callback (int control_id)
{
    //  Behave based on control ID
    /*switch (control_id)
    {
    case SHADERTYPE_RADIOGROUP:
        if (shaderType==OPENGL_SIMULATION)
        {
            glUseProgram(0);
            setShaders(openGLVS,openGLFS);
            glutPostRedisplay();
        }
        else if(shaderType==PERPIXEL)
        {
            glUseProgram(0);
            setShaders(perPixelVS,perPixelFS);
            glutPostRedisplay();
        }
        break;
    default:
        break;
    }*/
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
   glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   return Texture;
}
float * normalize(float *vec)
{
    float den = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    float vec1[3];
    vec1[0]= vec[0] / den;
    vec1[1]= vec[1] / den;
    vec1[2]= vec[2] / den;
    return vec1;
}

float* calcHalfVector(float *vec1,float *vec2)
{
     float hv[3] ;
     float *vec01 = normalize(vec1);
     float *vec02 = normalize(vec2);

     hv[0] = vec01[0] + vec02[0];
     hv[1] = vec01[1] + vec02[1];
     hv[2] = vec01[2] + vec02[2];

     return normalize (hv);

}
void init()
{
    glEnable(GL_DEPTH);
    floorTex = loadTexture(floorImg);
    leftWallTex = loadTexture(leftWallImg);
    backWallTex = loadTexture(backWallImg);
    
    rayTrace.abCreate();
    rayTrace.abCompileAndLink();

    rayTraceToon.abCreate();
    rayTraceToon.abCompileAndLink();
    
    rayTracePhong.abCreate();
    rayTracePhong.abCompileAndLink();

    rayTraceWard.abCreate();
    rayTraceWard.abCompileAndLink();
}

void reshape(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

//	ratio = 1.0f * w / h;
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the clipping volume
    glOrtho(0,1,0,1,0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}


void renderBitmapString(
        float x,
        float y,
        float z,
        void *font,
        char *string) {

  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    float t = clock()/1000.0;
    //std::cout<<t<<"\n";
    glLoadIdentity();
    //glBindTexture(GL_TEXTURE_2D,floorTex);
    if(shaderType==BLINN_PHONG)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,floorTex);
        rayTrace.abSetUniformTexture("floorTex",0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,leftWallTex);
        rayTrace.abSetUniformTexture("leftWallTex",1);
    
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,backWallTex);
        rayTrace.abSetUniformTexture("backWallTex",2);
        rayTrace.abSetUniformVariable("textureScale",textureScale);

        rayTrace.abSetUniformVariable("time",t);
        rayTrace.abSetUniformVariable("lPos",lPos);
        rayTrace.abSetUniformVariable("camera",camera);
        rayTrace.abSetUniformVariable("refL1",refL1);
        rayTrace.abSetUniformVariable("refL2",refL2);
        rayTrace.abSetUniformVariable("shadow",shadow);
        
        halfVector = calcHalfVector(camera , lPos); 
        rayTrace.abSetUniformVariable("halfVector",halfVector);
        rayTrace.abUseProgram();  
    }
    else if(shaderType==PHONG)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,floorTex);
        rayTracePhong.abSetUniformTexture("floorTex",0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,leftWallTex);
        rayTracePhong.abSetUniformTexture("leftWallTex",1);
    
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,backWallTex);
        rayTracePhong.abSetUniformTexture("backWallTex",2);
        rayTracePhong.abSetUniformVariable("textureScale",textureScale);

        rayTracePhong.abSetUniformVariable("time",t);
        rayTracePhong.abSetUniformVariable("lPos",lPos);
        rayTracePhong.abSetUniformVariable("camera",camera);
        rayTracePhong.abSetUniformVariable("refL1",refL1);
        rayTracePhong.abSetUniformVariable("refL2",refL2);
        rayTracePhong.abSetUniformVariable("shadow",shadow);
        rayTracePhong.abUseProgram();
    }
    else if(shaderType==TOON)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,floorTex);
        rayTraceToon.abSetUniformTexture("floorTex",0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,leftWallTex);
        rayTraceToon.abSetUniformTexture("leftWallTex",1);
    
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,backWallTex);
        rayTraceToon.abSetUniformTexture("backWallTex",2);
        rayTraceToon.abSetUniformVariable("textureScale",textureScale);

        rayTraceToon.abSetUniformVariable("time",t);
        rayTraceToon.abSetUniformVariable("lPos",lPos);
        rayTraceToon.abSetUniformVariable("camera",camera);
        rayTraceToon.abSetUniformVariable("refL1",refL1);
        rayTraceToon.abSetUniformVariable("refL2",refL2);
        rayTraceToon.abSetUniformVariable("shadow",shadow);
        rayTraceToon.abUseProgram();
    }
    else if(shaderType==WARD)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,floorTex);
        rayTraceWard.abSetUniformTexture("floorTex",0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,leftWallTex);
        rayTraceWard.abSetUniformTexture("leftWallTex",1);
    
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,backWallTex);
        rayTraceWard.abSetUniformTexture("backWallTex",2);
        rayTraceWard.abSetUniformVariable("textureScale",textureScale);

        rayTraceWard.abSetUniformVariable("time",t);
        rayTraceWard.abSetUniformVariable("lPos",lPos);
        rayTraceWard.abSetUniformVariable("refL1",refL1);
        rayTraceWard.abSetUniformVariable("refL2",refL2);
        rayTraceWard.abSetUniformVariable("shadow",shadow);
        rayTraceWard.abSetUniformVariable("camera",camera);
        rayTraceWard.abSetUniformVariable("alphaX",alphaX);
        rayTraceWard.abSetUniformVariable("alphaY",alphaY);
        rayTraceWard.abSetUniformVariable("tangent",tangent);
        rayTraceWard.abUseProgram();
    }

    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(1, 0);
        glVertex2f(1, 1);
        glVertex2f(0, 1);
    glEnd();
    
    rayTrace.abStopProgram();
    
    frame++;
    time1=glutGet(GLUT_ELAPSED_TIME);

    if (time1 - timebase > 1000) {
        sprintf(s,"FPS:%4.2f",
            frame*1000.0/(time1-timebase));
    //	s[8]='\0';
        timebase = time1;
        frame = 0;
    }

    glColor3f(0.0f,1.0f,1.0f);

    renderBitmapString(0.05,.95,0.0,(void *)font,s);
    glFlush();

    glutSwapBuffers();
}	 



void main(int argc, char** argv)
{
  glutInit            ( &argc, argv ); // Erm Just Write It =)
  glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA ); // Display Mode
  glutInitWindowPosition (0,0);
  glutInitWindowSize  ( 1280, 720 ); // If glutFullScreen wasn't called this is the window size
  mainWindow = glutCreateWindow    ( "RayTracer by Abhishek Bansal" ); // Window Title (argv[0] for current directory as title)
  init ();
  setupGLUI ();
  //glutFullScreen      ( );          // Put Into Full Screen
  glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
  glutReshapeFunc     ( reshape );
//  glutKeyboardFunc    ( keyboard );
//  glutSpecialFunc     ( arrow_keys );
//  glutIdleFunc			 ( display );
  glutMainLoop        ( );          // Initialize The Main Loop
  
}