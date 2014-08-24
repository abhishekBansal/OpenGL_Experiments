#ifndef _ABSHADERS_H
#define _ABSHADERS_H

#include <iostream>
#include <gl/glew.h>
#include <gl/glut.h>


class abShader
{
private:
	GLuint programID;
	GLuint v;
	GLuint f;
	char* vertexShaderFile;
	char* pixelShaderFile;
public:
	abShader(char *vert,char *pixel);
	GLuint abCompileAndLink();
	void abPrintShaderInfoLog(GLuint obj);
	void abPrintProgramInfoLog(GLuint obj);
	char *abTextFileRead(char *fn);
	void abUseProgram();
	void abCreate();
	void abSetUniformTexture(char *name, int unit);
	void abSetUniformVariable(char *name, float value);
//	~abShader();
};

#endif //_ABSHADERS_H


