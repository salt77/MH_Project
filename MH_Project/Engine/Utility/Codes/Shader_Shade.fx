
texture		g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;	
};

texture		g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

vector		g_vLightDir;

vector		g_vLightDiffuse;
vector		g_vLightAmbient;

vector		g_vMtrlDiffuse = (vector)1.f;
vector		g_vMtrlAmbient = (vector)1.f;

vector		g_vCamPos;
float		g_fPower = 10.f;

matrix		g_matInvProj;
matrix		g_matInvView;

struct PS_IN				
{
	float2			vTexUV		: TEXCOORD0;	
};
struct PS_OUT
{
	vector			vShade : COLOR0;
	vector			vSpecular : COLOR1;
};

PS_OUT		PS_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vNormal = tex2D(NormalSampler, In.vTexUV);
	vector vDepth = tex2D(DepthSampler, In.vTexUV);

	// depth 타겟의 g값에 viewz 보관했었다. 텍스쳐 uv 저장하기 위해 far값으로 나눠줬으니 다시 사용할 때는 far값을 곱해서 제대도 된 viewz값으로 복원해야 한다.
	float  fViewZ = vDepth.y * 300.f;
	
	// 텍스쳐 uv상태의 노말 값을 월드 좌표로 변환 시켜줘야 함(0 ~ 1 => -1 ~ 1)
	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);
	Out.vShade = saturate(dot(normalize(vector(g_vLightDir.xyz, 0.f)) * -1.f, vNormal)) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient);
	vector	vReflect = normalize(reflect(normalize(vector(g_vLightDir.xyz, 0.f)), vNormal));
	
	
	vector		vWorldPos;

	// 텍스쳐 0 ~ 1 -> 투영 -1 ~ 1
	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	
	// 텍스쳐 0 ~ 1 -> 투영 1 ~ -1
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;

	// 투영 0 ~ 1
	vWorldPos.z = vDepth.x * fViewZ;

	vWorldPos.w = fViewZ;

	vWorldPos = mul(vWorldPos, g_matInvProj);
	vWorldPos = mul(vWorldPos, g_matInvView);
	
	vector	vLook = normalize(vWorldPos - g_vCamPos);

	Out.vSpecular = pow(saturate(dot(vReflect, vLook * -1.f)), g_fPower);
	
	return Out;
}

PS_OUT		PS_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vNormal = tex2D(NormalSampler, In.vTexUV);

	// 텍스쳐 uv상태의 노말 값을 월드 좌표로 변환 시켜줘야 함(0 ~ 1 => -1 ~ 1)
	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);

	Out.vShade = saturate(dot(normalize(vector(g_vLightDir.xyz, 0.f)) * -1.f, vNormal)) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient);


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