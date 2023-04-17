
in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec2 ColorTextureScale;

void main()
{
	float pixelationLevel = 240.0f;

	//pixelate    
	vec2 roundedTexCoord = floor(TexCoord * pixelationLevel) / pixelationLevel;

	FragColor = Color * texture(ColorTexture, roundedTexCoord * ColorTextureScale);
}
