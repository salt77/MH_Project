texture		g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;	
};

vector		g_vLightDir;

vector		g_vLightDiffuse;
vector		g_vLightAmbient;

vector		g_vMtrlDiffuse = (vector)1.f;
vector		g_vMtrlAmbient = (vector)1.f;

struct PS_IN				
{
	float2			vTexUV		: TEXCOORD0;	
};
struct PS_OUT
{
	vector			vShade : COLOR0;
};

PS_OUT		PS_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vNormal = tex2D(NormalSampler, In.vTexUV);
	
	// 텍스쳐 uv상태의 노말 값을 월드 좌표로 변환 시켜줘야 함(0 ~ 1 => -1 ~ 1)
	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);

	Out.vShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient);
	
	return Out;
}

PS_OUT		PS_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vNormal = tex2D(NormalSampler, In.vTexUV);

	// 텍스쳐 uv상태의 노말 값을 월드 좌표로 변환 시켜줘야 함(0 ~ 1 => -1 ~ 1)
	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);

	Out.vShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient);

	return Out;
}


technique Default_Device
{
	pass Directional
	{
		zwriteenable = false;
		vertexshader = NULL;
		pixelshader = compile ps_3_0 PS_DIRECTIONAL();

	}
	pass Point
	{
		zwriteenable = false;
		vertexshader = NULL;
		pixelshader = compile ps_3_0 PS_POINT();
	}
};