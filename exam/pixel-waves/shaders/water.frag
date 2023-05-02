
in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

in vec4 ReflectedTexCoord;
//in vec2 DepthTexCoord;

in vec4 gl_FragCoord;

out vec4 FragColor;

// Water texture
uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform sampler2D DepthSampler;

uniform vec2 ColorTextureScale;

uniform mat4 WorldMatrix;
uniform mat4 WorldViewMatrix;
uniform mat4 WorldViewProjMatrix;

uniform mat4 ViewProjMatrix;
uniform mat4 InvViewMatrix;
uniform mat4 InvProjMatrix;

// Scene texture
uniform sampler2D SceneTexture;
uniform float Height;

float near = 0.01; 
float far  = 1000.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{

    // Output the reflection texture coordinates
    vec2 reflectedTexCoord = (ReflectedTexCoord.xy / ReflectedTexCoord.w) / 2.0 + 0.5;
    
    // Output the depth texture coordinates
    vec2 DepthTexCoord = (ReflectedTexCoord.xy / ReflectedTexCoord.w) / 2 + 0.5f;

	float sceneDepth	= LinearizeDepth ( texture(DepthSampler, DepthTexCoord).r ) / far;
	float depth			= LinearizeDepth ( gl_FragCoord.z ) / far;
	
	float depthDifference = sceneDepth - depth;

	if( depthDifference <= 0 ) {
		//FragColor = vec4(Color.rgb , 1.0) + vec4(0.3f);
		//return;
	}

	// Water Tex
	vec4 waterSample = Color * texture( ColorTexture, TexCoord * 0.15f );

	// Reflections
	vec4 SceneReflection = texture(SceneTexture, reflectedTexCoord );

	if(SceneReflection.r <= 0.0 && SceneReflection.g <= 0.0 && SceneReflection.b <= 0.0 )
		FragColor = Color;
	else 
		FragColor = Color / 5.0f + vec4(SceneReflection.rgb, 0.85f);
}
