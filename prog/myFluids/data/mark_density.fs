varying vec2 pos;

uniform vec2 mouse;
uniform sampler2D x0;

void main(void)
{
	//test addressing
	//gl_FragColor = vec4(pos.x*128.0, 128.0*pos.y,0,0 );
	//return;
	
	float d = distance(128*pos.xy, mouse.xy);
	float color = 1.5*step(clamp(d,0.0, 1.0), .5);
	gl_FragColor = vec4(color + texture2D(x0, pos.xy).x, 0, 0, 1);
	//gl_FragColor = vec4(pos.x, pos.y,1,1 );
}
