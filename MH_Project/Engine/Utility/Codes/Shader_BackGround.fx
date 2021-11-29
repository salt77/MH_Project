matrix		g_matWorld;		// ��� ���̺�
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
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition	: POSITION;			
	float2		vTexUV		: TEXCOORD0;
};

// ���ؽ� ���̴�
VS_OUT			VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), g_matWorld);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN				// �ȼ����̴������� POSITION�̶� �ø�ƽ�� ����� �� ����(���� ���̴� ���������� ��밡���ϵ��� ���׷��̵� ��)
{
	float2			vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2���� �ؽ�ó���� uv��ǥ�� �ش��ϴ� �ȼ��� ������ �����ϴ� �Լ�, ��ȯ Ÿ���� vector Ÿ��

	return Out;
}


technique Default_Device
{
	// pass : ����� ĸ��ȭ�� ���� ���� �� ������ �� ����, ������ �Լ� ȣ���� ����
	// �н��� �̸��� ������ ������ �������. pass�� ���ʿ� ����� �༮���� 0�� �ε����� �����ȴ�.

	pass Alphablend
	{
		Alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}
};