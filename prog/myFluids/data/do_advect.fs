varying vec2 pos;

uniform float dt;
uniform int	comp;

uniform sampler2D d0;
uniform sampler2D u;
uniform sampler2D v;
uniform float b; // boundary value

void main(void)
{
	float N = 128.0;
	float dt0 = dt;//*N;
	
	// WRONG: at least change b to source value
	//if(pos.x <= 1/128.0 || pos.x >= 127.0/128.0 || pos.y <= 1/128.0 || pos.y >= 127.0/128.0)
	//{
	//	gl_FragColor = vec4(b,0,0,0);
	//	return;
	//}	
	
	//gl_FragColor = texture2D(velocity, pos.xy);
	//return;
	
	// pos has coord of a middle of a cell so substract 0.5/128 to have a cell number 
	// when multiplied by 128
	vec2 our_cell = pos - vec2(0.5/128.0, 0.5/128.0);
	vec2 vel;
	vel.x = texture2D(u, pos.xy).x;
	vel.y = texture2D(v, pos.xy).x;

	//vel.y = -vel.y;
	vec2 coord = our_cell - dt0*vel;
	//coord = clamp(coord, 0.5/128.0, 127.5/128.0);
	
	
	//gl_FragColor = vec4(texture2D(velocity, pos.xy).xy, 0,1);
	//return;
	//gl_FragColor = vec4(pos.xy,0,1);
		
	vec2 cell = floor(coord*N)/N;
	
	//gl_FragColor = vec4(pos.x, pos.y, cell.x, pos.x);
	//return;
	
	//gl_FragColor = vec4(coord.xy,0,1);
	//return;
	
	//gl_FragColor = vec4((coord.xy-cell.xy),0,1);
	//return;
	
	vec2 offset = vec2(1.0/N, 0.0);
	
	vec2 st = N*(coord - cell);
	
	//gl_FragColor = vec4(st.x, st.y, 0, st.x);
	//gl_FragColor = vec4(st.xy,0,1);
	//return;
	
	vec2 midcell = vec2(0.5/N, 0.5/N);
	
	vec4 d;
	d.x = texture2D(d0, cell.xy + midcell).x; // 00
	d.y = texture2D(d0, cell.xy + offset.xy + midcell).x; // 10
	d.z = texture2D(d0, cell.xy + offset.yx + midcell).x; // 01
	d.w = texture2D(d0, cell.xy + offset.xx + midcell).x; // 11
	
	float v1 = (1.0-st.y)*d.x + st.y*d.z; // vert lerp 00 - 01
	float v2 = (1.0-st.y)*d.y + st.y*d.w;   // vert lerp  01 - 11
	float dout = (1.0-st.x)*v1 + st.x*v2;		// final horiz lerp  v1 - v2
					
	gl_FragColor = vec4(dout, 0, 0, 1);
}
