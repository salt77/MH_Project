matrix		g_matWorld, g_matView, g_matProj;		// 상수 테이블

float		g_fDissolveValue = 0.f;
float4		g_vLightDir;

texture		g_BaseTexture, g_NormalTexture, g_DissolveTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
};

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;

	minfilter = linear;
	magfilter = linear;
};

sampler DissolveSampler = sampler_state
{
	texture = g_DissolveTexture;

	minfilter = linear;
	magfilter = linear;
};


struct VS_IN
{
	vector		vPosition	: POSITION;			// 시멘틱 : 정점의 구성 옵션(쉽게 말해 FVF)

	float2		vTexUV		: TEXCOORD0;
	vector		vNormal		: NORMAL;
	float3		vTangent	: TANGENT;
	float3		vBiNormal	: BINORMAL;
};

struct VS_OUT
{
	vector		vPosition	: POSITION;
	float2		vTexUV		: TEXCOORD0;
	float3		vTangent	: TEXCOORD1;
	float3		vBiNormal	: TEXCOORD2;
	float3		vNormal		: TEXCOORD3;
	float4		vWorldPos	: TEXCOORD4;
	float4		vProjPos	: TEXCOORD5;
};

// 버텍스 쉐이더
VS_OUT			VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	Out.vPosition = mul(float4(In.vPosition.xyz, 1.f), g_matWorld);
	Out.vWorldPos = Out.vPosition;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vTexUV = In.vTexUV;

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vNormal = normalize(mul(In.vNormal.xyz, (float3x3)g_matWorld));
	Out.vTangent = normalize(mul(In.vTangent.xyz, (float3x3)g_matWorld));
	Out.vBiNormal = normalize(cross(Out.vTangent, Out.vNormal));

	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	vector		vPosition	: POSITION;
	float2		vTexUV		: TEXCOORD0;
	float3		vTangent	: TEXCOORD1;
	float3		vBiNormal	: TEXCOORD2;
	float3		vNormal		: TEXCOORD3;
	float4		vWorldPos	: TEXCOORD4;
	float4		vProjPos	: TEXCOORD5;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
	vector			vNormal : COLOR1;
	vector			vDepth : COLOR2;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4	vClipAmount = tex2D(DissolveSampler, In.vTexUV).r;
	clip(vClipAmount - g_fDissolveValue);

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = tex2D(NormalSampler, In.vTexUV);

	float4 tempNormal = Out.vNormal;
	float4 normalVec = 2 * tempNormal - 1.f;
	normalVec = normalize(normalVec);

	float3x3 TBN = float3x3(normalize(In.vTangent), normalize(In.vBiNormal), normalize(In.vNormal));
	TBN = transpose(TBN);
	float3	worldNormal = mul(TBN, normalVec);


	float3	TempLightDir = g_vLightDir.xyz;

	float3 bright = saturate(dot(-TempLightDir, worldNormal)) + 0.35f;

	Out.vColor.rgb = bright * Out.vColor.rgb;
	Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w * 0.03f, 0.f, 0.f);

	return Out;
}

technique Default_Device
{
	pass
	{
		alphatestenable = true;
		alphafunc = greater;
		alpharef = 0xc0;
		cullmode = none;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}
};