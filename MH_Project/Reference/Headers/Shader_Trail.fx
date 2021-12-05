matrix		g_matWorld, g_matView, g_matProj;		// 상수 테이블

float		g_fAlpha = 1.f;
float		g_fRatio = 1.f;

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
	float4		vProjPos : TEXCOORD1;
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
	Out.vProjPos = Out.vPos;

	return Out;
}

struct PS_IN
{
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
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
	Out.vColor.a *= g_fAlpha;

	return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0.f;

	float2 vTexUV = 0.5f * In.vProjPos.xy / In.vProjPos.w + 0.5f;
	vTexUV.y = 1.f - vTexUV.y;

	float4 Pert = tex2D(BaseSampler, In.vTexUV) * 2.f - 1.f;

	float	x = vTexUV.x;
	float	y = vTexUV.y;

	x += (Pert.x) * g_fRatio;
	y += (Pert.y) * g_fRatio;

	Out.vColor = tex2D(BaseSampler, float2(x, y));
	Out.vColor.a = Pert.a;

	return Out;
}


technique Default_Technique
{
	pass Default
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

	pass Distortion
	{
		cullmode = none;

		AlphaBlendenable = true;
		srcblend = one;
		destblend = one;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DISTORTION();
	}
};