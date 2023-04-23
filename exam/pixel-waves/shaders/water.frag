
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

uniform mat4 InvViewMatrix;
uniform mat4 InvProjMatrix;

// Scene texture
uniform vec4 SceneColor;
uniform sampler2D SceneTexture;

void main()
{

	float pixelationLevel = 128.0f;


	// Compute view vector en view space
	vec3 viewDir = GetDirection(VertexPosition, vec3(0));

	// Convert position, normal and view vector to world space
	vec3 position = (InvViewMatrix * vec4(VertexPosition, 1)).xyz;
	vec3 normal = (InvViewMatrix * vec4(VertexNormal, 0)).xyz;
	//viewDir = (InvViewMatrix * vec4(viewDir, 0)).xyz;

	// texture coordinates
	//vec3 FlipWorldPoso = viewDir * vec3(1,-1, 1);
	//vec2 SceneTexCoord = vec2(1.0 - FlipWorldPoso.x, 1.0 - FlipWorldPoso.y);


	// Water Tex
	vec2 roundedTexCoord = floor(TexCoord * pixelationLevel) / pixelationLevel;
	vec4 waterSample = vec4(Color.rgb,0.1f) * texture( ColorTexture, roundedTexCoord * ColorTextureScale);

	// Reflection
	vec2 SceneTexCoord = vec2(1.0f - roundedTexCoord.x, 1.0f - roundedTexCoord.y);
	vec4 SceneReflection = texture(SceneTexture, SceneTexCoord);

	// Compose
	FragColor = waterSample + vec4(SceneReflection.rgb, 0.2f);
}
