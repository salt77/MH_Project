#include "Collider.h"

USING(Engine)

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
{
}

//CCollider::CCollider(const CCollider & rhs)
//	: CComponent(rhs)
//	, m_pVB(rhs.m_pVB)
//	, m_pIB(rhs.m_pIB)
//	, m_vMin(rhs.m_vMin)
//	, m_vMax(rhs.m_vMax)
//	, m_matColMatrix(rhs.m_matColMatrix)
//	, m_pTexture(rhs.m_pTexture)
//{
//	m_pTexture->AddRef();
//}

CCollider::~CCollider()
{
}


HRESULT CCollider::Ready_Collider(/*const _vec3 * pPos, const _ulong & dwVtxCnt, const _ulong & dwStride, */const _float& fRadius)
{
	// 메시 사이즈에 맞는 바운딩 박스를 만들기 가장 작은 좌표 값과 가장 큰 좌표 값을 만들어주는 함수
	//D3DXComputeBoundingSphere(pPos, dwVtxCnt, sizeof(_vec3), &m_vCenter, &m_fRadius);
	D3DXCreateSphere(m_pGraphicDev, fRadius, 10, 10, &m_pSphere, nullptr);

	D3DXCreateTexture(m_pGraphicDev,
		1,
		1,
		1,  // miplevel
		0,	// 텍스처의 용도, D3DUSAGE_RENDERTARGET
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&m_pTexture);

	D3DLOCKED_RECT LockRect;
	m_pTexture->LockRect(0, &LockRect, NULL, 0);

	*((_ulong*)LockRect.pBits) = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_pTexture->UnlockRect(0);

	return S_OK;
}

void CCollider::Render_Collider(const _matrix * pColliderMatrix)
{
	m_matColMatrix = *pColliderMatrix;

#ifdef _DEBUG
	m_pGraphicDev->SetTransform(D3DTS_WORLD, pColliderMatrix);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetTexture(0, m_pTexture);
	m_pSphere->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev/*, const _vec3 * pPos, const _ulong & dwVtxCnt, const _ulong & dwStride*/, const _float& fRadius)
{
	CCollider*	pInstance = new	CCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider(/*pPos, dwVtxCnt, dwStride, */fRadius)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CCollider::Clone()
{
	return new CCollider(*this);
}

void CCollider::Free(void)
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	CComponent::Free();
}
