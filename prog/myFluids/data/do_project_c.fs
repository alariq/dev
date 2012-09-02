varying vec2 pos;

uniform sampler2D p;
uniform sampler2D u;
uniform sampler2D v;

// updates current velocity 
void main(void)
{
	float N = 128.0;
	float h = 1.0;///N;

	vec2 offset = vec2(1.0/N, 0.0);

	vec2 vel = vec2(texture2D(u, pos).x, texture2D(v, pos).x);

	vel.x -= 0.5*(	texture2D(p, pos + offset).x - texture2D(p, pos - offset).x )/h;

	vel.y -= 0.5*(	texture2D(p, pos + offset.yx).x	- texture2D(p, pos - offset.yx).x )/h;
	
	gl_FragData[0] = vec4(vel.x, 0, 0, 0);
	gl_FragData[1] = vec4(vel.y, 0, 0, 0);
}
