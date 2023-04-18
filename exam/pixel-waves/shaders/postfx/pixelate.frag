//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D SourceTexture;

void main()
{
	float pixelationLevel = 480.0f;

	//pixelate    
	vec2 roundedTexCoord = floor(TexCoord * pixelationLevel) / pixelationLevel;

	FragColor = texture(SourceTexture, roundedTexCoord);
}
