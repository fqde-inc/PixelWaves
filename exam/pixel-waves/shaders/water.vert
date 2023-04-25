
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 WorldPosition;
out vec3 WorldNormal;
out vec2 TexCoord;

out vec4 ClipSpace;

uniform mat4 ViewProjMatrix;
uniform mat4 WorldMatrix;
uniform mat4 WorldViewMatrix;
uniform mat4 WorldViewProjMatrix;

uniform float Speed;
uniform float Time;
uniform	float Amplitude;
uniform	float Wavelength;

void main()
{
	WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;

	// Up down
	float k = 2 * 3.14f / Wavelength;
	WorldPosition.y += Amplitude * sin(k * ( WorldPosition.x - Time * Speed ));

	WorldNormal = (WorldViewMatrix * vec4(VertexNormal, 0.0)).xyz;
	
	//TexCoord = VertexTexCoord;
	//ClipSpace = ViewProjMatrix * vec4( WorldPosition, 1.0 );

	//gl_Position = WorldViewProjMatrix * vec4(WorldPosition, 1.0) ;
	//gl_Position = ViewProjMatrix * vec4( WorldPosition, 1.0 );




	// Calculate the texture matrix by post-multiplying compositeMatrix with the inverse of pixelSize
    mat4 textureMatrix = WorldViewProjMatrix * (1.0f/4.0f);
    
    // Transform the vertex position to texture coordinates by multiplying it with the texture matrix
    vec4 texCoords = textureMatrix * vec4(VertexPosition, 1.0);
    
    // Output the texture coordinates
    TexCoord = texCoords.xy;
    
    // Output the vertex position
	gl_Position = WorldViewProjMatrix * vec4( WorldPosition, 1.0 );

}
