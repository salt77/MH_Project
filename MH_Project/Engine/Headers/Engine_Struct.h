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

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);	// �ؽ��� UV�� FLOAT�� 3���� ������ ǥ���ϰڴٴ� �ɼ�

	typedef struct tagVertex_Screen
	{
		_vec4		vPosition;
		_vec2		vTexUV;

	}VTXSCREEN;

	const _ulong FVF_SCREEN = D3DFVF_XYZRHW | D3DFVF_TEX1; // RHW ������ ������ ��ġ ���� ǥ���ϴ� �ڷ���


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

		LPD3DXMESH				pOriMesh;		// ��Ű���� ����Ǳ� �� ���� �ε���¸� �����ϱ� ���� �İ�ü

		_ulong					dwNumBones;		// �޽��� ���� ���� ����(�޽��� ���� �� ������ ����� ������ �ľ��ϱ� ����)

		_matrix*				pFrameOffSetMatrix;			// �ִϸ��̼��� ������ ���·� ����� ���� ���� �ִϸ��̼� ���

		_matrix**				ppCombinedTransformMatrix;	// ���� ���� CombinedTransformMatrix���� �ּҰ��� �����ϱ� ���� ���� ������
		_matrix*				pRenderingMatrix;			// ���� ��� = pFrameOffSetMatrix * (*ppCombinedTransformMatrix);

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
