#include "TerrainTex.h"

USING(Engine)

Engine::CTerrainTex::CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
	, m_pPos(nullptr)
{

}

Engine::CTerrainTex::CTerrainTex(const CTerrainTex& rhs)
	: CVIBuffer(rhs)
	, m_hFile(rhs.m_hFile)
	, m_fH(rhs.m_fH)
	, m_iH(rhs.m_iH)
	, m_pPos(rhs.m_pPos)
	
{

}

Engine::CTerrainTex::~CTerrainTex(void)
{

}

// 이거 나중에 쓰니까 걱정마세요 호호호
void CTerrainTex::Copy_Indices(INDEX32 * pIndex, const _ulong & dwTriCnt)
{
	Engine::INDEX32*		pOriginalIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pOriginalIndex, NULL);

	memcpy(pOriginalIndex, pIndex, sizeof(INDEX32) * dwTriCnt);

	m_pIB->Unlock();

	m_dwTriCnt = dwTriCnt;
}

HRESULT Engine::CTerrainTex::Ready_Buffer(const _ulong& dwCntX,
										const _ulong& dwCntZ, 
										const _ulong& dwVtxItv, 
										const _uint& iGrass)
{
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_dwVtxCntX = dwCntX;
	m_dwVtxCntZ = dwCntZ;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;
	m_dwFVF = FVF_TEX;

	m_pPos = new _vec3[m_dwVtxCnt];

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXTEX*		pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	_ulong	dwIndex = 0;

	for (_ulong i = 0; i < dwCntZ; ++i)
	{
		for (_ulong j = 0; j < dwCntX; ++j)
		{
			dwIndex = i * dwCntX + j;

			pVertex[dwIndex].vPosition = _vec3(_float(j) * dwVtxItv, 
										/*(pPixel[dwIndex] & 0x000000ff) / 20.f*/0.f,
										_float(i) * dwVtxItv);
			if (0 == iGrass)
				pVertex[dwIndex].vPosition -= _vec3(30.f, 0.f, 20.f);
			else if (1 == iGrass)
				pVertex[dwIndex].vPosition -= _vec3(103.f, 0.f, 20.f);
			else
				pVertex[dwIndex].vPosition -= _vec3(-7.f, 0.f, 20.f);

			m_pPos[dwIndex] = pVertex[dwIndex].vPosition;

			if (0 == iGrass)
			{
				pVertex[dwIndex].vTexUV = _vec2(_float(j) / (dwCntX - 1) * 50.f,
					_float(i) / (dwCntZ - 1) * 50.f);
			}
			else if (1 == iGrass)
			{
				pVertex[dwIndex].vTexUV = _vec2(_float(j) / (dwCntX - 1) * 50.f,
					_float(i) / (dwCntZ - 1) * 15.f);
			}
			else
			{
				pVertex[dwIndex].vTexUV = _vec2(_float(j) / (dwCntX - 1) * 50.f,
					_float(i) / (dwCntZ - 1) * 15.f);
			}
		}
	}	
	m_pVB->Unlock();
	
	//Safe_Delete_Array(pPixel);

	INDEX32*		pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_ulong	dwTriIndex = 0;

	for (_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			dwIndex = i * dwCntX + j;

			// 오른쪽 위
			pIndex[dwTriIndex]._0 = dwIndex + dwCntX;
			pIndex[dwTriIndex]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriIndex]._2 = dwIndex + 1;
			dwTriIndex++;

			// 왼쪽 아래
			pIndex[dwTriIndex]._0 = dwIndex + dwCntX;
			pIndex[dwTriIndex]._1 = dwIndex + 1;
			pIndex[dwTriIndex]._2 = dwIndex;
			dwTriIndex++;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

void Engine::CTerrainTex::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

void Engine::CTerrainTex::Free(void)
{
	if (false == m_bClone)
		Safe_Delete_Array(m_pPos);

	CVIBuffer::Free();
}

CComponent* Engine::CTerrainTex::Clone(void)
{
	return new CTerrainTex(*this);
}

CTerrainTex* Engine::CTerrainTex::Create(LPDIRECT3DDEVICE9 pGraphicDev,
										const _ulong& dwCntX,
										const _ulong& dwCntZ,
										const _ulong& dwVtxItv,
										const _uint& iGrass)
{
	CTerrainTex*	pInstance = new CTerrainTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv, iGrass)))
		Safe_Release(pInstance);

	return pInstance;
}

