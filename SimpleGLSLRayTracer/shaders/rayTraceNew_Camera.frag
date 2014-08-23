#ifdef GL_ES
precision highp float;
#endif

#define SPHERE1 1
//#define SPHERE99 99 //center sphere
#define SPHERE3 3
#define SPHERE4 4
#define SPHERE5 5
#define PLANE1 6
#define LEFTWALL 7
#define RIGHTWALL 8
#define BACKWALL 9

struct materialProperties
{
  vec4 specular;
  vec4 diffuse;
  vec4 ambient;
  float shininess;
};
struct sphere {
  float radius;
  vec3 center;
  materialProperties matProp;
};
struct plane {
  vec3 normal;
  vec4 eq;
  materialProperties matProp;
};  

// define primitives
sphere sphere1, /*sphere99*/ sphere3, sphere4, sphere5;
plane plane1,backWall,leftWall,rightWall;  

uniform sampler2D tex0;
uniform float time;

vec4 lmodel_ambient = vec4( 0.2, 0.2, 0.2, 1.0 );
//vec4 mat_ambient = vec4( 0.7, .2, .2, 1.0 );
//vec4 mat_diffuse = vec4( 1.0, 0.0, 0.0, 1.0 );
//vec4 mat_specular = vec4(1.0,1.0,1.0,1.0);
//eye position
vec3 camera = vec3(0.0,10.0,50.0);
//screen plane coordinates
vec3 p1 = vec3(-5.0,3.0,49.0);
vec3 p2 = vec3(5.0,3.0,49.0);
vec3 p3 = vec3(5.0,-3.0,49.0);
vec3 p4 = vec3(-5.0,-3.0,49.0);
//float shininess = 20.0;
vec4 specular = vec4( 0.3, 0.3, 0.3, 1.0 );
vec4 ambient = vec4( 0.8, 0.5, 0.1, 1.0 );
vec4 diffuse = vec4( .5, 0.5, 0.5, 1.0 );

vec3 lPos = vec3( 10.0, 30.0, 5.0 );

//parameters for five spheres sphere99 = centerSphere
vec3 center1 = vec3 (30.0 *sin(time),0.0, 30.0*cos (time));
float radius1 = 7.0;

vec3 center2 = vec3 (0.0,0.0,0.0);
float radius2 = 12.0;

vec3 center3 = vec3 (-30.0 *sin(time),0.0, -30.0*cos (time));
float radius3= 7.0;

vec3 center4 = vec3 (30.0 *sin(time),14.0, -30.0*cos (time));
float radius4= 7.0;

vec3 center5 = vec3 (-30.0 *sin(time),14.0, 30.0*cos (time));
float radius5= 7.0;

//...........................//

//walls and floors
vec4 planeEq = vec4 (0.0,1.0,0.0,10.0);
vec3 planeNormal = vec3 (0.0,1.0,0.0);

vec4 backWallEq = vec4 (0.0,0.0,1.0,40.0);
vec3 backWallNormal = vec3 (0.0,0.0,1.0);

vec4 leftWallEq = vec4 (1.0,0.0,0.0,40.0);
vec3 leftWallNormal = vec3 (1.0,0.0,0.0);

/*vec4 rightWallEq = vec4 (1.0,0.0,0.0,-1.0);
vec3 rightWallNormal = vec3 (-1.0,0.0,0.0);*/
//.............................//

float calcShadow (vec3 point,vec3 rayDirection, int id); //fwd declaration
vec3 computeLight(vec3 iPoint, vec3 iNormal, int id, vec4 mat_ambient, vec4 mat_diffuse,vec4  mat_specular, float shininess)
{
	vec3 normal,lightDir,halfVector;
	vec3 n,halfV,viewV,ldir;
	float NdotL,NdotHV;
	vec4 diffuse1, ambient1;
	
	lightDir = normalize(lPos- iPoint);
	
	viewV = normalize (camera - iPoint);
	halfVector = normalize(lightDir + viewV);
	 
        float shadowFactor = calcShadow (iPoint,lPos, id);
  	

	/* Compute the diffuse, ambient and globalAmbient terms */
	diffuse1 = mat_diffuse* diffuse ;
	ambient1 = mat_ambient *ambient;
	ambient1 += lmodel_ambient * mat_ambient;
	
  	diffuse1 = shadowFactor * diffuse1 ;
        

	/* a fragment shader can't write a verying variable, hence we need
	a new variable to store the normalized interpolated normal */
	n = normalize(iNormal);
	
	vec4 color = ambient1;
	
	/* compute the dot product between normal and ldir */
	NdotL = max(dot(n,lightDir),0.0);

	if (NdotL > 0.0) {
	//	halfV = normalize(halfVector);
		NdotHV = max(dot(n,halfVector),0.0);
		color += mat_specular * specular * pow(NdotHV,shininess) * shadowFactor;
		color += diffuse1 * NdotL;
	}
	return color.rgb;
}
	

int sphereIntersect(vec3 rayDir, vec3 rayOrigin, out float t1, in vec3 sCenter, in float sRadius)
{
	t1=1000000.0;
	rayDir = normalize(rayDir);
	float B = 2.0 *( ( rayDir.x * (rayOrigin.x - sCenter.x ) )+  ( rayDir.y * (rayOrigin.y - sCenter.y )) + ( rayDir.z * (rayOrigin.z - sCenter.z ) ));
	float C = pow((rayOrigin.x - sCenter.x),2.0) + pow((rayOrigin.y - sCenter.y),2.0) + pow((rayOrigin.z - sCenter.z),2.0) - pow(sRadius,2.0);
	
	float D = B*B - 4.0*C ;
	
	if(D>0.0)
	{
		t1= (-B - pow(D, .5)) / 2.0;
		if (t1 > 0.0)
		{
		        return 1;  
			 
		}
		
	}
	return 0; //since determinant of quadratic equation <0 so no solution and hence no intersection
}
int planeIntersect(vec3 rayDir, vec3 rayOrigin,vec4 eq, vec3 norm, out float t1)
{
	rayDir= normalize(rayDir);
	float b = dot(norm, rayDir );
	if(b <= 0.0) // b>0 means plane is away from the ray
	{
        float a = - dot(rayOrigin,norm) - eq.w;
        t1 = a/b ;
        if(t1 >= 0.0) //t <0 means that plane ray intersection is behind the camera
			return  1;
        else 
           	return 0;
	}
	else
    {
       	t1= 0.0;
		return 0; //no intersection or intersection behind camera
    }
}


int checkIntersection(out float t1,in vec3 rayOrigin,  in vec3 rayDirection)
{
	float tmin = 100000.0;
	t1=0.0;
	int id=0;
	rayDirection = normalize (rayDirection);
	if(sphereIntersect(rayDirection,rayOrigin, t1, sphere1.center, sphere1.radius) == 1)
	{
		if(t1 < tmin)
		{
			id = SPHERE1;
			tmin = t1; 
		}
	}
	
	if(sphereIntersect(rayDirection,rayOrigin, t1, sphere3.center, sphere3.radius) == 1)
	{
		if(t1 < tmin)
		{
			id = SPHERE3;
			tmin = t1; 
		}
	}
/*	if(sphereIntersect(rayDirection,rayOrigin, t1, sphere99.center, sphere99.radius) == 1)
	{
		if(t1 < tmin)
		{
			id = SPHERE99;
			tmin = t1; 
		}
	}*/
	if(sphereIntersect(rayDirection,rayOrigin, t1, sphere4.center, sphere4.radius) == 1)
	{
		if(t1 < tmin)
		{
			id = SPHERE4;
			tmin = t1; 
		}
	}
	if(sphereIntersect(rayDirection,rayOrigin, t1, sphere5.center, sphere5.radius) == 1)
	{
		if(t1 < tmin)
		{
			id = SPHERE5;
			tmin = t1; 
		}
	}
	if(planeIntersect(rayDirection,rayOrigin,plane1.eq, plane1.normal, t1) == 1)
	{
		if(t1 < tmin)
		{
			id= PLANE1;
			tmin = t1; 
		}
	}
	if(planeIntersect(rayDirection,rayOrigin, leftWall.eq, leftWall.normal, t1) == 1)
	{
		if(t1 < tmin)
		{
			id= LEFTWALL;
			tmin = t1; 
		}
	}
	/*if(planeIntersect(rayDirection,rayOrigin,rightWall.eq, rightWall.normal, t1) == 1)
	{
		if(t1 < tmin)
		{
			id= RIGHTWALL;
			tmin = t1; 
		}
	}*/
	if(planeIntersect(rayDirection,rayOrigin,backWall.eq, backWall.normal, t1) == 1)
	{
		if(t1 < tmin)
		{
			id= BACKWALL;
			tmin = t1; 
		}
	}
	if (id!=0) 
	{
		t1=tmin;
		return id;
	}
	else return 0;
}


float calcShadow (vec3 point,vec3 rayDirection, int id)
{
	float power = 1.0;
	float t1=0.0;
	if(sphereIntersect(rayDirection,point, t1, sphere1.center, sphere1.radius) == 1 )
	{
		power+=2.0;
	}
	/*if(sphereIntersect(rayDirection,point, t1, sphere99.center, sphere99.radius) == 1 )
	{
			power+=2.0;
	}*/	
	if(sphereIntersect(rayDirection,point, t1, sphere3.center, sphere3.radius) == 1 )
	{
		power+=2.0;
	}
	if(sphereIntersect(rayDirection,point, t1, sphere4.center, sphere4.radius) == 1 )
	{
		power+=2.0;
	}
	if(sphereIntersect(rayDirection,point, t1, sphere5.center, sphere5.radius) == 1 )
	{
		power+=2.0;
	}
	if(planeIntersect(rayDirection,point,plane1.eq,plane1.normal, t1) == 1)
	{
		power+=2.0;
	}
	if(planeIntersect(rayDirection,point,leftWall.eq,leftWall.normal, t1) == 1)
	{
		power+=2.0;
	}
	if(planeIntersect(rayDirection,point,backWall.eq,backWall.normal, t1) == 1)
	{
		power+=2.0;
	}
	return 1.0/power;
}

vec3 calculateNormal (int id, vec3 point) 
{
	vec3 normal;
	if(id==SPHERE1)
	{
		normal.x = (point.x - sphere1.center.x)/sphere1.radius ;
		normal.y = (point.y - sphere1.center.y)/sphere1.radius ;
		normal.z = (point.z - sphere1.center.z)/sphere1.radius ;
	}
	/*else if(id==SPHERE99)
	{
		normal.x = (point.x - sphere99.center.x)/sphere99.radius ;
		normal.y = (point.y - sphere99.center.y)/sphere99.radius ;
		normal.z = (point.z - sphere99.center.z)/sphere99.radius;
	}*/
	else if(id==SPHERE3)
	{
		normal.x = (point.x - sphere3.center.x)/sphere3.radius ;
		normal.y = (point.y - sphere3.center.y)/sphere3.radius ;
		normal.z = (point.z - sphere3.center.z)/sphere3.radius;
	}
	else if(id==SPHERE4)
	{
		normal.x = (point.x - sphere4.center.x)/sphere4.radius ;
		normal.y = (point.y - sphere4.center.y)/sphere4.radius ;
		normal.z = (point.z - sphere4.center.z)/sphere4.radius;
	}
	else if(id==SPHERE5)
	{
		normal.x = (point.x - sphere5.center.x)/sphere5.radius ;
		normal.y = (point.y - sphere5.center.y)/sphere5.radius ;
		normal.z = (point.z - sphere5.center.z)/sphere5.radius;
	}
	else if(id == PLANE1)
	{
		normal = plane1.normal;
	}
	else if(id == LEFTWALL)
	{
		normal = leftWall.normal;
	}
	/*else if(id == RIGHTWALL)
	{
		normal = rightWall.normal;
	}*/
	else if(id == BACKWALL)
	{
		normal = backWall.normal;
	}
	return normal;
}

vec3 assignColor(int id, vec3 iPoint, vec3 iNormal, vec2 p )
{
  if (id == SPHERE1)
  { 	
       	return computeLight(iPoint,iNormal, id,sphere1.matProp.ambient, sphere1.matProp.diffuse,sphere1.matProp.specular ,sphere1.matProp.shininess ); 
  }
  else if(id== SPHERE3)
  {
       	return computeLight(iPoint,iNormal, id,sphere3.matProp.ambient, sphere3.matProp.diffuse,sphere3.matProp.specular ,sphere3.matProp.shininess );
  }
 /* if(id== SPHERE99)
  {
       	return computeLight(iPoint,iNormal, id,sphere99.matProp.ambient, sphere99.matProp.diffuse,sphere99.matProp.specular ,sphere99.matProp.shininess );
  }*/
  else if(id== SPHERE4)
  {
       	return computeLight(iPoint,iNormal, id,sphere4.matProp.ambient, sphere4.matProp.diffuse,sphere4.matProp.specular ,sphere4.matProp.shininess );
  }
  else if(id== SPHERE5)
  {
       	return computeLight(iPoint,iNormal, id,sphere5.matProp.ambient, sphere5.matProp.diffuse,sphere5.matProp.specular ,sphere5.matProp.shininess );
  }
  else if(id==PLANE1)
  { 
		vec3 wTex = vec3(1.0,1.0,0.0);
      //  wTex = texture2D(tex0,p).rgb;
     	return wTex * computeLight(iPoint,iNormal, id,plane1.matProp.ambient, plane1.matProp.diffuse,plane1.matProp.specular ,plane1.matProp.shininess );
  }
  else if(id==LEFTWALL)
  { 
		vec3 wTex = vec3(1.0,0.0,0.0);
        //wTex = texture2D(tex0,p).rgb;
     	return wTex * computeLight(iPoint,iNormal, id,leftWall.matProp.ambient, leftWall.matProp.diffuse,leftWall.matProp.specular ,leftWall.matProp.shininess );
  }
/*  else if(id==RIGHTWALL)
  { 
		vec3 wTex = vec3(0.0,1.0,0.0);
        //wTex = texture2D(tex0,p).rgb;
     	return wTex * computeLight(iPoint,iNormal, id,rightWall.matProp.ambient, rightWall.matProp.diffuse,rightWall.matProp.specular ,rightWall.matProp.shininess );
  }*/
  else if(id==BACKWALL)
  { 
		vec3 wTex = vec3(0.0,0.0,1.0);
        //wTex = texture2D(tex0,p).rgb;
     	return wTex * computeLight(iPoint,iNormal, id,backWall.matProp.ambient, backWall.matProp.diffuse,backWall.matProp.specular ,backWall.matProp.shininess );
  }
  else
    	return vec3(0.0,0.0,0.0);
}

//function to invert a 3x3 matrix

void invert (out mat3 m)
{

}
void main()
{
	//Setting all material properties
	  
  	sphere1.radius = radius1;
  	sphere1.center = center1;
  	sphere1.matProp.ambient = vec4 (0.0,0.0,.3,1.0);
  	sphere1.matProp.diffuse = vec4 (0.0,0.0,.7,1.0);
  	sphere1.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	sphere1.matProp.shininess = 75.0;
	
	/*sphere99.radius = radius2;
  	sphere99.center = center2;
  	sphere99.matProp.ambient = vec4 (0.7,0.7,0.7,1.0);
  	sphere99.matProp.diffuse = vec4 (0.7,0.7,0.7,1.0);
  	sphere99.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	sphere99.matProp.shininess = 128.0;*/
	
	sphere3.radius = radius3;
  	sphere3.center = center3;
  	sphere3.matProp.ambient = vec4 (0.3,0.0,0.0,1.0);
  	sphere3.matProp.diffuse = vec4 (0.7,0.0,0.0,1.0);
  	sphere3.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	sphere3.matProp.shininess = 75.0;
	
	sphere4.radius = radius4;
  	sphere4.center = center4;
  	sphere4.matProp.ambient = vec4 (0.0,0.2,0.6,1.0);
  	sphere4.matProp.diffuse = vec4 (0.0,0.7,0.7,1.0);
  	sphere4.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	sphere4.matProp.shininess = 75.0;
	
	sphere5.radius = radius5;
  	sphere5.center = center5;
  	sphere5.matProp.ambient = vec4 (0.3,0.0,0.5,1.0);
  	sphere5.matProp.diffuse = vec4 (1.0,0.0,0.3,1.0);
  	sphere5.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	sphere5.matProp.shininess = 75.0;
  
   	plane1.eq = planeEq;
  	plane1.normal = planeNormal;
  	plane1.matProp.ambient = vec4 (0.3,0.3,0.3,1.0);
  	plane1.matProp.diffuse = vec4 (1.0,1.0,1.0,1.0);
  	plane1.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	plane1.matProp.shininess = 50.0;
  	
  	leftWall.eq = leftWallEq;
  	leftWall.normal = leftWallNormal;
  	leftWall.matProp.ambient = vec4 (0.3,0.3,0.3,1.0);
  	leftWall.matProp.diffuse = vec4 (1.0,1.0,1.0,1.0);
  	leftWall.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	leftWall.matProp.shininess = 50.0;
  	
  	/*rightWall.eq = rightWallEq;
  	rightWall.normal = rightWallNormal;
  	rightWall.matProp.ambient = vec4 (0.3,0.3,0.3,1.0);
  	rightWall.matProp.diffuse = vec4 (1.0,1.0,1.0,1.0);
  	rightWall.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	rightWall.matProp.shininess = 5.0;*/
  	
  	backWall.eq = backWallEq;
  	backWall.normal = backWallNormal;
  	backWall.matProp.ambient = vec4 (0.3,0.3,0.3,1.0);
  	backWall.matProp.diffuse = vec4 (1.0,1.0,1.0,1.0);
  	backWall.matProp.specular = vec4 (1.0,1.0,1.0,1.0);
  	backWall.matProp.shininess = 50.0;
	
	vec3 target = vec3 (0.0,10.0,-1.0);
	vec3 zaxis = target - camera;
	zaxis = normalize(zaxis);
	vec3 up=vec3( 0, 1, 0 );
	vec3 xaxis = cross( up,zaxis );
	vec3 yaxis = cross( xaxis,-zaxis );
	mat3  m;
	m[0].x = xaxis.x, m[0].y = xaxis.y, m[0].z = xaxis.z;
	m[1].x = yaxis.x, m[1].y = yaxis.y, m[1].z = yaxis.z;
	m[2].x = zaxis.x, m[2].y = zaxis.y, m[2].z = zaxis.z;
	m = inverse(m); //3x3 matrix will be stored in upper left side whereas other elements will be set as if they would have 
						//been in an identity matrix
	mat4 m4 = mat4(m);
	m4[0].w = camera.x, m4[1].w = camera.y, m4[2].w = camera.z;
	
	camera = mat3(m4) * camera;
	p1 = mat3(m4) * p1;
	p2 = mat3(m4) * p2;
	p3 = mat3(m4) * p3;
	p4 = mat3(m4) * p4;
	
  	vec3  xf = p1 + (p2-p1)/500.0 * gl_FragCoord.x;
	vec3  yf = p1 + (p4-p1)/500.0 * gl_FragCoord.y;
	vec2 p =  gl_FragCoord.xy;
    vec3 rayDirection  =   normalize(xf+yf);
	vec3 lightColor;
	
	float t1=0.0; // variable for ray parameter as in r = ro + t* rd
  	vec3 iPoint,iNormal;
  	
    int id = checkIntersection(t1,camera, rayDirection);    
         
  	if( id>0 )
    {	
		iPoint.x = camera.x + rayDirection.x * t1;
		iPoint.y = camera.y + rayDirection.y * t1;
		iPoint.z = camera.z + rayDirection.z * t1;
		
		iNormal.xyz = normalize(calculateNormal (id, iPoint)) ;
        
		lightColor.xyz = assignColor (id, iPoint, iNormal, p)   ;
		gl_FragColor=vec4(lightColor, 1.0);
         
		//calculating reflection vector and color; iteration since recursion is not supported
		vec3 refColor; 
		vec3 ref =reflect (rayDirection,iNormal);
		t1=0.0; 
         
		int id2 = checkIntersection(t1,iPoint, ref);    
		if( id2>0 )
		{
			refColor= vec3(1.0,1.0,1.0);
			iPoint.x = iPoint.x + ref.x * t1;
			iPoint.y = iPoint.y + ref.y * t1;
			iPoint.z = iPoint.z + ref.z * t1;
		
			iNormal.xyz = calculateNormal (id2, iPoint) ;
         
			refColor = assignColor(id2, iPoint, iNormal, p);
			gl_FragColor = vec4(mix(gl_FragColor.rgb, refColor,.35) ,1.0);  	
			
			
			//calculating reflection vector and color; iteration since recursion is not supported
			vec3 refColor2; 
			vec3 ref2 =reflect (ref,iNormal);
			t1=0.0; 
         
			int id3 = checkIntersection(t1,iPoint, ref2);    
			if( id3>0 )
			{
				refColor2= vec3(1.0,1.0,1.0);
				iPoint.x = iPoint.x + ref2.x * t1;
				iPoint.y = iPoint.y + ref2.y * t1;
				iPoint.z = iPoint.z + ref2.z * t1;
		
				iNormal.xyz = calculateNormal (id3, iPoint) ;
         
				refColor2 = assignColor(id3, iPoint, iNormal, p);
				gl_FragColor = vec4(mix(gl_FragColor.rgb, refColor2,.15) ,1.0);  	
			}
		}
         
   }
   else discard;
}
