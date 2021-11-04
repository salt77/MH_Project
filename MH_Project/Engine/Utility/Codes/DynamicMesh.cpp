#include "DynamicMesh.h"

USING(Engine)

CDynamicMesh::CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_pLoader(nullptr)
	, m_pAniCtrl(nullptr)
{
}

CDynamicMesh::CDynamicMesh(const CDynamicMesh & rhs)
	: CComponent(rhs)
	, m_pRootFrame(rhs.m_pRootFrame)
	, m_pLoader(rhs.m_pLoader)
	, m_MeshContainerList(rhs.m_MeshContainerList)
{
	m_pAniCtrl = CAniCtrl::Create(*rhs.m_pAniCtrl);
}

CDynamicMesh::~CDynamicMesh()
{
}


void CDynamicMesh::Set_AnimationIndex(const _uint & iIndex)
{
	m_pAniCtrl->Set_AnimationIndex(iIndex);
}

void CDynamicMesh::Play_Animation(const _float & fTimeDelta)
{
	m_pAniCtrl->Play_Animation(fTimeDelta);

	_matrix		matTemp;
	Update_FrameMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, D3DXMatrixIdentity(&matTemp));
}

HRESULT CDynamicMesh::Ready_Meshes(const wstring pFilePath, const wstring pFileName)
{
	_tchar	szFullPath[MAX_PATH] = L"";
	lstrcpy(szFullPath, pFilePath.c_str());
	lstrcat(szFullPath, pFileName.c_str());

	m_pLoader = CHierachyLoader::Create(m_pGraphicDev, pFilePath);
	NULL_CHECK_RETURN(m_pLoader, E_FAIL);

	LPD3DXANIMATIONCONTROLLER		pAniCtrl = nullptr;

	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath,
		D3DXMESH_MANAGED,
		m_pGraphicDev,
		m_pLoader, // HierarchyLoader
		NULL,
		&m_pRootFrame,
		&pAniCtrl)))	// AniCtrl
		return E_FAIL;

	m_pAniCtrl = CAniCtrl::Create(pAniCtrl);
	NULL_CHECK_RETURN(m_pAniCtrl, E_FAIL);

	Safe_Release(pAniCtrl);

	_matrix		matTemp;

	Update_FrameMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, D3DXMatrixIdentity(&matTemp));

	SetUp_FrameMatrices((D3DXFRAME_DERIVED*)m_pRootFrame);

	return S_OK;
}

void CDynamicMesh::Render_Meshes(void)
{
	for (auto& iter : m_MeshContainerList)
	{
		D3DXMESHCONTAINER_DERIVED*		pDerivedMeshContainer = iter;

		for (_ulong i = 0; i < pDerivedMeshContainer->dwNumBones; ++i)
			pDerivedMeshContainer->pRenderingMatrix[i] = pDerivedMeshContainer->pFrameOffSetMatrix[i]
			* *pDerivedMeshContainer->ppCombinedTransformMatrix[i];

		void*	pSrcVtx = nullptr; // 고정 불변의 메쉬 정점 정보
		void*	pDestVtx = nullptr; // 애니메이션 적용에 따른 변환된 메쉬 정점 정보

		pDerivedMeshContainer->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pDerivedMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

		//소프트웨어 스키닝을 수행하는 함수(스키닝뿐 아니라 애니메이션 변경 시, 뼈와 정점들의 정보도 동시에 변경해줌)
		pDerivedMeshContainer->pSkinInfo->UpdateSkinnedMesh(pDerivedMeshContainer->pRenderingMatrix, NULL, pSrcVtx, pDestVtx);

		for (_ulong i = 0; i < pDerivedMeshContainer->NumMaterials; ++i)
		{
			m_pGraphicDev->SetTexture(0, pDerivedMeshContainer->ppTexture[i]);
			pDerivedMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pDerivedMeshContainer->pOriMesh->UnlockVertexBuffer();
		pDerivedMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}
}

void CDynamicMesh::Render_Meshes(LPD3DXEFFECT & pEffect, const wstring& wstrNoRender)
{
	for (auto& iter : m_MeshContainerList)
	{
		D3DXMESHCONTAINER_DERIVED*		pDerivedMeshContainer = iter;

		for (_ulong i = 0; i < pDerivedMeshContainer->dwNumBones; ++i)
			pDerivedMeshContainer->pRenderingMatrix[i] = pDerivedMeshContainer->pFrameOffSetMatrix[i]
			* *pDerivedMeshContainer->ppCombinedTransformMatrix[i];

		void*	pSrcVtx = nullptr; // 고정 불변의 메쉬 정점 정보
		void*	pDestVtx = nullptr; // 애니메이션 적용에 따른 변환된 메쉬 정점 정보

		pDerivedMeshContainer->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pDerivedMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

		//소프트웨어 스키닝을 수행하는 함수(스키닝뿐 아니라 애니메이션 변경 시, 뼈와 정점들의 정보도 동시에 변경해줌)
		pDerivedMeshContainer->pSkinInfo->UpdateSkinnedMesh(pDerivedMeshContainer->pRenderingMatrix, NULL, pSrcVtx, pDestVtx);

		for (_ulong i = 0; i < pDerivedMeshContainer->NumMaterials; ++i)
		{
			if (L"" != wstrNoRender)
			{
				if (pDerivedMeshContainer->pwstrTextureName[i] == wstrNoRender)
				{
					continue;
				}
			}
			pEffect->SetTexture("g_BaseTexture", pDerivedMeshContainer->ppTexture[i]);
			if (pDerivedMeshContainer->ppNormalTexture[i])
				pEffect->SetTexture("g_NormalTexture", pDerivedMeshContainer->ppNormalTexture[i]);
			pEffect->CommitChanges();

			pDerivedMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pDerivedMeshContainer->pOriMesh->UnlockVertexBuffer();
		pDerivedMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}
}

void CDynamicMesh::Render_Meshes(LPD3DXEFFECT & pEffect, map<const wstring, _bool>	mapActiveParts)
{
	for (auto& iter : m_MeshContainerList)
	{
		D3DXMESHCONTAINER_DERIVED*		pDerivedMeshContainer = iter;

		for (_ulong i = 0; i < pDerivedMeshContainer->dwNumBones; ++i)
			pDerivedMeshContainer->pRenderingMatrix[i] = pDerivedMeshContainer->pFrameOffSetMatrix[i]
			* *pDerivedMeshContainer->ppCombinedTransformMatrix[i];

		void*	pSrcVtx = nullptr; // 고정 불변의 메쉬 정점 정보
		void*	pDestVtx = nullptr; // 애니메이션 적용에 따른 변환된 메쉬 정점 정보

		pDerivedMeshContainer->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pDerivedMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

		//소프트웨어 스키닝을 수행하는 함수(스키닝뿐 아니라 애니메이션 변경 시, 뼈와 정점들의 정보도 동시에 변경해줌)
		pDerivedMeshContainer->pSkinInfo->UpdateSkinnedMesh(pDerivedMeshContainer->pRenderingMatrix, NULL, pSrcVtx, pDestVtx);

		for (_ulong i = 0; i < pDerivedMeshContainer->NumMaterials; ++i)
		{
			_bool	bRenderSkip = false;
			map<const wstring, _bool>::iterator		iter = mapActiveParts.begin();

			for (; iter != mapActiveParts.end(); ++iter)
			{
				if (!iter->second && 
					pDerivedMeshContainer->pwstrTextureName[i] == iter->first)
				{
					bRenderSkip = true;

					continue;
				}
			}

			if (!bRenderSkip)
			{
				pEffect->SetTexture("g_BaseTexture", pDerivedMeshContainer->ppTexture[i]);
				if (pDerivedMeshContainer->ppNormalTexture[i])
					pEffect->SetTexture("g_NormalTexture", pDerivedMeshContainer->ppNormalTexture[i]);
				pEffect->CommitChanges();

				pDerivedMeshContainer->MeshData.pMesh->DrawSubset(i);
			}
		}

		pDerivedMeshContainer->pOriMesh->UnlockVertexBuffer();
		pDerivedMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}
}

void CDynamicMesh::Update_FrameMatrices(D3DXFRAME_DERIVED * pFrame, const _matrix * pParentMatrix)
{
	if (nullptr == pFrame)
		return;

	pFrame->CombinedTransformMatrix = pFrame->TransformationMatrix * (*pParentMatrix);

	if (nullptr != pFrame->pFrameSibling)
		Update_FrameMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pParentMatrix);

	if (nullptr != pFrame->pFrameFirstChild)
		Update_FrameMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, &pFrame->CombinedTransformMatrix);
}

void CDynamicMesh::SetUp_FrameMatrices(D3DXFRAME_DERIVED * pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED*	pDerivedMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		for (_ulong i = 0; i < pDerivedMeshContainer->dwNumBones; ++i)
		{
			const char* pBoneName = pDerivedMeshContainer->pSkinInfo->GetBoneName(i);
			D3DXFRAME_DERIVED* pDerivedFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pBoneName);

			pDerivedMeshContainer->ppCombinedTransformMatrix[i] = &pDerivedFrame->CombinedTransformMatrix;
		}

		m_MeshContainerList.push_back(pDerivedMeshContainer);
	}

	if (nullptr != pFrame->pFrameSibling)
		SetUp_FrameMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameSibling);

	if (nullptr != pFrame->pFrameFirstChild)
		SetUp_FrameMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);
}

CDynamicMesh * CDynamicMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring pFilePath, const wstring pFileName)
{
	CDynamicMesh*	pInstance = new CDynamicMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CDynamicMesh::Clone(void)
{
	return new CDynamicMesh(*this);
}

void CDynamicMesh::Free(void)
{
	Safe_Release(m_pAniCtrl);

	if (false == m_bClone)
	{
		m_pLoader->DestroyFrame(m_pRootFrame);
		Safe_Release(m_pLoader);
	}

	CComponent::Free();
}
