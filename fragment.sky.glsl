#version 410
#extension GL_NV_shadow_samplers_cube : enable
uniform samplerCube SkyTexture;

in vec3 Position;

void main()
{
    gl_FragColor = textureCube(SkyTexture, Position);
}