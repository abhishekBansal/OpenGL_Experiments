uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D ice;
uniform sampler2D rock;

uniform float maxHeight; 
uniform float minHeight;
varying float height;
void main()
{
	vec3 sandColor	= texture2D(sand,gl_TexCoord[0].st).rgb;
	vec3 grassColor	= texture2D(grass,gl_TexCoord[1].st).rgb;
	vec3 iceColor	= texture2D(ice,gl_TexCoord[2].st).rgb;
	vec3 rockColor	= texture2D(rock,gl_TexCoord[3].st).rgb;
	
	float maxHeight1 = 0.0;
	float lowerBound = 0.0;
	float height1 = 0.0;
	if(minHeight < 0)
	{ 
		lowerBound = abs(minHeight);
		maxHeight1 = maxHeight + lowerBound;
		height1 = height + lowerBound;
	}
	else if (minHeight > 0)
	{
		maxHeight1=maxHeight-minHeight;
		height1 = height - lowerBound;
	}
	else
	{
		maxHeight1=maxHeight;
		height1 = height;
	}
		
	float interval = maxHeight1/3.0;
	
	float grassWeight = clamp(1.0 -(abs(height1 - 0.0)/interval),0.0,1.0);	
	float sandWeight = clamp(1.0 -(abs(height1 -  interval)/interval),0.0,1.0);
	float rockWeight = clamp(1.0 -(abs(height1 -  (2.0 * interval))/interval),0.0,1.0);
	float iceWeight =clamp(1.0 -(abs(height1 - maxHeight1 * .8)/interval),0.0,1.0);	
	
	
	//Calculate Normalized Weights
	float totalWeight = grassWeight + sandWeight + iceWeight + rockWeight;
	
	grassWeight = grassWeight/totalWeight;
	sandWeight = sandWeight/totalWeight;
	iceWeight = iceWeight/totalWeight;
	rockWeight= rockWeight / totalWeight;
	//...........................//
	
	gl_FragColor = vec4((grassWeight * grassColor.rgb + sandWeight * sandColor.rgb + rockWeight * rockColor.rgb + iceWeight * iceColor.rgb),1.0);
}