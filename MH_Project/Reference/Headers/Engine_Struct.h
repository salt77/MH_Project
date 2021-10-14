#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagVertexColor
	{
		_vec3		vPosition;	// float * 3  : 4, 4, 4
		_ulong		dwColor;	// ulong      : 4

	}VTXCOL;

	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;


	typedef struct tagVertexTex
	{
		_vec3		vPosition;		// 12
		_vec2		vTexUV;			// 8

	}VTXTEX;

	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_TEX1;

	typedef struct tagVertexCube
	{
		_vec3		vPosition;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);	// 텍스쳐 UV를 FLOAT형 3개의 값으로 표현하겠다는 옵션

	typedef struct tagVertex_Screen
	{
		_vec4		vPosition;
		_vec2		vTexUV;

	}VTXSCREEN;

	const _ulong FVF_SCREEN = D3DFVF_XYZRHW | D3DFVF_TEX1; // RHW 윈도우 상태의 위치 값을 표현하는 자료형


	typedef struct INDEX16
	{
		_ushort		_0;
		_ushort		_1;
		_ushort		_2;

	}INDEX16;

	typedef struct INDEX32
	{
		_ulong		_0;
		_ulong		_1;
		_ulong		_2;

	}INDEX32;			

}
#endif // Engine_Struct_h__
