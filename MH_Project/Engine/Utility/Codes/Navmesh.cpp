#include "Navmesh.h"

USING(Engine)

CNavmesh::CNavmesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CNavmesh::CNavmesh(const CNavmesh & rhs)
	: CVIBuffer(rhs)
{
}

CNavmesh::~CNavmesh()
{
}


HRESULT CNavmesh::Ready_Buffer(const _vec3 & fFirstIndex, const _vec3 & fSecondIndex, const _vec3 & fThirdIndex)
{
	m_dwTriCnt = 1;
	m_dwVtxCnt = 3;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;
	m_dwFVF = FVF_TEX;

	m_pPos = new _vec3[m_dwVtxCnt];

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXTEX*		pVertex = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = fFirstIndex;
	m_pPos[0] = pVertex[0].vPosition;
	//pVertex[0].vTexUV = _vec2(_float(j) / (dwCntX - 1) * 20.f,
	//	_float(i) / (dwCntZ - 1) * 20.f);

	pVertex[1].vPosition = fSecondIndex;
	m_pPos[1] = pVertex[1].vPosition;

	pVertex[2].vPosition = fThirdIndex;
	m_pPos[2] = pVertex[2].vPosition;

	m_pVB->Unlock();

	return S_OK;
}

void CNavmesh::Render_Buffer(void)
{
	// 버텍스 정보가 보관되어 있는 객체를 디바이스 객체와 연결(링크)하는 함수
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetFVF(m_dwFVF);

	//m_pGraphicDev->SetIndices(m_pIB);

	m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwVtxCnt); // 버텍스 버퍼만을 이용하여 출력하는 함수
	//m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt); // 인덱스 버퍼도 활용하여 출력하는 함수
}

CVIBuffer * CNavmesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 & fFirstIndex, const _vec3 & fSecondIndex, const _vec3 & fThirdIndex)
{
	CNavmesh*	pInstance = new CNavmesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(fFirstIndex, fSecondIndex, fThirdIndex)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CNavmesh::Clone(void)
{
	return new CNavmesh(*this);
}

void CNavmesh::Free()
{
	if (false == m_bClone)
		Safe_Delete_Array(m_pPos);

	CVIBuffer::Free();
}
