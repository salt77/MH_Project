matrix		g_matWorld, g_matView, g_matProj;		// ��� ���̺�

texture		g_BaseTexture;


sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
};

struct VS_IN
{
	vector		vPosition	: POSITION;			// �ø�ƽ : ������ ���� �ɼ�(���� ���� FVF)
	float3		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition	: POSITION;			
	float3		vTexUV		: TEXCOORD0;
};

// ���ؽ� ���̴�
VS_OUT			VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN				// �ȼ����̴������� POSITION�̶� �ø�ƽ�� ����� �� ����(���� ���̴� ���������� ��밡���ϵ��� ���׷��̵� ��)
{
	float3			vTexUV		: TEXCOORD0;
	//vector			vPlayerPos	: TEXCOORD1;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = texCUBE(BaseSampler, In.vTexUV);	// 2���� �ؽ�ó���� uv��ǥ�� �ش��ϴ� �ȼ��� ������ �����ϴ� �Լ�, ��ȯ Ÿ���� vector Ÿ��
	Out.vColor.rgb *= 2.3f;

	return Out;
}


technique Default_Device
{
	pass Default
	{
		ZEnable = true;
		ZWriteEnable = false;
		CullMode = cw;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();		
	}
};