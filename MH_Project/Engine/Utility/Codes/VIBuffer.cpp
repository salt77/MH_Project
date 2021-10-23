#include "VIBuffer.h"

USING(Engine)

Engine::CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_dwVtxSize(0)
	, m_dwVtxCnt(0)
	, m_dwFVF(0)
	, m_dwIdxSize(0)
	, m_dwTriCnt(0)
{

}

Engine::CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_dwVtxSize(rhs.m_dwVtxSize)
	, m_dwVtxCnt(rhs.m_dwVtxCnt)
	, m_dwFVF(rhs.m_dwFVF)
	, m_dwIdxSize(rhs.m_dwIdxSize)
	, m_dwTriCnt(rhs.m_dwTriCnt)
	, m_IdxFmt(rhs.m_IdxFmt)
	, m_dwVtxCntX(rhs.m_dwVtxCntX)
	, m_dwVtxCntZ(rhs.m_dwVtxCntZ)
{
	m_pIB->AddRef();
	m_pVB->AddRef();
}

Engine::CVIBuffer::~CVIBuffer(void)
{

}

HRESULT Engine::CVIBuffer::Ready_Buffer(void)
{
	if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_dwVtxSize * m_dwVtxCnt,
												0, // 정적 버퍼(사이즈를 정해준 만큼 할당해서 사용한다는 뜻) - D3DUSAGE_DYNAMIC(동적 버퍼 : 파티클)
												m_dwFVF,
												D3DPOOL_MANAGED,
												&m_pVB,
												NULL)))
	return E_FAIL;

	if (FAILED(m_pGraphicDev->CreateIndexBuffer(m_dwIdxSize * m_dwTriCnt,
												0, // 정적 버퍼(사이즈를 정해준 만큼 할당해서 사용한다는 뜻) - D3DUSAGE_DYNAMIC(동적 버퍼 : 파티클)
												m_IdxFmt,
												D3DPOOL_MANAGED,
												&m_pIB,
												NULL)))
												return E_FAIL;

	return S_OK;
}

void Engine::CVIBuffer::Render_Buffer(void)
{
	// 버텍스 정보가 보관되어 있는 객체를 디바이스 객체와 연결(링크)하는 함수
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetFVF(m_dwFVF);

	m_pGraphicDev->SetIndices(m_pIB);

	// m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwVtxCnt); // 버텍스 버퍼만을 이용하여 출력하는 함수
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt); // 인덱스 버퍼도 활용하여 출력하는 함수
}

void Engine::CVIBuffer::Free(void)
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
	
	CComponent::Free();
}

