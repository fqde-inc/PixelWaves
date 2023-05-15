
in vec2 TexCoord;
in vec4 ReflectedTexCoord;
in vec4 DepthTexCoord;

out vec4 FragColor;

// Camera planes
float near = 0.01; 
float far  = 100.0; 

// Foam treshold
float zTreshold = 0.0001;

// Water uniforms
uniform sampler2D ColorTexture;
uniform sampler2D DepthSampler;
uniform sampler2D SceneTexture;

uniform vec4 Color;

uniform float Height;
uniform float Time;
uniform float DistortionStrength;
uniform float DistortionFrequency;
uniform float DistortionSpeed;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    // Transform reflection coords to NDC
    vec2 reflectedTexCoord = (ReflectedTexCoord.xy / ReflectedTexCoord.w) / 2.0 + 0.5;
    
    // Transform the depth texture coordinates to NDC
    vec2 depthTexCoord = (DepthTexCoord.xy / DepthTexCoord.w) / 2 + 0.5f;

	// Linearize Z-values
	float sceneDepth	= LinearizeDepth ( texture(DepthSampler, depthTexCoord.xy).r ) / far;
	float depth			= LinearizeDepth ( gl_FragCoord.z ) / far;

	float depthDifference = sceneDepth - depth;

	if( depthDifference <= zTreshold ) {
		FragColor = vec4(Color.rgb , 1.0) + vec4(0.3f);
		return;
	}

	// Water Tex
	vec4 waterSample = Color * texture( ColorTexture, TexCoord * 0.15f );

	// Reflections
	// Calculate distortion offset
	float horizontalOffset = cos( Time * DistortionSpeed + gl_FragCoord.y * DistortionFrequency) * DistortionStrength;
	reflectedTexCoord.x += horizontalOffset;

	vec4 SceneReflection = texture(SceneTexture, reflectedTexCoord);

	// Replace black pixel by water color
	if( SceneReflection.rgb == vec3(0))
		FragColor = Color;
	else 
		FragColor = Color / 5.0f + vec4(SceneReflection.rgb, 0.85f);
}
