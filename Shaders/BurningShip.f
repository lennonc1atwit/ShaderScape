uniform int SAMPLES = 8;
uniform int MAX_ITER = 80;
uniform float THRESHOLD = 16;
uniform float ZOOM = 1;
uniform vec4 OFFSET = vec4(1,2,3,4);
uniform mat4x4 TEST = mat4x4(vec4(1,2,3,4), vec4(5,6,7,8), vec4(9,10,11,12), vec4(13,14,15,16));
// uniform mat2x3 TEST = mat2x3(vec3(1, 2, 3), vec3(4, 5, 6));
uniform int Color = 1;

struct palette {
    vec3 c0, c1, c2, c3, c4;
};

palette palette_red_accented() {
    palette p; 
    p.c0 = vec3(0,0,16)/255.;
    p.c1 = vec3(202,202,200)/255.;
    p.c2 = vec3(255)/255.;
    p.c3 = vec3(182,132,107)/255.;
    p.c4 = vec3(247,49,73)/255.;
    return p;    
}

    
palette palette_calm_blue() {
    palette p; 
    p.c0 = vec3(5,8,49)/255.;
    p.c1 = vec3(19,48,114)/255.;
    p.c2 = vec3(213,230,247)/255.;
    p.c3 = vec3(213,230,247)/255.;
    p.c4 = vec3(5,8,49)/255.;
    return p;    
}
    
palette palette_green() {
    palette p; 
    p.c0 = vec3(0,120,122)/255.;
    p.c1 = vec3(148,235,216)/255.;
    p.c2 = vec3(0,179,73)/255.;
    p.c3 = vec3(0,121,57)/255.;
    p.c4 = vec3(0,61,52)/255.;
    return p;    
}

palette palette_blue() {
    palette p; 
    p.c0 = vec3(0,2,5)/255.;
    p.c1 = vec3(8,45,58)/255.;
    p.c2 = vec3(38,116,145)/255.;
    p.c3 = vec3(167,184,181)/255.;
    p.c4 = vec3(207,197,188)/255.;
    return p;    
}

// Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float randSeed = 0.;
vec2 nextRand2() {
    vec2 v = vec2( randSeed++, randSeed++ );
	return vec2( random( v+0.34 ), random( v+0.75 ) );    
}

// Mapping
vec3 cmap( float t, palette p ) {
    vec3 col = vec3(0);
    col = mix( p.c0,  p.c1, smoothstep(0. , .2, t));
    col = mix( col, p.c2, smoothstep(.2, .4 , t));
    col = mix( col, p.c3, smoothstep(.4 , .6, t));
    col = mix( col, p.c4, smoothstep(.6,  .8, t));
    col = mix( col, p.c0, smoothstep(.8, 1.,  t));
    return col;
}

float bship(vec2 uv) {
    
    // z = ( |Re(z)| + i|Im(z)| )^2
    
	vec2 c = 0.1*(uv - vec2(.2,0)) - vec2(1.74,0.035); 
    vec2 z = vec2(0); 
    float i = 0.;
    
    for(; ++i <= MAX_ITER ;) {
        z = abs(z);
        z = mat2(z, -z.y, z.x) * z + c;
    	if( dot(z,z) > THRESHOLD ) break;
	}
  
    float l2 = log(2.);
    return i - log(log(dot(z,z))/l2)/l2;		    
}

// Main
layout(location = 0) out vec4 fragColor;
void main() {
    palette pal = palette_red_accented();

    if(Color == 1)
    {
        pal = palette_blue();
    }
    if(Color == 2)
    {
        pal = palette_calm_blue();
    }
    if(Color == 3)
    {
        pal = palette_green();
    }
    
    vec3 col = vec3(0);

    for(float i=0.; i < SAMPLES; i++) {
        vec2 p = 0.5*(2.*gl_FragCoord.xy-iResolution.xy+nextRand2());

        p *= TEST[0][0]/iResolution.y;
        p += OFFSET.xy;
        p.x /= iResolution.z;
        p.y = -p.y;
        

        float orbit = bship(p) / MAX_ITER;
    	col += cmap( orbit   , pal ); 
    }
    
    col /= SAMPLES;

    fragColor = vec4(col, 1.);
}