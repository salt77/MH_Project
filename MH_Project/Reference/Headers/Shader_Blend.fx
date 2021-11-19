
texture		g_AlbedoTexture;

sampler AlbedoSampler = sampler_state
{
	texture = g_AlbedoTexture;

	mipfilter = linear;
	magfilter = linear;
};

texture		g_ShadeTexture;

sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;

	mipfilter = linear;
	magfilter = linear;
};

texture		g_SpecularTexture;

sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;

	mipfilter = linear;
	magfilter = linear;
};

struct PS_IN				
{
	float2			vTexUV		: TEXCOORD0;	
};
struct PS_OUT
{
	vector			vColor : COLOR0;
};

PS_OUT		PS_TEMP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vAlbedo = tex2D(AlbedoSampler, In.vTexUV);
	vector vShade = tex2D(ShadeSampler, In.vTexUV);
	vector vSpecular = tex2D(SpecularSampler, In.vTexUV);

	Out.vColor = vAlbedo * vShade + vSpecular;

	return Out;
}


technique Default_Device
{
	pass Directional
	{
		zwriteenable = false;		
		Alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = NULL;
		pixelshader = compile ps_3_0 PS_TEMP();

	}
};