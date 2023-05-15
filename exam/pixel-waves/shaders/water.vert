
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 WorldPosition;
out vec3 WorldNormal;

out vec4 ReflectedTexCoord;
out vec2 TexCoord;
out vec4 DepthTexCoord;

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
	WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
    
	// Add water height and wave height for a small wavy effect
	float k = 2 * 3.14f / Wavelength;
	WorldPosition.y += Height + ( Amplitude * sin(k * ( WorldPosition.x - Time * Speed )) );

    // World normal
	// WorldNormal = ( WorldViewMatrix * vec4(VertexNormal, 0.0)).xyz;

    // Output vertex position
	gl_Position = WorldViewProjMatrix * vec4( WorldPosition, 1.0 );
    
    // Output base tex coord
    TexCoord = VertexTexCoord;
    
    // Transform the vertex position to texture coordinates by multiplying it with the texture matrix
    ReflectedTexCoord = MirrorViewMatrix * vec4(WorldPosition, 1.0);
    DepthTexCoord = ReflectedTexCoord;
    
    // TODO : Use copy of scene texture, and recalculate coordinates
    //DepthTexCoord = ViewProjMatrix * vec4(WorldPosition, 1.0);

}
