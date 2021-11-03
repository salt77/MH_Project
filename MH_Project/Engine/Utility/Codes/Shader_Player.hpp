matrix		g_matWorld, g_matView, g_matProj;		// ��� ���̺�

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


// ���ؽ� ���̴�
struct VS_IN
{
	vector		vPosition	: POSITION;			// �ø�ƽ : ������ ���� �ɼ�(���� ���� FVF)
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition	: POSITION;	
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	vector		vProjPos	: TEXCOORD1;
};

VS_OUT			VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV	= mul(g_matWorld, g_matView);
	matWVP	= mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));
	Out.vProjPos = Out.vPosition;
	
	Out.vTexUV = In.vTexUV;

	return Out;
}


// �ȼ� ���̴�
struct PS_IN				
{
	float2			vTexUV		: TEXCOORD0;	
	vector			vNormal		: NORMAL;
	vector			vProjPos	: TEXCOORD1;
};

struct PS_OUT
{
	vector			vColor	: COLOR0;
	vector			vNormal : COLOR1;
	vector			vDepth	: COLOR2;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// ���� ���ͷ� ����ȭ�� ���� ���� �������� �븻 ������ ��ǥ ���� -1~1�̶� ������ ���´�.
	// �̷� ��ǥ�� �ؽ��� uv��ǥ�� 0~1�� �����Ͽ� ����ؾ� �Ѵ�.
	Out.vColor = tex2D(NormalSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// r : z �����Ⱑ ���� ���� ������ z���� ����
	// g : �佺���̽� ������ z���� ����
	// b, a : �ƹ� ���� �������� ����(far������ ������ �� => Far���� 1000.f�̱� ������ uv�� ǥ���� �Ұ�����)
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, 
						In.vProjPos.w * 0.001f, 
						0.f, 
						0.f);

	return Out;
}

PS_OUT		PS_ALPHATEST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	return Out;
}



technique Default_Device
{
	pass 
	{
		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaTest
	{
		alphatestenable = true;
		alphafunc = greater;
		alpharef = 0xc0;
		cullmode = none;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_ALPHATEST();
	}
};