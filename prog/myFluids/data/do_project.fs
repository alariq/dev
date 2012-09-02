varying vec2 pos;


uniform sampler2D velocity;

void main(void)
{
	float N = 128.0;
	float h = 1.0/N; 
	float div;
	float p;

	vec2 offset = vec2(1.0/N, 0.0);

	div = 	-0.5*h*(
			texture2D(velocity, pos + offset).x 
			-texture2D(velocity, pos - offset).x
			+ texture2D(velocity, pos + offset.yx).y
			- texture2D(velocity, pos - offset.yx).y
	);

	p = 0; 
					
	gl_FragColor = vec4(div, p, 0, 0);
}
