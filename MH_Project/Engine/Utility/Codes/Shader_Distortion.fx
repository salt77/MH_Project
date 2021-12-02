matrix		g_matWorld, g_matView, g_matProj;		// ��� ���̺�

float		g_fRatio = 0.1f;

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

// ���ؽ� ���̴�
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Matrix		matWV, matWVP;

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
	pass one
	{
		cullmode = none;

		AlphaBlendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};