matrix		g_matWorld, g_matView, g_matProj;		// 상수 테이블

int			g_iType;
	
float		g_fFullHpRatio;
float		g_fHpRatio;

float		g_fFullLineHpRatio;
float		g_fLineHpRatio;

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

	float	fHpRatio = g_fHpRatio;
	fHpRatio /= g_fFullHpRatio;

	float	fLineHpRatio = g_fLineHpRatio;
	fLineHpRatio /= g_fFullLineHpRatio;

	//float EndX = 0.65f;

	float2	vTexUV = In.vTexUV;
	float	fAlpha = 1.f;

	if (0 == g_iType)
	{
		if (0.5f <= fHpRatio)
		{
			if (fLineHpRatio < vTexUV.x)
			{
				vTexUV.x = 1.f;
				vTexUV.y = 1.f;
			}
		}
		else if (0.5f >= fHpRatio &&
				 0.4f < fHpRatio)
		{
			if (fLineHpRatio < vTexUV.x)
			{
				fAlpha = 0.f;
			}
		}
		else
		{
			fAlpha = 0.f;
		}
	}
	else if (1 == g_iType)
	{
		if (0.3f <= fHpRatio && 
			0.5f > fHpRatio)
		{
			if (fLineHpRatio < vTexUV.x)
			{
				vTexUV.x = 1.f;
				vTexUV.y = 1.f;
			}
		}
		else if (0.3f >= fHpRatio && 
				 0.2f < fHpRatio)
		{
			if (fLineHpRatio < vTexUV.x)
			{
				fAlpha = 0.f;
			}
		}
		else
		{
			fAlpha = 0.f;
		}
	}
	else if (2 == g_iType)
	{
		if (0.1f <= fHpRatio && 
			0.3f > fHpRatio)
		{
			if (fLineHpRatio < vTexUV.x)
			{
				vTexUV.x = 1.f;
				vTexUV.y = 1.f;
			}
		}
		else if (0.1f >= fHpRatio &&
				 0.f < fHpRatio)
		{
			if (fLineHpRatio < vTexUV.x)
			{
				fAlpha = 0.f;
			}
		}
		else
		{
			fAlpha = 0.f;
		}
	}

	Out.vColor = tex2D(BaseSampler, vTexUV);
	Out.vColor.a = fAlpha;

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