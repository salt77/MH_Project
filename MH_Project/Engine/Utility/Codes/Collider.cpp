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


void CCollider::Set_MatrixInterpolX(_float fX)
{
	m_fInterpolX = fX;

	m_vPos.x = m_matColMatrix._41;
	m_vPos.y = m_matColMatrix._42;
	m_vPos.z = m_matColMatrix._43;

	_matrix		matWorld;
	m_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);

	D3DXMatrixInverse(&matWorld, nullptr, &matWorld);
	D3DXVec3TransformCoord(&m_vPos, &m_vPos, &matWorld);

	m_vPos.x += m_fInterpolX;
}

void CCollider::Set_MatrixInterpolY(_float fY)
{
	m_fInterpolY = fY;

	m_vPos.x = m_matColMatrix._41;
	m_vPos.y = m_matColMatrix._42;
	m_vPos.z = m_matColMatrix._43;

	_matrix		matWorld;
	m_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);

	D3DXMatrixInverse(&matWorld, nullptr, &matWorld);
	D3DXVec3TransformCoord(&m_vPos, &m_vPos, &matWorld);

	m_vPos.y += m_fInterpolY;
}

void CCollider::Set_MatrixInterpolZ(_float fZ)
{
	m_fInterpolZ = fZ;

	m_vPos.x = m_matColMatrix._41;
	m_vPos.y = m_matColMatrix._42;
	m_vPos.z = m_matColMatrix._43;

	_matrix		matWorld;
	m_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);

	D3DXMatrixInverse(&matWorld, nullptr, &matWorld);
	D3DXVec3TransformCoord(&m_vPos, &m_vPos, &matWorld);

	m_vPos.z += m_fInterpolZ;
}

HRESULT CCollider::Ready_Collider(const _float& fRadius, const _matrix * pColliderMatrix, COLLIDERTYPE eColliderType)
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

		switch (eColliderType)
		{
		case Engine::COLTYPE_SPHERE_DAMAGED:
			*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 1.f * (1.f - i), 0.f, 1.f);
			break;
		case Engine::COLTYPE_SPHERE_HIT:
			*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 0.f, 1.f * (1.f - i), 1.f);
			break;
		}

		m_pTexture[i]->UnlockRect(0);
	}

	if (m_matColParts)
		m_matColMatrix = (*m_matColParts) * *pColliderMatrix;
	else
		m_matColMatrix = *pColliderMatrix;

	memcpy(&m_vCenter, &m_matColMatrix._41, sizeof(_vec3));

	m_fRadius = fRadius;
	m_eColliderType = eColliderType;

	return S_OK;
}

void CCollider::LateUpdate_Collider(const _matrix * pColliderMatrix)
{
	if (m_matColParts)
		m_matColMatrix = (*m_matColParts) * *pColliderMatrix;
	else
		m_matColMatrix = *pColliderMatrix;

	memcpy(&m_vCenter, &m_matColMatrix._41, sizeof(_vec3));
}

void CCollider::Render_Collider(COLTYPE eType, const _matrix * pColliderMatrix)
{
	if (m_matColParts)
		m_matColMatrix = (*m_matColParts) * *pColliderMatrix;
	else
		m_matColMatrix = *pColliderMatrix;

	memcpy(&m_vCenter, &m_matColMatrix._41, sizeof(_vec3));

	//if (0.f != m_vPos.x)
	//{
	//	m_matColMatrix._41 = m_vPos.x * m_matColMatrix._41;
	//}
	//else if (0.f != m_vPos.y)
	//{
	//	m_matColMatrix._42 = m_vPos.y * m_matColMatrix._42;
	//}
	//else if (0.f != m_vPos.z)
	//{
	//	m_matColMatrix._43 = m_vPos.z * m_matColMatrix._43;
	//}

//#ifdef _DEBUG
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matColMatrix);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetTexture(0, m_pTexture[m_eColType]);
	m_pSphere->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
//#endif
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fRadius, const _matrix * pColliderMatrix, COLLIDERTYPE eColliderType)
{
	CCollider*	pInstance = new	CCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider(fRadius, pColliderMatrix, eColliderType)))
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
