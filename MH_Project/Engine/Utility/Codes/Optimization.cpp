#include "Optimization.h"

USING(Engine)

Engine::COptimization::COptimization(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_pFrustum(nullptr)
	, m_pQuadTree(nullptr)
{

}

Engine::COptimization::COptimization(const COptimization& rhs)
	: CComponent(rhs)
	, m_pFrustum(rhs.m_pFrustum)
	, m_pQuadTree(rhs.m_pQuadTree)
{

}

Engine::COptimization::~COptimization(void)
{

}

HRESULT Engine::COptimization::Ready_Optimization(_bool bChoice, const _ulong& dwCntX, const _ulong& dwCntZ)
{
	m_pFrustum = CFrustum::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(m_pFrustum, E_FAIL);

	if (false == bChoice)
		return S_OK;

	// ���� Ʈ��
	m_pQuadTree = CQuadTree::Create(dwCntX, dwCntZ);
	NULL_CHECK_RETURN(m_pQuadTree, E_FAIL);

	return S_OK;
}

Engine::_bool Engine::COptimization::Isin_Frustum_ForObject(const _vec3* pWorldPos, const _float& fRadius)
{
	return m_pFrustum->Isin_Frustum_ForObject(pWorldPos, fRadius);
}

Engine::COptimization* Engine::COptimization::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool bChoice, const _ulong& dwCntX, const _ulong& dwCntZ)
{
	COptimization*		pInstance = new COptimization(pGraphicDev);

	if (FAILED(pInstance->Ready_Optimization(bChoice, dwCntX, dwCntZ)))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CComponent* Engine::COptimization::Clone(void)
{
	return new COptimization(*this);
}

void Engine::COptimization::Free(void)
{
	if (false == m_bClone)
	{
		Safe_Release(m_pFrustum);
		Safe_Release(m_pQuadTree);
	}

	CComponent::Free();
}

void Engine::COptimization::Isin_Frustum_ForTerrain(const _vec3* pVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ, INDEX32* pIndex, _ulong* pTriCnt)
{
	m_pFrustum->Isin_Frustum_ForTerrain(pVtxPos, dwCntX, dwCntZ, pIndex, pTriCnt, m_pQuadTree);
}

