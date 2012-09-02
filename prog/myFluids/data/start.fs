// julia.frag: Fragment shader for drawing the Julia set

uniform float Zoom;
uniform vec2  Center;
uniform vec2  JuliaSeed;
uniform vec4  InnerColor;
uniform vec4  OuterColor1;
uniform vec4  OuterColor2;
uniform float time;

uniform float2 arr[3];
	
varying vec3  Position;
varying float LightIntensity;

#define R(p,a) p=vec2(p.x*cos(a)+p.y*sin(a),p.y*cos(a)-p.x*sin(a));

float perlin(vec3 p) {
   vec3 i = floor(p);
   vec4 a = dot(i, float3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
   vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;
   a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);
   a.xy = mix(a.xz, a.yw, f.y);
   return mix(a.x, a.y, f.z);
}

void main(void)
{
	R(Position.xy, 0.5*time);
	//R(Position.xy, 0.05*sin(10*time)* max(0,sqrt(2) - length(Position.xy)   ));
    float   real  = Position.x * 1.3*(1 - .2*abs(sin(5*time))) /*abs(sin(3.14*time/180))*/ - 0.0/*Center.x*/;
    float   imag  = Position.y * 1.5*(1 - .2*abs(sin(5*time))) + /*Center.y*/ - 0.05;
    float   Creal = /*JuliaSeed.x*/ -0.9 + 0.1;
    float   Cimag = 0.2;//JuliaSeed.y;

    float r2 = 0.0;
    int   iter;

    for (iter = 0; iter < 14; ++iter)
    {
        float tempreal = real;

        real = (tempreal * tempreal) - (imag * imag) + Creal;
        imag = 2.0 * tempreal * imag + Cimag;
        r2   = (real * real) + (imag * imag);
    }

    // Base the color on the number of iterations
	
    vec3 color;

    if (r2 < 4.0)
        color = InnerColor.rgb;
    else
        color = mix(OuterColor1.rgb, OuterColor2.rgb, fract(float(iter) * 0.05));

    //color *= LightIntensity;
	
	color.xyz = length(Position.xy).xxx;
	float t = atan(Position.x, abs(Position.y) < 0.0001 ? 0.0001 : Position.y);
	t = (t+3.1415);//*180/3.1415;
	if( t - floor(t) > 30*3.14/180 + (.2 + .2*sin(time*0.05*length(Position.xy) + 10*time)) && color.x < 0.5)
		color.xyz = InnerColor.xyz;//arr[0].x,arr[1].y,0);
		

	//color.x = abs(sin(3.14*100*time/180));
	//color.xy = 0.5*Position.zy + 0.5;
	//color.xyz = perlin(100*Position.xyz).xxx;
    gl_FragColor = vec4 (clamp(color, 0.0, 1.0), 1.0);
	//gl_FragColor = vec4(1,1,1,1);
}