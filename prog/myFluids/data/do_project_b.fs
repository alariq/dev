varying vec2 pos;


uniform sampler2D div;
uniform sampler2D p;

void main(void)
{
	float N = 128.0;
	float h = 1.0;///N;

	vec2 offset = vec2(1.0/N, 0.0);

	float div = texture2D(div, pos).x;
	float res_p = 0.25*(div 
		+ texture2D(p, pos - offset).x
		+ texture2D(p, pos + offset).x
		+ texture2D(p, pos - offset.yx).x
		+ texture2D(p, pos + offset.yx).x
	); 
	
	gl_FragData[0] = vec4(res_p,0,0,0);
}
