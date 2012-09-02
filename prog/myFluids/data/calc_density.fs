varying vec2 pos;

void main(void)
{
	gl_FragColor = vec4(pos.xy,0,1);
	//gl_FragColor = vec4(1,0,0,1);
}