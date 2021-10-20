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


HRESULT CCollider::Ready_Collider(const _float& fRadius, COLLIDERTYPE eColliderType)
{
	// 메시 사이즈에 맞는 바운딩 박스를 만들기 가장 작은 좌표 값과 가장 큰 좌표 값을 만들어주는 함수
	//D3DXComputeBoundingSphere(pPos, dwVtxCnt, sizeof(_vec3), &m_vCenter, &m_fRadius);
	D3DXCreateSphere(m_pGraphicDev, fRadius, 10, 10, &m_pSphere, nullptr);

	for (_ulong i = 0; i < COL_END; ++i)
	{
		D3DXCreateTexture(m_pGraphicDev,
			1,
			1,
			1,  // miplevel
			0,	// 텍스처의 용도, D3DUSAGE_RENDERTARGET
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			&m_pTexture[i]);

		D3DLOCKED_RECT LockRect;
		m_pTexture[i]->LockRect(0, &LockRect, NULL, 0);

		*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 1.f * (1.f - i), 0.f, 1.f);

		m_pTexture[i]->UnlockRect(0);
	}

	m_fRadius = fRadius;
	m_eColliderType = eColliderType;

	return S_OK;
}

void CCollider::Render_Collider(COLTYPE eType, const _matrix * pColliderMatrix)
{
	m_matColMatrix = *pColliderMatrix;

	if (m_matColParts)
		m_matColMatrix = (*m_matColParts) * m_matColMatrix;

	if (0.f != m_fInterpolX)
		m_matColMatrix._41 += m_fInterpolX;
	if (0.f != m_fInterpolY)
		m_matColMatrix._42 += m_fInterpolY;
	if (0.f != m_fInterpolZ)
		m_matColMatrix._43 += m_fInterpolZ;

#ifdef _DEBUG
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matColMatrix);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetTexture(0, m_pTexture[m_eColType]);
	m_pSphere->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fRadius, COLLIDERTYPE eColliderType)
{
	CCollider*	pInstance = new	CCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider(fRadius, eColliderType)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CCollider::Clone()
{
	return new CCollider(*this);
}

void CCollider::Free(void)
{
	for (_ulong i = 0; i < COL_END; ++i)
		Safe_Release(m_pTexture[i]);

	m_pSphere->Release();
	m_pSphere = 0;
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	CComponent::Free();
}
