matrix		g_matWorld, g_matView, g_matProj;		// 상수 테이블

float		g_fAlphaValue = 1.f;

texture		g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
};

struct VS_IN
{
	vector		vPos	: POSITION;
	float2		vTexUV	: TEXCOORD0;
};

struct VS_OUT
{
	vector		vPos	: POSITION;
	float2		vTexUV	: TEXCOORD0;
};

// 버텍스 쉐이더
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);

	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2 vTexUV = In.vTexUV;

	Out.vColor = tex2D(BaseSampler, vTexUV);
	Out.vColor.a *= g_fAlphaValue;

	return Out;
}


technique Default_Technique
{
	pass AlphaBlend_Default
	{
		AlphaTestEnable = false;
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
		
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlend_Black
	{
		cullmode = none;

		zenable = true;
		zwriteenable = false;

		AlphaBlendenable = true;
		srcblend = srcalpha;
		destblend = one;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlend_Cull_None
	{
		cullmode = none;

		AlphaTestEnable = false;
		AlphaBlendEnable = true;
		srcblend = one;
		destblend = one;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};