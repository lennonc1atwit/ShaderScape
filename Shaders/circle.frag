uniform float thickness = 1.0;
uniform float fade = 0.010;
uniform float Mod = 1.0;
uniform vec4  color = vec4(1.0, 0.7, 0.0, 1.0);

layout(location = 0) out vec4 fragColor;
void main()
{
    if(thickness == 0)
        discard;

    vec2 uv = gl_FragCoord.xy/iResolution.xy * 2.0 - 1.0;
    vec2 mv = iMouse.xy/iResolution.xy * 2.0 - 1.0;

    //uv-=mv;

    float aspect = iResolution.x / iResolution.y;

    if(iResolution.x > iResolution.y)
        uv.x *= aspect;
    else
        uv.y /= aspect;
    
    float dist = mod(iTime - length(uv),Mod);
    float mask = smoothstep(0.0, fade, dist);
    mask *= smoothstep(thickness+fade, thickness, dist);

    fragColor = color * mask;
}