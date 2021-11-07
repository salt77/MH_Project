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
	// ��� ������ ��������� ��ȸ�ϸ鼭 �θ�� �ڽ�, �Ǵ� ���� ���踦 ���� �� �ֵ��� ��ĵ��� ���Ͽ� �ϼ��� ���� ��� ���¸� ������ִ� �Լ�
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

// �� ������ �����ϱ� ���� ����ü
//typedef struct _D3DXFRAME
//{
//	LPSTR                   Name;						// ���� �̸��� �����ϱ� ���� ����(�����ڵ� �ƴ� �ƽ�Ű ������ ���ڿ�)
//	D3DXMATRIX              TransformationMatrix;		// ���� ���� ���� ���� �����ϰ� �ִ� ���
//
//	LPD3DXMESHCONTAINER     pMeshContainer;				// �޽� �����̳�(����ƽ �޽�)
//
//	struct _D3DXFRAME       *pFrameSibling;				// ���� ���� �ּ�
//	struct _D3DXFRAME       *pFrameFirstChild;			// �ڽ� ���� �ּ�
//
//} D3DXFRAME, *LPD3DXFRAME;

// ���� ������ �޽��� �������� ������ �����ϴ� ����ü
//typedef struct _D3DXMESHCONTAINER
//{
//	LPSTR                   Name;				// �޽� �����̳��� �̸�(�����ڵ� �ƴ� �ƽ�Ű ������ ���ڿ�), ������ ��� �׸�, ���� ������� �ʴ´�.
//
//	D3DXMESHDATA            MeshData;			// �޽��� ������ �����ϰ�, �ش��ϴ� �޽� �İ�ü�� �����ϱ� ���� ����ü
//
//	LPD3DXMATERIAL          pMaterials;			// �޽��� ���� ������ �ؽ�ó �̸��� �����ϱ� ���� ����ü
//	LPD3DXEFFECTINSTANCE    pEffects;			// �޽��� ���� ������ ����Ʈ ������ �����ϱ� ���� �İ�ü(�츮�� ����� �� ����)
//	DWORD                   NumMaterials;		// ������ ���� == ������� ���� == �ؽ����� ����
//	DWORD                  *pAdjacency;			// ������ ������ �Ǵ� ������ ������ ���� �迭�� ���·� ������ ��, ù ��° �ּҸ� ����
//
//	LPD3DXSKININFO          pSkinInfo;			// ��Ű���� �����ϱ� ���� ���� �Լ��� �����ϴ� �İ�ü
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;	// ���� �޽������̳��� �ּҸ� �����ϱ� ���� ������
//
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;