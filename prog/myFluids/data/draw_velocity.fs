varying vec2 pos;

uniform sampler2D u;
uniform sampler2D v;

void main(void)
{
	float cu = texture2D(u, pos.xy).x;
	float cv = texture2D(v, pos.xy).x;
	
	gl_FragColor = vec4(cu,cv, 0, 1);
}
