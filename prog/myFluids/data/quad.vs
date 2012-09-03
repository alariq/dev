varying vec2 pos;

void main(void)
{
    gl_Position    = vec4(gl_Vertex.xy, 0, 1); // vec3(gl_Vertex.zyx);
    //pos.xy = 0.5*gl_Vertex.xy + 0.5 ;
    pos.xy = gl_MultiTexCoord0.xy;
}
