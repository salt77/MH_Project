#include "CubeTex.h"

USING(Engine)

Engine::CCubeTex::CCubeTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{

}

Engine::CCubeTex::CCubeTex(const CCubeTex& rhs)
	: CVIBuffer(rhs)
{

}

Engine::CCubeTex::~CCubeTex(void)
{

}

HRESULT Engine::CCubeTex::Ready_Buffer(void)
{
	m_dwVtxCnt = 8;
	m_dwTriCnt = 12;
	m_dwVtxSize = sizeof(VTXCUBE);
	m_dwFVF = FVF_CUBE;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCUBE*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	
	// �ո�
	pVertex[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);
	pVertex[0].vTexUV = pVertex[0].vPosition;

	pVertex[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	pVertex[1].vTexUV = pVertex[1].vPosition;

	pVertex[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	pVertex[2].vTexUV = pVertex[2].vPosition;

	pVertex[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	pVertex[3].vTexUV = pVertex[3].vPosition;

	// �޸�
	pVertex[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	pVertex[4].vTexUV = pVertex[4].vPosition;

	pVertex[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	pVertex[5].vTexUV = pVertex[5].vPosition;

	pVertex[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	pVertex[6].vTexUV = pVertex[6].vPosition;

	pVertex[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
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

	return S_OK;
}

void Engine::CCubeTex::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

void Engine::CCubeTex::Free(void)
{
	CVIBuffer::Free();
}

CCubeTex* Engine::CCubeTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCubeTex*	pInstance = new CCubeTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* Engine::CCubeTex::Clone(void)
{
	return new CCubeTex(*this);
}

