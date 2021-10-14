#include "Texture.h"

USING(Engine)

Engine::CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)	
{
	
}

Engine::CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)	
{
	_uint	iContainerSize = rhs.m_vecTexture.size();

	m_vecTexture.reserve(iContainerSize);

	m_vecTexture = rhs.m_vecTexture;

	for (auto& iter : m_vecTexture)
		iter->AddRef();
}

Engine::CTexture::~CTexture(void)
{

}

HRESULT Engine::CTexture::Ready_Texture(const _tchar* pPath, 
										TEXTURETYPE eType, 
										const _uint& iCnt)
{
	m_vecTexture.reserve(iCnt);

	IDirect3DBaseTexture9*		pTexture = nullptr;
	
	for (_uint i = 0; i < iCnt; ++i)
	{
		TCHAR	szFileName[256] = L"";

		wsprintf(szFileName, pPath, i);

		switch (eType)
		{
		case TEX_NORMAL:
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture)))
				return E_FAIL;
			break;

		case TEX_CUBE:
			if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DCUBETEXTURE9*)&pTexture)))
				return E_FAIL;
			break;
		}

		m_vecTexture.push_back(pTexture);
	}

	return S_OK;
}

void Engine::CTexture::Set_Texture(const _uint& iIndex /*= 0*/)
{
	if (m_vecTexture.size() < iIndex)
		return; 

	// 텍스쳐를 그리기 하는 함수
	m_pGraphicDev->SetTexture(0, m_vecTexture[iIndex]);
}


CTexture* Engine::CTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
									const _tchar* pPath, 
									TEXTURETYPE eType, 
									const _uint& iCnt /*= 1*/)
{
	CTexture*		pInstance = new CTexture(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture(pPath, eType, iCnt)))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CTexture::Free(void)
{
	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();

	CComponent::Free();

}

CComponent* Engine::CTexture::Clone(void)
{
	return new CTexture(*this);
}

