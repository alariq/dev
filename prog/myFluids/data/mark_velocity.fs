varying vec2 pos;

uniform vec4 mouse;
uniform float speed_k;

uniform sampler2D u0;
uniform sampler2D v0;

void main(void)
{
	//gl_FragColor = vec4(pos.x*128, pos.y*128, 0,1 );
	//return;

	float u0 = texture2D(u0, pos.xy).x;
	float v0 = texture2D(v0, pos.xy).x;
	vec2 vel0 = vec2(u0, v0);
	//gl_FragColor = vec4(v0,v0,v0,v0);
	//return;
	float radius = 5;
	
	float d = distance(128*pos.xy, mouse.xy);
	float cd = clamp(d,0.0, radius);
	float color = (radius - cd)/(5*radius) + 0.55*(1-step(radius, cd));

	vec2 r = color*0.05*mouse.zw + vel0;
	gl_FragData[0] = //vec4(d,0,0,u0);
			vec4(r.x,0,0,1);//vec4(r.x, 0,0,1);
	gl_FragData[1] = //vec4(d,0,0,v0);
			vec4(r.y,0,0,1);//vec4(r.y, 0,0,1);
	
}
