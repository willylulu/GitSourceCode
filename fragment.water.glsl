#version 420 core                                                              
#extension GL_NV_shadow_samplers_cube : enable  
layout(binding = 0)uniform sampler2D dudvmap;   
layout(binding = 1)uniform sampler2D reflectMap;
layout(binding = 2)uniform sampler2D refrectMap;
layout(location = 0) out vec4 fragColor;

in vec4 clipspace;
in vec2 texCoord;
in vec4 viewspace;
uniform vec3 eyeposition;   
uniform float moveFactor; 

const vec4 fogColor = vec4(0.5, 0.5,0.5,1.0);
float fogFactor = 0;
float fogDensity = 0.2;
float fog_start = 1;
float fog_end = 6.0f;

void main(void)                                                                
{
	vec4 tmp = vec4(1.0 / clipspace.w);
    vec4 projCoord = clipspace * tmp;
    projCoord += vec4(1.0);
    projCoord *= vec4(0.5);
    vec2 distort1 = (texture(dudvmap,vec2(projCoord.x*2+moveFactor,-projCoord.y*2)).rg*2-1)*0.01;
    vec2 distort2 = (texture(dudvmap,vec2(-projCoord.x*2+moveFactor,-projCoord.y*2+moveFactor)).rg*2-1)*0.01;
    projCoord.x += distort1.x + distort2.x;
    projCoord.y += distort1.y + distort2.y;
    projCoord = clamp(projCoord, 0.01, 0.99);

    vec4 reflectPart = texture(reflectMap,vec2(projCoord.x,-projCoord.y));
    vec4 refrectPart = texture(refrectMap,vec2(projCoord.x,projCoord.y));

    float dist = length(viewspace);
    fogFactor = 1.0 /pow(exp(dist * fogDensity),0.05);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    vec4 finalColor = reflectPart*0.6+refrectPart*0.4;
    fragColor = mix(fogColor,finalColor,fogFactor);
}                        