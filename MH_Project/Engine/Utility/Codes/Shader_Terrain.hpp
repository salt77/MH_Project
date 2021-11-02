matrix		g_matWorld, g_matView, g_matProj;		// 상수 테이블

//float		g_fDetail;

texture		g_BaseTexture;
texture		g_NormalTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
};

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;

	minfilter = LINEAR;
	magfilter = LINEAR;
};

//texture		g_BaseTexture1;
//sampler BaseSampler1 = sampler_state
//{
//	texture = g_BaseTexture1;
//
//	minfilter = linear;
//	magfilter = linear;
//};

//vector	g_vPlayerPos;		// 월드 스페이스 상의 포지션
//float		g_fRange;

struct VS_IN
{
	vector		vPosition	: POSITION;			// 시멘틱 : 정점의 구성 옵션(쉽게 말해 FVF)
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition	: POSITION;	
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	//vector		vWorldPos	: TEXCOORD1;
};

// 버텍스 쉐이더
VS_OUT			VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));

	Out.vTexUV = In.vTexUV;
	//Out.vWorldPos = mul(vector(In.vPosition.xyz, 1.f), g_matWorld);

	return Out;
}

struct PS_IN
{
	float2			vTexUV : TEXCOORD0;
	vector			vNormal : NORMAL;
	//vector			vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
	vector			vNormal : COLOR1;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Out.vColor = (vColor) * (g_vLightDiffuse * g_vMtrlDiffuse) + (In.vShade * (g_vLightAmbient * g_vMtrlAmbient));
	// Out.vColor = (vColor) * (g_vLightDiffuse * g_vMtrlDiffuse) * (In.vShade + (g_vLightAmbient * g_vMtrlAmbient));

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = tex2D(NormalSampler, In.vTexUV);

	return Out;
}


technique Default_Device
{
	pass Render_Terrain
	{
		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}
};