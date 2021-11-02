matrix		g_matWorld, g_matView, g_matProj;		// 상수 테이블
texture		g_BaseTexture;


sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
};

struct VS_IN
{
	vector		vPosition	: POSITION;			// 시멘틱 : 정점의 구성 옵션(쉽게 말해 FVF)
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition	: POSITION;			
	float2		vTexUV		: TEXCOORD0;
};

// 버텍스 쉐이더
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

struct PS_IN				// 픽셀쉐이더에서는 POSITION이란 시멘틱을 사용할 수 없다(현재 쉐이더 버전에서는 사용가능하도록 업그레이드 됨)
{
	float2			vTexUV		: TEXCOORD0;
	//vector			vPlayerPos	: TEXCOORD1;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처에서 uv좌표에 해당하는 픽셀의 색상을 추출하는 함수, 반환 타입은 vector 타입
	
	//Out.vColor.rb = 0.5f;

	return Out;
}

PS_OUT		PS_MAIN_TEMP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처에서 uv좌표에 해당하는 픽셀의 색상을 추출하는 함수, 반환 타입은 vector 타입

	Out.vColor.a = 0.5f;

	return Out;
}


technique Default_Device
{
	// pass : 기능의 캡슐화를 위해 여러 개 생성할 수 있음, 진입점 함수 호출이 목적
	// 패스의 이름은 있으나 없느나 상관없다. pass는 위쪽에 선언된 녀석부터 0번 인덱스로 지정된다.

	pass Alphablend
	{
		Alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
		
	}

	pass alphatest
	{
		alphatestenable = true;
		alphafunc = greater;
		alpharef = 0xc0;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN_TEMP();

	}
};