#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef	struct tagObjectInfo
	{
		_int		iHp;
		_int		iMaxHp;

	} TAGINFO;

	typedef	struct tagPlayerInfo
	{
		tagObjectInfo	tagInfo;

		_int			iStamina;
		_int			iMaxStamina;
		_int			iSkillPoint;
		_int			iMaxSkillPoint;

	} PL_INFO;

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


	typedef struct D3DXFRAME_DERIVED : public D3DXFRAME
	{
		_matrix			CombinedTransformMatrix;

	}D3DXFRAME_DERIVED;


	typedef struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
	{
		LPDIRECT3DTEXTURE9*		ppTexture;
		LPDIRECT3DTEXTURE9*		ppNormalTexture;
		LPDIRECT3DTEXTURE9*		ppSpecularTexture;

		LPD3DXMESH				pOriMesh;		// 스키닝이 적용되기 전 최초 로드상태를 보관하기 위한 컴객체

		_ulong					dwNumBones;		// 메쉬가 지닌 뼈의 개수(메쉬가 지닌 뼈 상태의 행렬의 개수를 파악하기 위함)

		_matrix*				pFrameOffSetMatrix;			// 애니메이션을 포함한 형태로 만들기 위한 최초 애니메이션 행렬

		_matrix**				ppCombinedTransformMatrix;	// 뼈가 지닌 CombinedTransformMatrix들의 주소값을 보관하기 위한 이중 포인터
		_matrix*				pRenderingMatrix;			// 최종 행렬 = pFrameOffSetMatrix * (*ppCombinedTransformMatrix);

		wstring*				pwstrTextureName;

	}D3DXMESHCONTAINER_DERIVED;


	typedef struct PARSINGFORCOLLIDER
	{
//#ifdef _AFX
//		CString wstrColName;
//		CString wstrBoneName;
//#else
//		
//#endif
		wstring	wstrColName;
		wstring wstrBoneName;
		_float	fRadius = 0.f;

	} PS_Collider;
}
#endif // Engine_Struct_h__
