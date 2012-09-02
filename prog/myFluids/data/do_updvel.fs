varying vec2 pos;

uniform float dt;

uniform sampler2D velocity0;

void main(void)
{
	float diff = 0.001;
	float N = 128;
	float dt0 = dt*N;
	
	vec2 speed = vec2(0, 1/N);
	
	vec4 v = texture2D(velocity0, pos.xy + dt0*speed);
	gl_FragColor = v;
}
