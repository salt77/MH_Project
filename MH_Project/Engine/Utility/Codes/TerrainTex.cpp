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
										const _ulong& dwVtxItv)
{
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;
	m_dwFVF = FVF_TEX;

	m_pPos = new _vec3[m_dwVtxCnt];

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	/////////////////////////////////////////////////////////////////////
	//LPDIRECT3DTEXTURE9			pTexture = nullptr;

	//D3DXCreateTexture(m_pGraphicDev,
	//	129,
	//	129,
	//	1, // miplevel
	//	0,	// 텍스처의 용도, D3DUSAGE_RENDERTARGET
	//	D3DFMT_A8R8G8B8,
	//	D3DPOOL_MANAGED,
	//	&pTexture);

	//D3DLOCKED_RECT LockRect;
	//pTexture->LockRect(0, &LockRect, NULL, 0);

	//for (_int i = 0; i < 129; ++i)
	//{
	//	for (_int j = 0; j < 129; ++j)
	//	{
	//		_int	iIndex = i * 129 + j;

	//		*(((_ulong*)LockRect.pBits) + iIndex) = D3DCOLOR_ARGB(255, 255, 0, 0);
	//	}
	//}

	//pTexture->UnlockRect(0);
	//D3DXSaveTextureToFile(L"../Bin/Resources/Texture/Terrain/TestHeight.bmp", D3DXIFF_BMP, pTexture, NULL);
	

	_ulong		dwByte = 0;

	m_hFile = CreateFile(L"../Bin/Resource/Texture/Terrain/Height.bmp",
		GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	ReadFile(m_hFile, &m_fH, sizeof(BITMAPFILEHEADER), &dwByte, NULL);
	ReadFile(m_hFile, &m_iH, sizeof(BITMAPINFOHEADER), &dwByte, NULL);

	_ulong*		pPixel = new _ulong[m_iH.biWidth * m_iH.biHeight];

	ReadFile(m_hFile, pPixel, sizeof(_ulong) * m_iH.biWidth * m_iH.biHeight, &dwByte, NULL);

	CloseHandle(m_hFile);

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

			m_pPos[dwIndex] = pVertex[dwIndex].vPosition;

			pVertex[dwIndex].vTexUV = _vec2(_float(j) / (dwCntX - 1) * 20.f,
											_float(i) / (dwCntZ - 1) * 20.f);
		}
	}	
	m_pVB->Unlock();
	
	Safe_Delete_Array(pPixel);

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
										const _ulong& dwVtxItv)
{
	CTerrainTex*	pInstance = new CTerrainTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv)))
		Safe_Release(pInstance);

	return pInstance;
}

