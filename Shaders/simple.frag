uniform vec3 color = vec3(1.0, 0.5, 0.0);
uniform float const1 = 0.5;
uniform float const2 = 0.003;

layout(location = 0) out vec4 fragColor;
void main()
{
	vec2 uv = gl_FragCoord.xy/iResolution.xy * 2.0 - 1.0;
	uv.x *= iResolution.z;
	
	float a = sin(uv.x * const1 * 9.0 + iTime * 5.0) / 2.0;
	float b = sin(uv.x * const1 * 7.0 + iTime * 7.0) / 2.0;
	float c = sin(uv.x * const1 * 5.0 + iTime * 9.0) / 2.0;

	float d = a + b + c;
	
	float f = const2 / abs((uv.y + d)/3.0);
	
	fragColor = vec4(color * f, 1.0);
}