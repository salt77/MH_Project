#include "BoxCollider.h"

USING(Engine)

Engine::CBoxCollider::CBoxCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_pIB(nullptr)
	, m_pVB(nullptr)
{

}

Engine::CBoxCollider::CBoxCollider(const CBoxCollider& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
	, m_matColMatrix(rhs.m_matColMatrix)
{
	for (_ulong i = 0; i < COL_END; ++i)
	{
		m_pTexture[i] = rhs.m_pTexture[i];
		m_pTexture[i]->AddRef();
	}
}

Engine::CBoxCollider::~CBoxCollider(void)
{
}


HRESULT Engine::CBoxCollider::Ready_Collider(_float vMinX, _float vMinY, _float vMinZ,
											_float vMaxX, _float vMaxY, _float vMaxZ,
											const _matrix * pColliderMatrix, COLLIDERTYPE eColliderType)
{
	// 메시 사이즈에 맞는 바운딩 박스를 만들기 가장 작은 좌표 값과 가장 큰 좌표 값을 만들어주는 함수
	//D3DXComputeBoundingBox(pPos, dwVtxCnt, sizeof(_vec3), &m_vMin, &m_vMax);
	//D3DXCreateBox(m_pGraphicDev, vMaxX, vMaxY, vMaxZ, &m_pSphere, nullptr);
	m_vMin.x = vMinX;
	m_vMin.y = vMinY;
	m_vMin.z = vMinZ;
	m_vMax.x = vMaxX;
	m_vMax.y = vMaxY;
	m_vMax.z = vMaxZ;

//#ifdef _DEBUG

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXCUBE) * 8,
		0, // 정적 버퍼(사이즈를 정해준 만큼 할당해서 사용한다는 뜻) - D3DUSAGE_DYNAMIC(동적 버퍼 : 파티클)
		FVF_CUBE,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL)))
		return E_FAIL;

	if (FAILED(m_pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 12,
		0, // 정적 버퍼(사이즈를 정해준 만큼 할당해서 사용한다는 뜻) - D3DUSAGE_DYNAMIC(동적 버퍼 : 파티클)
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL)))
		return E_FAIL;


	VTXCUBE*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 앞면
	pVertex[0].vPosition = _vec3(m_vMin.x, m_vMax.y, m_vMin.z);
	pVertex[0].vTexUV = pVertex[0].vPosition;

	pVertex[1].vPosition = _vec3(m_vMax.x, m_vMax.y, m_vMin.z);
	pVertex[1].vTexUV = pVertex[1].vPosition;

	pVertex[2].vPosition = _vec3(m_vMax.x, m_vMin.y, m_vMin.z);
	pVertex[2].vTexUV = pVertex[2].vPosition;

	pVertex[3].vPosition = _vec3(m_vMin.x, m_vMin.y, m_vMin.z);
	pVertex[3].vTexUV = pVertex[3].vPosition;

	// 뒷면
	pVertex[4].vPosition = _vec3(m_vMin.x, m_vMax.y, m_vMax.z);
	pVertex[4].vTexUV = pVertex[4].vPosition;

	pVertex[5].vPosition = _vec3(m_vMax.x, m_vMax.y, m_vMax.z);
	pVertex[5].vTexUV = pVertex[5].vPosition;

	pVertex[6].vPosition = _vec3(m_vMax.x, m_vMin.y, m_vMax.z);
	pVertex[6].vTexUV = pVertex[6].vPosition;

	pVertex[7].vPosition = _vec3(m_vMin.x, m_vMin.y, m_vMax.z);
	pVertex[7].vTexUV = pVertex[7].vPosition;

	m_pVB->Unlock();

	INDEX16*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// X+
	pIndex[0]._0 = 1;
	pIndex[0]._1 = 5;
	pIndex[0]._2 = 6;

	pIndex[1]._0 = 1;
	pIndex[1]._1 = 6;
	pIndex[1]._2 = 2;

	// X-
	pIndex[2]._0 = 4;
	pIndex[2]._1 = 0;
	pIndex[2]._2 = 3;

	pIndex[3]._0 = 4;
	pIndex[3]._1 = 3;
	pIndex[3]._2 = 7;

	// y+
	pIndex[4]._0 = 4;
	pIndex[4]._1 = 5;
	pIndex[4]._2 = 1;

	pIndex[5]._0 = 4;
	pIndex[5]._1 = 1;
	pIndex[5]._2 = 0;

	// y-
	pIndex[6]._0 = 3;
	pIndex[6]._1 = 2;
	pIndex[6]._2 = 6;

	pIndex[7]._0 = 3;
	pIndex[7]._1 = 6;
	pIndex[7]._2 = 7;

	// z+
	pIndex[8]._0 = 7;
	pIndex[8]._1 = 6;
	pIndex[8]._2 = 5;

	pIndex[9]._0 = 7;
	pIndex[9]._1 = 5;
	pIndex[9]._2 = 4;

	// z-
	pIndex[10]._0 = 0;
	pIndex[10]._1 = 1;
	pIndex[10]._2 = 2;

	pIndex[11]._0 = 0;
	pIndex[11]._1 = 2;
	pIndex[11]._2 = 3;

	m_pIB->Unlock();

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
		case Engine::COLTYPE_BOX_DAMAGED:
			*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 1.f * (1.f - i), 0.f, 1.f);
			break;
		case Engine::COLTYPE_BOX_HIT:
			*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 0.f, 1.f * (1.f - i), 1.f);
			break;
		}

		m_pTexture[i]->UnlockRect(0);
	}


	if (m_matColParts)
		m_matColMatrix = (*m_matColParts) * *pColliderMatrix;
	else
		m_matColMatrix = *pColliderMatrix;

	m_eColliderType = eColliderType;

//#endif

	return S_OK;
}

void Engine::CBoxCollider::Render_Collider(COLTYPE eType, const _matrix* pColliderMatrix)
{
	if (m_matColParts)
		m_matColMatrix = (*m_matColParts) * *pColliderMatrix;
	else
		m_matColMatrix = *pColliderMatrix;

//#ifdef _DEBUG
//	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matColMatrix);
//	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
//
//	m_pGraphicDev->SetTexture(0, m_pTexture[m_eColType]);
//	//m_pSphere->DrawSubset(0);
//
//	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXCUBE));
//	m_pGraphicDev->SetFVF(FVF_CUBE);
//	m_pGraphicDev->SetIndices(m_pIB);
//	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12); // 인덱스 버퍼도 활용하여 출력하는 함수
//
//	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
//#endif
}

Engine::CBoxCollider* Engine::CBoxCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
													_float vMinX, _float vMinY, _float vMinZ,
													_float vMaxX, _float vMaxY, _float vMaxZ, 
													const _matrix * pColliderMatrix, COLLIDERTYPE eColliderType)
{
	CBoxCollider*		pInstance = new	CBoxCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider(vMinX, vMinY, vMinZ, vMaxX, vMaxY, vMaxZ, pColliderMatrix, eColliderType)))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CComponent* Engine::CBoxCollider::Clone(void)
{
	return new CBoxCollider(*this);
}

void Engine::CBoxCollider::Free(void)
{
	for (_ulong i = 0; i < COL_END; ++i)
		Safe_Release(m_pTexture[i]);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	CComponent::Free();
}

