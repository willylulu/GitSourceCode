#version 430

in vec3 vv3normal;
in vec2 vv2coordinate;
in vec4 viewSpace;

uniform sampler2D s;
uniform int toggler;

layout(location = 0) out vec4 fragColor;

const vec4 fogColor = vec4(0.5, 0.5,0.5,1.0);
float fogFactor = 0;
float fogDensity = 0.2;
float fog_start = 1;
float fog_end = 6.0f;

void main()
{
	if(toggler == 0)
		fragColor = vec4(vv3normal, 1.0);
	else{
		float dist = length(viewSpace);
		fogFactor = 2.0 /pow(exp(dist * fogDensity),0.05);
		fogFactor = clamp( fogFactor, 0.0, 1.0 );
		vec4 color = vec4(texture(s, vv2coordinate));
		if(color.a < 0.8)
			discard;
		else
			fragColor = mix(fogColor,color,fogFactor);
	}
	
}