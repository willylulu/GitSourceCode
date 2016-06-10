#version 430

in vec3 vv3normal;
in vec2 vv2coordinate;
in vec3 position;

uniform sampler2D s;
uniform int toggler;

layout(location = 0) out vec4 fragColor;

void main()
{
	if(toggler == 0)
		fragColor = vec4(vv3normal, 1.0);
	else{
		vec4 color = vec4(texture(s, vv2coordinate));
		if(color.a < 0.8 )
			discard;
		else
			fragColor = vec4(vec3(position.y),1.0);
	}
	
}