#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 WorldPosition;
out vec3 WorldNormal;
out vec2 TexCoord;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

uniform float Speed;
uniform float Time;
uniform	float Amplitude;
uniform	float Wavelength;

void main()
{
	
	WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;

	float k = 2 * 3.14f / Wavelength;
	WorldPosition.y += Amplitude * sin(k * ( WorldPosition.x - Time * Speed ));

	WorldNormal = (WorldMatrix * vec4(VertexNormal, 0.0)).xyz;
	TexCoord = VertexTexCoord;
	gl_Position = ViewProjMatrix * vec4(WorldPosition, 1.0);
}
