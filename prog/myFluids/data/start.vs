// julia.vert: Vertex shader for drawing the Julia set

uniform vec3 LightPosition;
uniform float SpecularContribution;
uniform float DiffuseContribution;
uniform float Shininess;

//uniform vec4 vViewPosition;

varying float LightIntensity;
varying vec3  Position;
varying vec3  raydir;

varying vec3  vViewPosition;

void main(void)
{
    vec3 ecPosition = vec3 (gl_ModelViewMatrix * gl_Vertex);
    vec3 tnorm      = normalize(gl_NormalMatrix * gl_Normal);
    vec3 lightVec   = normalize(LightPosition - ecPosition);
    vec3 reflectVec = reflect(-lightVec, tnorm);
    vec3 viewVec    = normalize(-ecPosition);
    float spec      = max(dot(reflectVec, viewVec), 0.0);
    spec            = pow(spec, Shininess);
    LightIntensity  = DiffuseContribution * 
                          max(dot(lightVec, tnorm), 0.0) +
                          SpecularContribution * spec;
    Position        = vec3(gl_Vertex.xyz); // vec3(gl_Vertex.zyx);
    gl_Position     = vec4(gl_Vertex.xy, 0,1);//ftransform();
	vec3 r			= gl_Vertex.xyz*vec3(1, 1, 0) + vec3(0,0,1);
	raydir 			= mul(vec4(r,0), gl_ModelViewMatrix).xyz;
	//Position     	= vec4(gl_Vertex.x, gl_Vertex.y, 0.0, 1.0);//ftransform();
	vViewPosition	= vec3(gl_ModelViewMatrix*vec4(0,0,0,1));
}
