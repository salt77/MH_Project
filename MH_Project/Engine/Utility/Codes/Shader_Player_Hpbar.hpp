matrix		g_matWorld, g_matView, g_matProj;		// ��� ���̺�

bool		g_bIsSpbar;

float		g_fFullHpRatio;
float		g_fHpRatio;

texture		g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = LINEAR;
	magfilter = LINEAR;
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

// ���ؽ� ���̴�
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

	float	fHpRatio = g_fHpRatio;
	fHpRatio /= g_fFullHpRatio;

	float EndX = 0.628f;

	float2 vTexUV = In.vTexUV;

	// TexUV.x�� ü�� �������� ���� ���� �ִٸ� TextureUV�� 1.f�� ARGB�� �����϶�?
	if (!g_bIsSpbar)
	{
		if (fHpRatio * EndX < vTexUV.x)
		{
			vTexUV.x = 1.f;
		}
	}
	else
	{
		EndX = 0.9f;

		if (fHpRatio * EndX < vTexUV.x)
		{
			vTexUV.x = 1.f;
		}
	}

	Out.vColor = tex2D(BaseSampler, vTexUV);

	return Out;
}


technique Default_Technique
{
	pass Default
	{
		AlphaBlendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
		
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};