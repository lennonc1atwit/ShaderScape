uniform vec4 color = vec4(0.9, 0.7, 0.0, 1.0);
uniform float omega = 1;
uniform float k = 0;

layout(location = 0) out vec4 fragColor;
void main()
{
    vec2 uv = gl_FragCoord.xy/iResolution.xy * 2.0 - 1.0;
    vec2 mv = iMouse.xy/iResolution.xy * 2.0 - 1.0;
    uv-=mv;

    float aspect = iResolution.x / iResolution.y;

    if(iResolution.x > iResolution.y)
        uv.x *= aspect;
    else
        uv.y /= aspect;

    vec4 newColor = color.rgba;
    newColor.r = tan((k * uv.x/uv.y) + (omega * iTime));
    newColor.b = tan((k * (uv.x+uv.y)) + (omega * iTime));
    newColor.g = sin((k * (uv.y-uv.x)) + (omega * iTime));

    fragColor = newColor;
}