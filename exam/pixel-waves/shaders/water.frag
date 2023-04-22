
in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

// Water texture
uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec2 ColorTextureScale;

// Scene texture
uniform vec4 SceneColor;
uniform sampler2D SceneTexture;

void main()
{
	float pixelationLevel = 128.0f;

	//pixelate    
	//vec2 roundedTexCoord = floor(TexCoord * pixelationLevel) / pixelationLevel;
	
	// Default
	//FragColor = Color * texture( ColorTexture, roundedTexCoord * ColorTextureScale);

	// Reflection

	vec4 SceneReflection = texture( SceneTexture, TexCoord );
	
	FragColor = Color * SceneReflection;

	 // Flip

	//v_texcoord = vec2(a_texcoord.s, 1.0 - a_texcoord.t);

	// Compose
	//FragColor = Color * texture( ColorTexture, TexCoord * ColorTextureScale) * SceneReflection;
}
