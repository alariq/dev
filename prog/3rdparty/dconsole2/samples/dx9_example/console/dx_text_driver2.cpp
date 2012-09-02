#include <string>
#include <math.h>
#include "dx_text_driver2.h"
extern LPDIRECT3DDEVICE9 GetDevice();




D3DVERTEXELEMENT9 D3DXTextDriver2::fontVertDecl_[] = 
{
	{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	D3DDECL_END()
};

//-------------------------------------------------------------------------------------
// Vertex shader
// We use the register semantic here to directly define the input register
// matWVP.  Conversely, we could let the HLSL compiler decide and check the
// constant table.
//-------------------------------------------------------------------------------------
const char* g_strVertexShaderProgram = 
" float4x4 matWVP : register(c0);              "  
"                                              "  
" struct VS_IN                                 "  
" {                                            " 
"     float4 ObjPos   : POSITION;              "  // Object space position 
"     float4 Color    : COLOR;                 "  // Vertex color 
"     float4 texCoord : TEXCOORD0;	           "  
" };                                           " 
"                                              " 
" struct VS_OUT                                " 
" {                                            " 
"     float4 ProjPos  : POSITION;              "  // Projected space position 
"     float4 Color    : COLOR;                 " 
"     float4 texCoord : TEXCOORD0;	           "  
" };                                           "  
"                                              "  
" VS_OUT main( VS_IN In )                      "  
" {                                            "  
"     VS_OUT Out;                              "  
"     Out.ProjPos = mul( matWVP, In.ObjPos );  "  // Transform vertex into
"     Out.Color = In.Color;                    "  // Projected space and 
"	  Out.texCoord = In.texCoord;			   "
"     return Out;                              "  // Transfer color
" }                                            ";

//-------------------------------------------------------------------------------------
// Pixel shader
//-------------------------------------------------------------------------------------
const char* g_strPixelShaderProgram =			
"												"
" sampler baseMap: register(s0) = sampler_state "
" {												"
"    //Texture = (tex0);						"
"    MipFilter = LINEAR;						"
"    MinFilter = LINEAR;						"
"    MagFilter = LINEAR;						"
"	AddressU=CLAMP;								"
"	AddressV=CLAMP;								"
" };  											"
"   											"
"   											"
"   											"
"   											"
"   											"
"   											"
" struct PS_IN                                 "
" {                                            "
"     float4 Color : COLOR;                    "  // Interpolated color from      
"     float4 texCoord : TEXCOORD0;             "  // Interpolated color from                      
" };                                           "  // the vertex shader
"                                              "  
" float4 main( PS_IN In ) : COLOR              "  
" {                                            "  
"     return In.Color*tex2D( baseMap, In.texCoord.xy );                         "  // Output color
" }                                            "; 


D3DXTextDriver2::D3DXTextDriver2():
fontHeight_(0),
fontWidth_(0),
scrWidth_(0),
scrHeight_(0),
num_chars_(0)
{
}

D3DXTextDriver2::~D3DXTextDriver2()
{
}

size_t D3DXTextDriver2::getWidth(const char* s, size_t len)
{
	if(!s) return 0;
	if(-1 == len) return strlen(s)*charStep_;

	// as we are using in this implementation fixed with font we can just return this
	return len*charStep_;
}

bool D3DXTextDriver2::init(const char* fontPath, int charStep, int fontWidth, int fontHeight, int screenWidth, int screenHeight)
{
	HRESULT hr = E_FAIL;
	hr = D3DXCreateTextureFromFile(GetDevice(), fontPath, &pFontTexture_);
	if(FAILED(hr))
		return false;

	LPDIRECT3DSURFACE9 pTexSurf;
	hr = pFontTexture_->GetSurfaceLevel(0,&pTexSurf);
	if(FAILED(hr)) return false;
	hr = pTexSurf->GetDesc(&fontTexDesc_);
	if(FAILED(hr)) return false;
	pTexSurf->Release();

	
	GetDevice()->CreateVertexDeclaration(fontVertDecl_, &pFontVD_);

	// buffer enough to store 512 chars using triangles
	if( FAILED( GetDevice()->CreateVertexBuffer( (6*512)*sizeof(FontVertex_t),
                                                  0,
                                                  NULL,
                                                  D3DPOOL_MANAGED, 
                                                  &pVB_, 
                                                  NULL ) ) )
		return false;



	ID3DXBuffer* pShaderCode = 0;
    ID3DXBuffer* pErrorMsg = 0;

	hr = D3DXCompileShaderFromFile("font.vs", NULL, NULL, "main", "vs_2_0", 
									D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION , 
                                   &pShaderCode, 
                                   &pErrorMsg, 
                                   NULL);
	if( FAILED(hr) )
    {
        if( pErrorMsg )
            OutputDebugString( (char*)pErrorMsg->GetBufferPointer() );
        return false;
    }
    
    // Create vertex shader.
	hr = GetDevice()->CreateVertexShader( (DWORD*)pShaderCode->GetBufferPointer(), &pVertexShader_);
	// The shader code is no longer required.
	pShaderCode->Release();
	if(FAILED(hr))
		return false;


	hr = D3DXCompileShaderFromFile("font.ps",NULL, NULL,"main","ps_2_0",
		D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION , 
        &pShaderCode, 
        &pErrorMsg,
        NULL);

	if( FAILED(hr) )
    {
        if( pErrorMsg )
            OutputDebugString( (char*)pErrorMsg->GetBufferPointer() );
        return false;
    }

    // Create pixel shader.
    hr = GetDevice()->CreatePixelShader( (DWORD*)pShaderCode->GetBufferPointer(), &pPixelShader_ );
	// The shader code is no longer required.
	pShaderCode->Release();
	if(FAILED(hr))
		return false;

	charStep_ = charStep;
	fontHeight_ = fontWidth;
	fontWidth_ = fontHeight;

	//create projection matrix for 2D elements
	D3DXMatrixOrthoLH(&matProj_,(float)screenWidth, (float)screenHeight, 0.1f, 100);
	D3DXMATRIX tr_m, scale_m;

	D3DXMatrixTranslation(&tr_m, -.5f*(float)screenWidth, -.5f*(float)screenHeight, 0);
	D3DXMatrixTranslation(&scale_m, 1, -1, 1);
	
	D3DXMatrixIdentity(&matView_);
	D3DXMatrixIdentity(&matWorld_);

	D3DXMatrixMultiply(&matView_, &matView_, &tr_m);
	D3DXMatrixMultiply(&matView_, &matView_, &scale_m);
	
	scrWidth_ = screenWidth;
	scrHeight_ = screenHeight;

	return true;
}

bool D3DXTextDriver2::beginDraw()
{
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	num_chars_ = 0;


		
	GetDevice()->SetVertexDeclaration( pFontVD_ );
	
	GetDevice()->SetVertexShader( pVertexShader_ );
	GetDevice()->SetPixelShader( pPixelShader_ );
   
	// Build the world-view-projection matrix and pass it into the vertex shader
	D3DXMATRIX matWVP = matWorld_ * matView_ * matProj_;
	GetDevice()->SetVertexShaderConstantF( 0, (FLOAT*)&matWVP, 4 );

	GetDevice()->SetTexture(0,pFontTexture_);

    if( FAILED( pVB_->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
        return false;

	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	//GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);// because of HZet
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return true;
}

void D3DXTextDriver2::endDraw()
{
	pVB_->Unlock();
	GetDevice()->SetStreamSource( 0, pVB_, 0, sizeof(FontVertex_t) );
	HRESULT hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2*num_chars_);

	GetDevice()->SetVertexShader( 0 );
	GetDevice()->SetPixelShader( 0 );
	GetDevice()->SetTexture(0,0);

}

void	D3DXTextDriver2::printString(float x, float y, const char* s, const unsigned char* color)
{
	if(!s) return;
	size_t len = strlen(s);
	if(len==0) return;

	FontVertex_t* pCurVert = pVertices + 6*num_chars_;
	if(num_chars_ + len > VB_SIZE)
		return ;
	
	const float du = fontWidth_/(float)fontTexDesc_.Width;
	const float dv = fontHeight_/(float)fontTexDesc_.Height;

	D3DCOLOR  d3d_color = D3DCOLOR_RGBA(color[0],color[1],color[2],color[3]);

	float depth  = .11f;
	x-=0.5f;
	y-=0.5f;
	for(unsigned int i=0;i<len;++i)
	{
		float u,v;
		u = (s[i] % fontWidth_)/(float)fontWidth_;
		v = (s[i] / fontHeight_)/(float)fontHeight_;

		// 1st tri
		pCurVert[6*i+0].x = x;
		pCurVert[6*i+0].y = y;
		pCurVert[6*i+0].z = depth;
		pCurVert[6*i+0].tu = u;
		pCurVert[6*i+0].tv = v+dv;
		pCurVert[6*i+0].color = d3d_color;
		pCurVert[6*i+0].z = depth;

		pCurVert[6*i+1].x = x;
		pCurVert[6*i+1].y = y+fontHeight_;
		pCurVert[6*i+1].tu = u;
		pCurVert[6*i+1].tv = v;
		pCurVert[6*i+1].color = d3d_color;
		pCurVert[6*i+1].z = depth;

		pCurVert[6*i+2].x = x+fontWidth_;
		pCurVert[6*i+2].y = y;
		pCurVert[6*i+2].tu = u+du;
		pCurVert[6*i+2].tv = v+dv;
		pCurVert[6*i+2].color = d3d_color;
		pCurVert[6*i+2].z = depth;
	
		// 2nd tri
		pCurVert[6*i+3].x = x+fontWidth_;
		pCurVert[6*i+3].y = y;
		pCurVert[6*i+3].tu = u+du;
		pCurVert[6*i+3].tv = v+dv;
		pCurVert[6*i+3].color = d3d_color;
		pCurVert[6*i+3].z = depth;

		pCurVert[6*i+4].x = x;
		pCurVert[6*i+4].y = y+fontHeight_;
		pCurVert[6*i+4].tu = u;
		pCurVert[6*i+4].tv = v;
		pCurVert[6*i+4].color = d3d_color;
		pCurVert[6*i+4].z = depth;

		pCurVert[6*i+5].x = x+fontWidth_;
		pCurVert[6*i+5].y = y+fontHeight_;
		pCurVert[6*i+5].tu = u+du;
		pCurVert[6*i+5].tv = v;
		pCurVert[6*i+5].color = d3d_color;
		pCurVert[6*i+5].z = depth;

		x += charStep_;
		num_chars_++;
	}
}

void D3DXTextDriver2::getScreenParams(int& width, int& height)
{
	width = scrWidth_;
	height = scrHeight_;
}

void D3DXTextDriver2::setScreenParams(int w, int h)
{
	scrWidth_ = w;
	scrHeight_ = h;
	//create projection matrix for 2D elements
    D3DXMatrixOrthoLH(&matProj_,(float)scrWidth_, (float)scrHeight_, 0.1f, 100);
}

void D3DXTextDriver2::getFontParams(int& font_width, int& font_height) 
{
	font_width = fontWidth_;
	font_height = fontHeight_;
}

