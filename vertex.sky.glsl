#version 410
layout(location = 0) in vec3 Vertex;

uniform vec3 CameraPosition;
uniform mat4 MVP;

out vec3 Position;

void main()
{
    Position = Vertex.xyz;
    gl_Position = MVP * vec4(Vertex.xyz + CameraPosition, 1.0);
}