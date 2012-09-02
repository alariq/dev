 float4x4 matWVP : register(c0);


struct VS_IN                                 
{                                              
	float3 ObjPos   : POSITION;    
	
	float4 Color    : COLOR;
	float2 texCoord : TEXCOORD0;
};                                 


struct VS_OUT                                
 {                                                
	float4 ProjPos  : POSITION;                  
	float4 Color    : COLOR;                      
	float2 texCoord : TEXCOORD0;	           
 };        
 
VS_OUT main( VS_IN In )                      
{                                                
	VS_OUT Out;                         
	float4 pos;
	pos.xyz = In.ObjPos;
	pos.w = 1;
	Out.ProjPos = mul( matWVP, pos );      
	Out.Color = In.Color;                    	  
	Out.texCoord = In.texCoord + 0.5/(16.0*256.0);			        
	return Out;                               
}                         