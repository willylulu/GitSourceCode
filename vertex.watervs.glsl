#version 430

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 iv3normal;
layout(location = 2) in vec2 iv2coordinate;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;

uniform int toggler;

out vec3 vv3normal;
out vec2 vv2coordinate;
out vec2 texCoord;
out vec3 position;

void main()
{
	vv3normal = iv3normal;
	vv2coordinate = iv2coordinate;
	position = (mMatrix * vec4(iv3vertex, 1.0)).xyz;
	if(toggler==0)gl_ClipDistance[0] = position.y-25;
	else
	{
		float isBound = -1000;
		if(183>position.x&&position.x>-72&&80>position.z&&position.z>-130)isBound = 1000;
		gl_ClipDistance[0] = min(-position.y+25,isBound);
	} 
	gl_Position = vpMatrix * mMatrix * vec4(iv3vertex, 1.0);
}