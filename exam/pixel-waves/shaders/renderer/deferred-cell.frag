//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D DepthTexture;
uniform sampler2D AlbedoTexture;
uniform sampler2D NormalTexture;
uniform sampler2D OthersTexture;
uniform mat4 InvViewMatrix;
uniform mat4 InvProjMatrix;

void main()
{
	// Extract information from g-buffers
	vec3 position = ReconstructViewPosition(DepthTexture, TexCoord, InvProjMatrix);
	vec3 albedo = texture(AlbedoTexture, TexCoord).rgb;
	vec3 normal = GetImplicitNormal(texture(NormalTexture, TexCoord).xy);
	vec4 others = texture(OthersTexture, TexCoord);

	// Compute view vector en view space
	vec3 viewDir = GetDirection(position, vec3(0));

	// Convert position, normal and view vector to world space
	position = (InvViewMatrix * vec4(position, 1)).xyz;
	normal = (InvViewMatrix * vec4(normal, 0)).xyz;
	viewDir = (InvViewMatrix * vec4(viewDir, 0)).xyz;

	// Set surface material data
	SurfaceData data;
	data.normal = normal;
	data.albedo = albedo;
	data.ambientOcclusion = others.x;
	data.roughness = others.y;
	data.metalness = others.z;

	// Compute lighting
	vec3 lighting = ComputeLighting(position, data, viewDir, true);
	//FragColor = vec4(lighting, 1.0f);
	FragColor = others;
}








in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;

uniform float AmbientReflection;
uniform float DiffuseReflection;
uniform float SpecularReflection;
uniform float SpecularExponent;

uniform vec3 AmbientColor;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

// Toon settings
uniform vec3 Intensity;

vec3 GetAmbientReflection(vec3 objectColor)
{
	return AmbientColor * AmbientReflection * objectColor;
}

vec3 GetDiffuseReflection(vec3 objectColor, vec3 lightVector, vec3 normalVector)
{
	return LightColor * DiffuseReflection * objectColor * max(dot(lightVector, normalVector), 0.0f);
}

float GetSpecularIntensity(float lightIntensity, vec3 lightVector, vec3 viewVector, vec3 normalVector)
{
	vec3 halfVector = normalize(lightVector + viewVector);
	float NdotH = dot(normalVector, halfVector);

	return pow( NdotH * lightIntensity, SpecularExponent * SpecularExponent);
}

vec3 GetBlinnPhongReflection(vec3 objectColor, vec3 lightVector, vec3 viewVector, vec3 normalVector)
{
	float NdotL = dot(lightVector, normalVector);
	float lightIntensity = smoothstep(0, 0.01, NdotL);

	vec3 light = lightIntensity * LightColor;
	
	float specularIntensity = GetSpecularIntensity(lightIntensity, lightVector, viewVector, normalVector);
	vec3 smoothSpecular = smoothstep(0.005, 0.01, specularIntensity) * vec3(1.0f);

	float _RimAmount = .9f;
	float _RimThreshold = 0.1f;
	vec3 _RimColor = vec3(1.0f);

	float rimDot = 1.0f - dot(viewVector, normalVector);
	float rimIntensity = smoothstep(_RimAmount - 0.01, _RimAmount + 0.01, rimDot);
	vec3 rim = rimIntensity * _RimColor;

	return objectColor * ( AmbientColor + light + smoothSpecular + rim );

}

void main()
{

	vec4 objectColor = Color * texture(ColorTexture, TexCoord);
	vec3 lightVector = normalize(LightPosition - WorldPosition);
	vec3 viewVector = normalize(CameraPosition - WorldPosition);
	vec3 normalVector = normalize(WorldNormal);

	// cartoon
	FragColor = vec4( GetBlinnPhongReflection(objectColor.rgb, lightVector, viewVector, normalVector), 1.0f );

}
