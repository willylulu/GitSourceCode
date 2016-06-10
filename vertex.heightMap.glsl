#version 430

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 iv3normal;
layout(location = 2) in vec2 iv2coordinate;

uniform mat4 um4mvp;
uniform mat4 um4m;
uniform mat4 um4v;

out vec3 vv3normal;
out vec2 vv2coordinate;
out vec2 texCoord;
out vec3 position;

void main()
{
	vv3normal = iv3normal;
	vv2coordinate = iv2coordinate;
	position = (um4m * vec4(iv3vertex, 1.0)).xyz;
	gl_Position = um4mvp*um4m * vec4(iv3vertex, 1.0);
}