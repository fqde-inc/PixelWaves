
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 WorldPosition;
out vec3 WorldNormal;

out vec2 ReflectedTexCoord;
out vec2 TexCoord;
out vec2 DepthTexCoord;

out vec4 ClipSpace;

uniform mat4 MirrorViewMatrix;

uniform mat4 DepthWorldMatrix;

uniform mat4 ViewProjMatrix;
uniform mat4 WorldMatrix;
uniform mat4 WorldViewMatrix;
uniform mat4 WorldViewProjMatrix;

uniform float Height;
uniform float Speed;
uniform float Time;
uniform	float Amplitude;
uniform	float Wavelength;

void main()
{
    // Output the vertex position
	WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
    
	// Add water height
	float k = 2 * 3.14f / Wavelength;
	WorldPosition.y += Height + ( Amplitude * sin(k * ( WorldPosition.x - Time * Speed )) );

    // World normal
	WorldNormal = ( WorldViewMatrix * vec4(VertexNormal, 0.0)).xyz;

    // Output vertex position
	gl_Position = WorldViewProjMatrix * vec4( WorldPosition, 1.0 );
    
    // Output base tex coord
    TexCoord = VertexTexCoord;
    
    // Transform the vertex position to texture coordinates by multiplying it with the texture matrix
    vec4 reflectCoord = MirrorViewMatrix * vec4(WorldPosition, 1.0);

    // Output the reflection texture coordinates
    ReflectedTexCoord = (reflectCoord.xy / gl_Position.w) / 2.0 + 0.5;
    
    // Output the depth texture coordinates
	//DepthTexCoord = (gl_Position.xy / gl_Position.w) * 0.5f + 0.5f;
    DepthTexCoord = (reflectCoord.xy / gl_Position.w) / 2 + 0.5f;

}
