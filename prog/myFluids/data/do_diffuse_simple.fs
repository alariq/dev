varying vec2 pos;
uniform float dt;

uniform sampler2D x;
uniform sampler2D x0;
uniform float b; // boundary value

#define HALF_PIX vec2(0.5/128.0, 0.5/128)
void main(void)
{
	float diff = 0.003;
	float N = 128;
	
	vec2 tp = pos.xy;// - vec2(0.5/128.0, 0.5/128); 
	if(tp.x <= 1/128.0 || tp.x >= 127.0/128.0 || tp.y <= 1/128.0 || tp.y >= 127.0/128.0)
	{
		gl_FragColor = vec4(texture2D(x0, pos.xy - HALF_PIX).x,0,0,0);
		return;
	}
			
	vec2 offset = vec2(1/N, 0);
	float dout = texture2D(x0, pos.xy).x;
	vec4 d;
	
	//gl_FragColor = vec4(1,1,0,1*dout);
	//return;
		
	d.x = texture2D(x0, pos.xy - offset.xy).x;
	d.y = texture2D(x0, pos.xy - offset.yx).x;
	d.z = texture2D(x0, pos.xy + offset.xy).x;
	d.w = texture2D(x0, pos.xy + offset.yx).x;

	float a = dt*diff;//*N*N;
	dout = (dout + a*dot(d, vec4(1,1,1,1)))/(1 + 4*a);
	gl_FragColor = vec4(dout,0, 0, 1);
}
