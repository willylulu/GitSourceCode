#version 420 core                                                
                                                                 
layout(location = 0) in vec3 iv3vertex;

uniform mat4 um4mvp; 
uniform mat4 um4v; 
out vec4 clipspace;
out vec4 viewspace;                                                                  
                                                                 
void main(void)                                                  
{
	viewspace = um4v*vec4(iv3vertex,1.0);
	clipspace = um4mvp*vec4(iv3vertex,1.0);                                      
    gl_Position = clipspace;
}		