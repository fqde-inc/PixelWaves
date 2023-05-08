//Inputs
in vec2 TexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform sampler2D SourceTexture;

uniform float Exposure;

uniform vec3 ColorFilter;
uniform float Contrast;
uniform float HueShift;
uniform float Saturation;
uniform float Pixelation;
uniform int Downsampling;
uniform float Sharpness;

vec2 Resolution = vec2(1024,1024);

vec3 AdjustContrast(vec3 color)
{
	color = (color - vec3(0.5f)) * Contrast + vec3(0.5f);
	return clamp(color, 0, 1);
}

vec3 AdjustHue(vec3 color)
{
	vec3 hsvColor = RGBToHSV(color);
	hsvColor.x = fract(hsvColor.x + HueShift + 1.0f);
	return HSVToRGB(hsvColor);
}

vec3 AdjustSaturation(vec3 color)
{
	vec3 luminance = vec3(GetLuminance(color));
	color = (color - luminance) * Saturation + luminance;
	return clamp(color, 0, 1);
}

vec3 ApplyColorFilter(vec3 color)
{
	return color * ColorFilter;
}

vec3 ApplySharpen (sampler2D tex, vec2 fragCoord)
{   
	vec2 step = 1.0 / Resolution;
	
	vec3 up		= texture( tex, fragCoord + vec2( 0, step.y) ).rgb;
	vec3 left	= texture( tex, fragCoord + vec2( -step.x, 0) ).rgb;
	vec3 right	= texture( tex, fragCoord + vec2( step.x, 0) ).rgb;
	vec3 down	= texture( tex, fragCoord + vec2( 0, -step.y) ).rgb;

    vec3 around = 0.25 * (up + left + right + down);
	vec3 center  = texture( tex, fragCoord ).rgb;
    
	return center + (center - around) * Sharpness;
}

void main()
{
	float pixelation = Resolution.x / Downsampling;

	// Pixelate
	vec2 roundedTexCoord = floor(TexCoord * pixelation) / pixelation;

	// Read from the HDR framebuffer
	vec3 hdrColor = texture(SourceTexture, roundedTexCoord).rgb;

	// Apply exposure
	vec3 color = vec3(1.0f) - exp(-hdrColor * Exposure);

	// Color grading
	color = AdjustContrast(color);
	color = AdjustHue(color);
	color = AdjustSaturation(color);
	color = ApplyColorFilter(color);

	// Apply sharpen
    vec3 sharpenedColor = ApplySharpen(SourceTexture, roundedTexCoord);

    // Combine sharpened and original color
    color = mix(color, sharpenedColor.rgb, 0.5f);

	// Assign the fragment color
	FragColor = vec4(color.rgb, 1.0f);
}
