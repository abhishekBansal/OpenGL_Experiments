//varying vec4 diffuse,ambient;
//varying vec3 normal,lightDir,halfVector;
//varying vec3 normal;

void main()
{	
 /* first transform the normal into eye space and normalize the result */
	//	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	//gl_TexCoord[0] = gl_MultiTexCoord0;			
	//gl_Position = gl_Vertex;	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
//	gl_Position = ftransform();
} 