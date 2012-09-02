varying vec2 pos;


uniform sampler2D u;
uniform sampler2D v;

void main(void)
{
	float N = 128.0;
	float h = 1.0;
	float div, p;

	vec2 offset = vec2(1.0/N, 0.0);

	div = -0.5*h*( 
		+ texture2D(u, pos + offset).x 
		- texture2D(u, pos - offset).x 
		+ texture2D(v, pos + offset.yx).x
		- texture2D(v, pos - offset.yx).x
	);

	p = 0; 
	
	gl_FragData[0] = vec4(div, 0,0,0);
	gl_FragData[1] = vec4(p, 0,0,0);
	// copy values to other texture (we will need them in phase do_project_c)
	gl_FragData[2] = vec4(texture2D(u, pos.xy).x,0,0,0);
	gl_FragData[3] = vec4(texture2D(v, pos.xy).x,0,0,0);
}
