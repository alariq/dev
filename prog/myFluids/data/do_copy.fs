varying vec2 pos;

uniform sampler2D tex;

void main(void)
{
	gl_FragColor = vec4( texture2D(tex, pos.xy).x, 0,0,0);
	return;
}
