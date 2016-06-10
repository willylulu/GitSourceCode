#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    gl_ClipDistance[0] = gl_in[0].gl_ClipDistance[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    gl_ClipDistance[0] = gl_in[1].gl_ClipDistance[0];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    gl_ClipDistance[0] = gl_in[2].gl_ClipDistance[0];
    EmitVertex();

    EndPrimitive();	
}