#include<gl/glew.h>
#include "abShaders.h"

abShader::abShader(char *vert, char *pixel)
{
	vertexShaderFile=vert;
	pixelShaderFile=pixel;
}

void abShader::abCreate()
{
	glewInit();
	if (glewIsSupported("GL_VERSION_2_0"))
		std::cout<<"Ready for OpenGL 2.0\n";
	else {
		std::cout<<"OpenGL 2.0 not supported\n";
	}
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
}
GLuint abShader::abCompileAndLink()
{
	char *vs = NULL,*fs = NULL;

	vs = abTextFileRead(vertexShaderFile);
	fs = abTextFileRead(pixelShaderFile);

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);

	delete [] vs; delete [] fs;

	glCompileShader(v);
	glCompileShader(f);

	abPrintShaderInfoLog(v);
	abPrintShaderInfoLog(f);

	programID = glCreateProgram();
	glAttachShader(programID,v);
	glAttachShader(programID,f);

	glLinkProgram(programID);
	abPrintProgramInfoLog(programID);
	//glUseProgram(programID);

	return programID;
}
void abShader::abPrintShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::cout<<infoLog;
        free(infoLog);
    }
}

void abShader::abPrintProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::cout<<infoLog;
        free(infoLog);
    }
}



char * abShader::abTextFileRead(char *fn) {

	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL)
	{
		if(!(fp = fopen(fn,"rt")))
			std::cout<<"\nFollowing Shader File failed to open: \n"<<fn<<"\n";
		else 
		{
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
	else
	{
		std::cout<<"\n Null Path Provided";
	}
	return content;
}

void abShader::abUseProgram()
{
	glUseProgram(programID);
}

void abShader::abSetUniformTexture(char *name, int unit)
{  
	glUseProgram(programID);
	GLint parameterLocation = glGetUniformLocation(programID, name);
	glUniform1i(parameterLocation, unit);
	glUseProgram(0);
}
void abShader::abSetUniformVariable(char *name, float value)
{  
	glUseProgram(programID);
	GLint parameterLocation = glGetUniformLocation(programID, name);
	glUniform1f(parameterLocation, value);
	glUseProgram(0);
}