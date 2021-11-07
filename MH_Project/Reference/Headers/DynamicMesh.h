#pragma once

#include "Component.h"
#include "HierachyLoader.h"
#include "AniCtrl.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicMesh : public CComponent
{
private:
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicMesh(const CDynamicMesh& rhs);
	virtual ~CDynamicMesh(void);

public:
	list<D3DXMESHCONTAINER_DERIVED*>	Get_MeshContainerList() { return m_MeshContainerList; }
	const	_float&						Get_AniFrameTime() { return m_pAniCtrl->Get_AniFrameTime(); }
	const	_double&					Get_AniFrameEndTime() { return m_pAniCtrl->Get_AniFrameEndTime(); }
	const	_float&						Get_AniSpeed() { return m_pAniCtrl->Get_AniSpeed(); }

public:
	void			Set_TrackSpeed(_float fSpeed) { m_pAniCtrl->Set_TrackSpeed(fSpeed); }
	void			Set_AnimationIndex(const _uint& iIndex);

public:
	void			Play_Animation(const _float& fTimeDelta);
	_bool			Is_AnimationSetFinish(const _float& fEndTime) { return m_pAniCtrl->Is_AnimationSetFinish(fEndTime); }

public:
	HRESULT		Ready_Meshes(const wstring pFilePath, const wstring pFileName);
	void		Render_Meshes(void);
	void		Render_Meshes(LPD3DXEFFECT& pEffect, const wstring& wstrNoRender = L"");
	void		Render_Meshes(LPD3DXEFFECT& pEffect, map<const wstring, _bool>	mapActiveParts);

private:
	// 모든 뼈들을 재귀적으로 순회하면서 부모와 자식, 또는 형제 관계를 맺을 수 있도록 행렬들을 곱하여 완성된 월드 행렬 상태를 만들어주는 함수
	void					Update_FrameMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix);
	void					SetUp_FrameMatrices(D3DXFRAME_DERIVED* pFrame);

private:
	D3DXFRAME*									m_pRootFrame;
	CHierachyLoader*							m_pLoader;
	list<D3DXMESHCONTAINER_DERIVED*>			m_MeshContainerList;
	CAniCtrl*									m_pAniCtrl;

public:
	static CDynamicMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pFilePath, const wstring pFileName);
	virtual CComponent*			Clone(void);

private:
	virtual void	Free(void);
};

END

// 뼈 정보를 저장하기 위한 구조체
//typedef struct _D3DXFRAME
//{
//	LPSTR                   Name;						// 뼈의 이름을 저장하기 위한 변수(유니코드 아닌 아스키 상태의 문자열)
//	D3DXMATRIX              TransformationMatrix;		// 뼈의 최초 상태 값을 보관하고 있는 행렬
//
//	LPD3DXMESHCONTAINER     pMeshContainer;				// 메쉬 컨테이너(스태틱 메쉬)
//
//	struct _D3DXFRAME       *pFrameSibling;				// 형제 뼈의 주소
//	struct _D3DXFRAME       *pFrameFirstChild;			// 자식 뼈의 주소
//
//} D3DXFRAME, *LPD3DXFRAME;

// 뼈를 제외한 메쉬의 실제적인 정보를 저장하는 구조체
//typedef struct _D3DXMESHCONTAINER
//{
//	LPSTR                   Name;				// 메쉬 컨테이너의 이름(유니코드 아닌 아스키 상태의 문자열), 하지만 없어도 그만, 거의 사용하지 않는다.
//
//	D3DXMESHDATA            MeshData;			// 메쉬의 종류를 선택하고, 해당하는 메쉬 컴객체를 보관하기 위한 구조체
//
//	LPD3DXMATERIAL          pMaterials;			// 메쉬의 재질 정보와 텍스처 이름을 보관하기 위한 구조체
//	LPD3DXEFFECTINSTANCE    pEffects;			// 메쉬가 지닌 고유의 이펙트 정보를 보관하기 위한 컴객체(우리는 사용할 수 없다)
//	DWORD                   NumMaterials;		// 재질의 개수 == 서브셋의 개수 == 텍스쳐의 개수
//	DWORD                  *pAdjacency;			// 인접한 폴리곤 또는 정점의 정보를 동적 배열의 형태로 보관한 뒤, 첫 번째 주소만 저장
//
//	LPD3DXSKININFO          pSkinInfo;			// 스키닝을 수행하기 위한 각종 함수를 제공하는 컴객체
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;	// 다음 메쉬컨테이너의 주소를 저장하기 위한 포인터
//
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;