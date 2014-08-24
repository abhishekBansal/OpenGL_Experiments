/*
  Simple Demo for GLSL
  www.lighthouse3d.com
*/
#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <GL/glew.h>
#include <GL/glui.h>
#include <GL/glut.h>
#include <cstdlib>
#include "windows.h"

char *openGLVS = "shaders\\OpenGLSimulation.vert";
char *openGLFS = "shaders\\OpenGLSimulation.frag";

char *perPixelVS = "shaders\\perPixel.vert";
char *perPixelFS = "shaders\\perPixel.frag";

char *perPixelPLVS="shaders\\pointlight.vert";
char *perPixelPLFS="shaders\\pointlight.frag";

char *perPixelSLVS="shaders\\spotLight.vert";
char *perPixelSLFS="shaders\\spotLight.frag";

using namespace std;

GLint loc;
GLuint v,f,oglSimulationProgram,perPixelDirProgram,perPixelPointProgram,perPixelSpotProgram;
/////////////////////////////////////////// LIGHTING PARAMETERS //////////////////////////////////////////////////

float lpos[4] = {1.0,0.0,1.0,0.0};

GLfloat diffuse[] = { 0.5, 0.0, 1.0, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };

GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };

GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };

GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0,1.0, 1.0, 1.0 };
GLfloat shininess[] = { 100.0 };
GLfloat spotDir[] = {0.0,0.0,-1.0};
float kc=1,kl=0,kq=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////// GLUI PARAMETERS //////////////////////////////////////////////////

GLUI * glui_window;
//  Set up the GLUI window and its components
void setupGLUI ();
//  Idle callack function
void idle ();
//  Declare callbacks related to GLUI
void glui_callback (int arg);

int mainWindow=0;
int shaderType=0;
int modelType=0;
float spotCutOff=45.0;
float spotExponent=0.01;
enum {
    OPENGL_SIMULATION=0,
    PERPIXEL,
    PERPIXEL_POINT,
    PERPIXEL_POINT_SPOT
};
enum{
    SHADERTYPE_RADIOGROUP=0,
};
enum {
    TEAPOT=0,
    SPHERE,
    TORUS,
    CUBE
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *textFileRead(char *fn) {


    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fn != NULL) {
        fp = fopen(fn,"rt");

        if (fp != NULL) {
      
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

            if (count > 0) {
                content = new char[sizeof(char) * (count+1)];
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

int textFileWrite(char *fn, char *s) {

    FILE *fp;
    int status = 0;

    if (fn != NULL) {
        fp = fopen(fn,"w");

        if (fp != NULL) {
            
            if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
                status = 1;
            fclose(fp);
        }
    }
    return(status);
}

void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    float ratio = 1.0* w / h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective.
    gluPerspective(45,ratio,1,100);
    glMatrixMode(GL_MODELVIEW);
}

float a = 0;
void init()
{	
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}
void renderScene(void) {
    if(shaderType==OPENGL_SIMULATION)
    {
        lpos[3]=0;
        glUseProgram(oglSimulationProgram);
    }
    else if (shaderType==PERPIXEL)
    {
        lpos[3]=0;
        glUseProgram(perPixelDirProgram);
    }
    else if (shaderType==PERPIXEL_POINT)
    {
        lpos[3]=1;
        glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,kc);
        glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,kl);
        glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,kq);
        glUseProgram(perPixelPointProgram);
    }
    else if (shaderType==PERPIXEL_POINT_SPOT)
    {
        lpos[3]=1;
        glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,kc);
        glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,kl);
        glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,kq);
        glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,spotCutOff);
        glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,spotDir);
        glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,spotExponent);
        glUseProgram(perPixelSpotProgram);
    }
    
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    gluLookAt(0.0,0.0,5.0, 
              0.0,0.0,0.0,
              0.0f,1.0f,0.0f);
    
    //glRotatef(a,0,1,0);

    glColor3f(1.0,0.0,0.0);
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(modelType==TEAPOT) glutSolidTeapot(1);
    else if(modelType==SPHERE) glutSolidSphere(1,30,30);
    else if(modelType==TORUS) glutSolidTorus(.5,1,30,30);
    else if(modelType==CUBE) glutSolidCube(2);
    a+=0.4;

    glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {

    if (key == 27) 
        exit(0);
}

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        cout<<"glError in file %s @ line %d:"<< file<< line<< gluErrorString(glErr);
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        cout<<infoLog;
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        cout<<infoLog;
        free(infoLog);
    }
}



GLuint compileAndLinkShaders(char *vert, char* pixel ) 
{
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    
    char *vs = NULL,*fs = NULL;

    vs = textFileRead(vert);
    fs = textFileRead(pixel);

    const char * vv = vs;
    const char * ff = fs;

    glShaderSource(v, 1, &vv,NULL);
    glShaderSource(f, 1, &ff,NULL);

    delete [] vs; delete [] fs;

    glCompileShader(v);
    glCompileShader(f);

    printShaderInfoLog(v);
    printShaderInfoLog(f);

    GLuint programID = glCreateProgram();
    glAttachShader(programID,v);
    glAttachShader(programID,f);

    glLinkProgram(programID);
    printProgramInfoLog(programID);

    return programID;

}
void setShaders() {
    std::cout<<"\nTrying to compile Per Pixel Point programs\n";
    perPixelPointProgram=compileAndLinkShaders(perPixelPLVS,perPixelPLFS);
    
    std::cout<<"\nTrying to compile OGL SIM programs\n";
    oglSimulationProgram=compileAndLinkShaders(openGLVS,openGLFS);
    
    std::cout<<"\nTrying to compile Per Pixel Dir programs\n";
    perPixelDirProgram=compileAndLinkShaders(perPixelVS,perPixelFS);

    std::cout<<"\nTrying to compile Per Pixel SPOT programs\n";
    perPixelSpotProgram=compileAndLinkShaders(perPixelSLVS,perPixelSLFS);
}

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

    GLUI_Panel *masterPanel = glui_window->add_panel ("");

    //light properties panel
    GLUI_Panel *lightingPanel = glui_window->add_panel_to_panel(masterPanel,"Lighting Properties");
    
        //diffuse light
        GLUI_Spinner *diffuseLight[3];
        diffuseLight[0] = glui_window->add_spinner_to_panel(lightingPanel,"diffuseR",GLUI_SPINNER_FLOAT,&diffuse[0]);
        diffuseLight[0]->set_float_limits(0.0,1.0);
        diffuseLight[1] = glui_window->add_spinner_to_panel(lightingPanel,"diffuseG",GLUI_SPINNER_FLOAT,&diffuse[1]);
        diffuseLight[1]->set_float_limits(0.0,1.0);
        diffuseLight[2] = glui_window->add_spinner_to_panel(lightingPanel,"diffuseB",GLUI_SPINNER_FLOAT,&diffuse[2]);
        diffuseLight[2]->set_float_limits(0.0,1.0);

        glui_window->add_separator_to_panel(lightingPanel);

        //Ambient Light
        GLUI_Spinner *ambientLight[3];
        ambientLight[0] = glui_window->add_spinner_to_panel(lightingPanel,"ambientR",GLUI_SPINNER_FLOAT,&ambient[0]);
        ambientLight[0]->set_float_limits(0.0,1.0);
        ambientLight[1] = glui_window->add_spinner_to_panel(lightingPanel,"ambientG",GLUI_SPINNER_FLOAT,&ambient[1]);
        ambientLight[1]->set_float_limits(0.0,1.0);
        ambientLight[2] = glui_window->add_spinner_to_panel(lightingPanel,"ambientB",GLUI_SPINNER_FLOAT,&ambient[2]);
        ambientLight[2]->set_float_limits(0.0,1.0);

        glui_window->add_separator_to_panel(lightingPanel);

        //Specular Light
        GLUI_Spinner *specularLight[3];
        specularLight[0] = glui_window->add_spinner_to_panel(lightingPanel,"specularR",GLUI_SPINNER_FLOAT,&specular[0]);
        specularLight[0]->set_float_limits(0.0,1.0);
        specularLight[1] = glui_window->add_spinner_to_panel(lightingPanel,"specularG",GLUI_SPINNER_FLOAT,&specular[1]);
        specularLight[1]->set_float_limits(0.0,1.0);
        specularLight[2] = glui_window->add_spinner_to_panel(lightingPanel,"specularB",GLUI_SPINNER_FLOAT,&specular[2]);
        specularLight[2]->set_float_limits(0.0,1.0);

        glui_window->add_separator_to_panel(lightingPanel);

        //Light Position
        GLUI_Spinner *lightPosition[3];
        lightPosition[0] = glui_window->add_spinner_to_panel(lightingPanel,"LightPosX",GLUI_SPINNER_FLOAT,&lpos[0]);
        lightPosition[0]->set_float_limits(-10.0,10.0);
        lightPosition[1] = glui_window->add_spinner_to_panel(lightingPanel,"LightPosY",GLUI_SPINNER_FLOAT,&lpos[1]);
        lightPosition[1]->set_float_limits(-10.0,10.0);
        lightPosition[2] = glui_window->add_spinner_to_panel(lightingPanel,"LightPosZ",GLUI_SPINNER_FLOAT,&lpos[2]);
        lightPosition[2]->set_float_limits(-10.0,10.0);

        glui_window->add_separator_to_panel(lightingPanel);	

        GLUI_Panel *spotPanel = glui_window->add_panel_to_panel(lightingPanel,"SpotLight Parameters");
        GLUI_Spinner *spotCutOffSpinner;
        spotCutOffSpinner= glui_window->add_spinner_to_panel(spotPanel,"SpotCutOff",GLUI_SPINNER_FLOAT,&spotCutOff);
        spotCutOffSpinner->set_float_limits(0.0,90.0);
        GLUI_Spinner *spotDirSpinner[3];
        spotDirSpinner[0]= glui_window->add_spinner_to_panel(spotPanel,"SpotDirX",GLUI_SPINNER_FLOAT,&spotDir[0]);
        spotDirSpinner[0]->set_float_limits(-1.0,1.0);
        spotDirSpinner[1]= glui_window->add_spinner_to_panel(spotPanel,"SpotDirY",GLUI_SPINNER_FLOAT,&spotDir[1]);
        spotDirSpinner[1]->set_float_limits(-1.0,1.0);
        spotDirSpinner[2]= glui_window->add_spinner_to_panel(spotPanel,"SpotDirZ",GLUI_SPINNER_FLOAT,&spotDir[2]);
        spotDirSpinner[2]->set_float_limits(-1.0,1.0);
        GLUI_Spinner *spotExponentSpinner;
        spotExponentSpinner = glui_window->add_spinner_to_panel(spotPanel,"Spot Exponent",GLUI_SPINNER_FLOAT,&spotExponent);

    glui_window->add_column_to_panel(masterPanel);
    //Material Properties panel
    GLUI_Panel *materialPanel = glui_window->add_panel_to_panel(masterPanel,"Material Properties");
        
        //diffuse Material Properties
        GLUI_Spinner *diffuseMaterial[3];
        diffuseMaterial[0] = glui_window->add_spinner_to_panel(materialPanel,"diffuseR",GLUI_SPINNER_FLOAT,&mat_diffuse[0]);
        diffuseMaterial[0]->set_float_limits(0.0,1.0);
        diffuseMaterial[1] = glui_window->add_spinner_to_panel(materialPanel,"diffuseG",GLUI_SPINNER_FLOAT,&mat_diffuse[1]);
        diffuseMaterial[1]->set_float_limits(0.0,1.0);
        diffuseMaterial[2] = glui_window->add_spinner_to_panel(materialPanel,"diffuseB",GLUI_SPINNER_FLOAT,&mat_diffuse[2]);
        diffuseMaterial[2]->set_float_limits(0.0,1.0);

        glui_window->add_separator_to_panel(materialPanel);

        //Ambient Material Properties
        GLUI_Spinner *ambientMaterial[3];
        ambientMaterial[0] = glui_window->add_spinner_to_panel(materialPanel,"ambientR",GLUI_SPINNER_FLOAT,&mat_ambient[0]);
        ambientMaterial[0]->set_float_limits(0.0,1.0);
        ambientMaterial[1] = glui_window->add_spinner_to_panel(materialPanel,"ambientG",GLUI_SPINNER_FLOAT,&mat_ambient[1]);
        ambientMaterial[1]->set_float_limits(0.0,1.0);
        ambientMaterial[2] = glui_window->add_spinner_to_panel(materialPanel,"ambientB",GLUI_SPINNER_FLOAT,&mat_ambient[2]);
        ambientMaterial[2]->set_float_limits(0.0,1.0);

        glui_window->add_separator_to_panel(materialPanel);

        //Specular Material Properties
        GLUI_Spinner *specularMaterial[3];
        specularMaterial[0] = glui_window->add_spinner_to_panel(materialPanel,"specularR",GLUI_SPINNER_FLOAT,&mat_specular[0]);
        specularMaterial[0]->set_float_limits(0.0,1.0);
        specularMaterial[1] = glui_window->add_spinner_to_panel(materialPanel,"specularG",GLUI_SPINNER_FLOAT,&mat_specular[1]);
        specularMaterial[1]->set_float_limits(0.0,1.0);
        specularMaterial[2] = glui_window->add_spinner_to_panel(materialPanel,"specularB",GLUI_SPINNER_FLOAT,&mat_specular[2]);
        specularMaterial[2]->set_float_limits(0.0,1.0);
        
        glui_window->add_separator_to_panel(materialPanel);

        GLUI_Spinner *shininessMaterial;
        shininessMaterial = glui_window->add_spinner_to_panel(materialPanel,"Shininess",GLUI_SPINNER_FLOAT,&shininess[0]);
        shininessMaterial->set_float_limits(0.0,128.0);

        glui_window->add_separator_to_panel(materialPanel);

        GLUI_Panel *attenuationPanel = glui_window->add_panel_to_panel(materialPanel,"Attenuation Coefficients");
        GLUI_Spinner *attenuationCoeff[3];
        attenuationCoeff[0]= glui_window->add_spinner_to_panel(attenuationPanel,"Constant",GLUI_SPINNER_FLOAT,&kc);
        attenuationCoeff[0]->set_float_limits(0.0,100.0);
        attenuationCoeff[1]= glui_window->add_spinner_to_panel(attenuationPanel,"Linear",GLUI_SPINNER_FLOAT,&kl);
        attenuationCoeff[1]->set_float_limits(0.0,100.0);
        attenuationCoeff[2]= glui_window->add_spinner_to_panel(attenuationPanel,"Quadratic",GLUI_SPINNER_FLOAT,&kq);
        attenuationCoeff[2]->set_float_limits(0.0,10.0);

        GLUI_Panel *modelPanel = glui_window->add_panel_to_panel(materialPanel,"Select Model");
        GLUI_RadioGroup *modelRadio = glui_window->add_radiogroup_to_panel(modelPanel,&modelType);
        glui_window->add_radiobutton_to_group(modelRadio, "Teapot");
        glui_window->add_radiobutton_to_group(modelRadio, "Sphere");
        glui_window->add_radiobutton_to_group(modelRadio, "Torus");
        glui_window->add_radiobutton_to_group(modelRadio, "Cube");

    GLUI_Panel * shaderPanel = glui_window->add_panel("Shader Type");
        GLUI_RadioGroup *shaderTypeRadioGroup = glui_window->add_radiogroup_to_panel(shaderPanel,&shaderType,SHADERTYPE_RADIOGROUP,glui_callback);
        glui_window->add_radiobutton_to_group(shaderTypeRadioGroup,"OpenGL Simulation Directional Light");
        glui_window->add_radiobutton_to_group(shaderTypeRadioGroup,"Per Pixel Directional Light");
        glui_window->add_radiobutton_to_group(shaderTypeRadioGroup,"Per Pixel Point Light");
        glui_window->add_radiobutton_to_group(shaderTypeRadioGroup,"Per Pixel Spot Light");
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
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    mainWindow=glutCreateWindow("OpenGL Lighting Simulator");

    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processNormalKeys);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0,1.0,1.0,1.0);
//	glEnable(GL_CULL_FACE);
    init();

    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        cout<<"Ready for OpenGL 2.0\n";
    else {
        cout<<"OpenGL 2.0 not supported\n";
        //exit(1);
    }
    setupGLUI ();
    setShaders();
    glutMainLoop();
    return 0;
}

