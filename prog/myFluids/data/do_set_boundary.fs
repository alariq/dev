varying vec2 pos;
uniform float dt;

uniform sampler2D x;
uniform int b; // boundary value

#define LEFT 1/128.0
#define TOP 1/128.0
#define BOTTOM 127/128.0
#define RIGHT 127/128.0
#define HALF_CELL	0.5/128.0

void main(void)
{
	float diff = 0.01;
	float N = 128;
	
	vec3 offset = vec3(1/128.0, 0, -1/128.0);
	
	//gl_FragColor = vec4(pos.x,pos.y,texture2D(x, pos.xy).x,b);
	//return;
	
	float v = 1224;

	if(pos.x <= LEFT)
	{
		if(pos.y <= TOP)
		{
			float t = texture2D(x, pos.xy + offset.xx).x;
			float v1 = b==2 ? -t : t;
			float v2 = b==1 ? -t : t;
			v = 0.5*(v1 + v2);
		}
		else if(pos.y >= BOTTOM)
		{
			float t = texture2D(x, pos.xy + offset.xz).x;
			float v1 = b==2 ? -t : t;
			float v2 = b==1 ? -t : t;
			v = 0.5*(v1 + v2);
		}
		else
		{
			float t = texture2D(x, pos.xy + offset.xy).x;
			v = b == 1 ? -t : t;
		}
	}
	else if(pos.x >= RIGHT)
	{
		if(pos.y <= TOP)
		{
			float t = texture2D(x, pos.xy + offset.zx).x;
			float v1 = b==2 ? -t : t;
			float v2 = b==1 ? -t : t;
			v = 0.5*(v1 + v2);
		}
		else if(pos.y >= BOTTOM)
		{
			float t = texture2D(x, pos.xy + offset.zz).x;
			float v1 = b==2 ? -t : t;
			float v2 = b==1 ? -t : t;
			v = 0.5*(v1 + v2);
		}
		else
		{
			float t = texture2D(x, pos.xy + offset.zy).x;
			v = b == 1 ? -t : t;
		}
	}
	else
	{
		if(pos.y <= TOP)
		{
			float t = texture2D(x, pos.xy + offset.yx).x;
			v = b == 2 ? -t : t;
		}
		else if(pos.y >= BOTTOM)
		{
			float t = texture2D(x, pos.xy + offset.yz).x;
			v = b == 2 ? -t : t;
		}
		else
		{
			v = texture2D(x, pos.xy);
		}
	}
	
	gl_FragData[0] = vec4(v,0,0,0);//texture2D(x, pos.xy));
}
