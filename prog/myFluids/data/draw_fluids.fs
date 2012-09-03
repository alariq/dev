varying vec2 pos;

uniform sampler2D d;
//uniform sampler2D p;

void main(void)
{
	float dens = texture2D(d, pos.xy).x;
	float pressure = 1;// + texture2D(p, pos.xy).x;
	
	gl_FragColor = vec4(dens*pressure,dens*pressure,dens*pressure, 1);
}
