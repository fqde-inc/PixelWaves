
in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

// Water texture
uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec2 ColorTextureScale;

uniform mat4 WorldMatrix;
uniform mat4 WorldViewMatrix;
uniform mat4 WorldViewProjMatrix;

uniform mat4 ViewProjMatrix;
uniform mat4 InvViewMatrix;
uniform mat4 InvProjMatrix;

// Scene texture
uniform sampler2D SceneTexture;

void main()
{

	// Transform the texture coordinates using the inverse view and projection matrices
    vec4 clipSpace = ViewProjMatrix * vec4(TexCoord, 0.0, 1.0);
    vec2 ndc = clipSpace.xy / clipSpace.w;
    vec2 reflectedTexCoord = ndc * 0.5 + 0.5; // Convert to [0,1] range

	// Reflection
	reflectedTexCoord = vec2(reflectedTexCoord.x, 1.0f - reflectedTexCoord.y);

	// Water Tex
	vec4 waterSample = vec4(Color.rgb, 0.1f) * texture( ColorTexture, TexCoord * ColorTextureScale);


	vec4 SceneReflection = texture(SceneTexture, reflectedTexCoord);

	// Compose
	FragColor = SceneReflection;
}
