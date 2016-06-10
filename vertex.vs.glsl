#version 430

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 iv3normal;
layout(location = 2) in vec2 iv2coordinate;

uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

out vec3 vv3normal;
out vec2 vv2coordinate;
out vec4 viewSpace;

void main()
{
	viewSpace = vMatrix* mMatrix * vec4(iv3vertex, 1.0);
	gl_Position = pMatrix * vMatrix * mMatrix* vec4(iv3vertex, 1.0);
	vv3normal = iv3normal;
	vv2coordinate = iv2coordinate;
}